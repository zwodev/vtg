import Foundation
import AVFoundation
import VideoToolbox
import Metal

private var currentIndex: Int = 0
private var condition = NSCondition()
private var bufferAvailable = false 
private var readIndex: Int = 0 

func getWriteIndex() -> Int {
    condition.lock()
    while !bufferAvailable {
        condition.wait()  
    }

    bufferAvailable = false
    let writeIndex = (readIndex + 1) % 2
    condition.unlock()
    return writeIndex
}

func swapBuffers() {
    condition.lock()
    readIndex = (readIndex + 1) % 2
    condition.unlock()
}

func getReadIndex() -> Int {
    condition.lock()
    let currentReadIndex = readIndex
    condition.unlock()
    return currentReadIndex
}

func signalBufferConsumed() {
    condition.lock()
    bufferAvailable = true 
    condition.signal()
    condition.unlock()
}

private var assetReader: AVAssetReader!
private var sampleBufferOutput: AVAssetReaderTrackOutput!
private var decompressionSession: VTDecompressionSession?

private var videoWidth: Int32 = 0
private var videoHeight: Int32 = 0
private var videoStartTime = CMTime.invalid
private var playbackStartTimeHost: DispatchTime?

private var metalDevice: MTLDevice!
private var metalTextures = [MTLTexture]()
private var currentTextureIndex = 0
private var metalTextureCache: CVMetalTextureCache?

private var decodeThread: Thread?
private var decodingShouldRun = true


private var lastDecodedFramePTSLock = NSLock()
private var lastDecodedFramePTS: CMTime = CMTime.invalid


private let lastErrorLock = NSRecursiveLock()
private var errorBuffer: UnsafeMutablePointer<CChar>?
private let errorQueue = DispatchQueue(label: "error.queue")
private var lastErrorString: String = ""


func setLastError(_ error: String) {
    lastErrorLock.lock()
    defer { lastErrorLock.unlock() }
    lastErrorString = error
}

func setupTextureCacheIfNeeded() {
    guard metalTextureCache == nil else { return }
    if let device = metalDevice {
        CVMetalTextureCacheCreate(kCFAllocatorDefault, nil, device, nil, &metalTextureCache)
    }
}

func createMetalTexture(from pixelBuffer: CVPixelBuffer, pixelFormat: MTLPixelFormat, planeIndex: Int) -> MTLTexture? {
    guard let cache = metalTextureCache else { return nil }

    let width = CVPixelBufferGetWidthOfPlane(pixelBuffer, planeIndex)
    let height = CVPixelBufferGetHeightOfPlane(pixelBuffer, planeIndex)
    var cvMetalTexture: CVMetalTexture?

    let status = CVMetalTextureCacheCreateTextureFromImage(
        kCFAllocatorDefault,
        cache,
        pixelBuffer,
        nil,
        pixelFormat,
        width,
        height,
        planeIndex,
        &cvMetalTexture)

    guard status == kCVReturnSuccess, let metalTexture = cvMetalTexture else { return nil }
    return CVMetalTextureGetTexture(metalTexture)
}

@_cdecl("swift_get_last_error")
public func swift_get_last_error() -> UnsafePointer<CChar>? {
    lastErrorLock.lock()
    defer { lastErrorLock.unlock() }

    guard !lastErrorString.isEmpty else { return nil }

    //let count = lastErrorString.utf8.count + 1
    //let buffer = UnsafeMutablePointer<CChar>.allocate(capacity: count)
    _ = lastErrorString.withCString { strcpy(errorBuffer, $0) }
    return UnsafePointer(errorBuffer)
}

@_cdecl("swift_get_video_size")
public func swift_get_video_size(widthOut: UnsafeMutablePointer<Int32>, heightOut: UnsafeMutablePointer<Int32>) {
    widthOut.pointee = videoWidth
    heightOut.pointee = videoHeight
}

func vtStatusDescription(_ status: OSStatus) -> String {
    switch status {
    case 0:
        return "No Error"
    case kVTVideoDecoderBadDataErr:
        return "Bad Data Error"
    case kVTVideoDecoderAuthorizationErr:
        return "Authorization Error"
    case kVTVideoDecoderUnsupportedDataFormatErr:
        return "Unsupported Data Format"
    case kVTVideoDecoderMalfunctionErr:
        return "Decoder Malfunction"
    default:
        return "Unknown error code \(status)"
    }
}

// Background decode loop (runs on dedicated thread)
private func decodeLoop() {
    autoreleasepool {
        while !Thread.current.isCancelled {           
            guard decompressionSession != nil, assetReader.status == .reading,
                  let sampleBuffer: CMSampleBuffer = sampleBufferOutput.copyNextSampleBuffer() else {
                    Thread.sleep(forTimeInterval: 0.1)
                continue
            }

            let status = VTDecompressionSessionDecodeFrame(
                decompressionSession!,
                sampleBuffer: sampleBuffer,
                flags: VTDecodeFrameFlags._EnableAsynchronousDecompression,
                //flags: [],
                frameRefcon: nil,
                infoFlagsOut: nil
            )

            if status != noErr {
                //print("DecodeFrame call failed: \(status) (\(vtStatusDescription(status)))")
            }
        }
    }
}

// MARK: Exported functions for Godot
@_cdecl("swift_initialize_decoder")
public func swift_initialize_decoder(
    videoPath: UnsafePointer<Int8>,
    metalTexturePtrs: UnsafeMutablePointer<UnsafeMutableRawPointer?>?,
    textureCount: Int32,
    width: Int32,
    height: Int32
) {

    errorBuffer = UnsafeMutablePointer<CChar>.allocate(capacity: 1024)

    if metalDevice == nil {
        metalDevice = MTLCreateSystemDefaultDevice()
    }
    metalTextures.removeAll()
    
    let pathStr = String(cString: videoPath)
    let url = URL(fileURLWithPath: pathStr)
    let asset = AVURLAsset(url: url)
    
    // Synchronous load video track
    var videoTrack: AVAssetTrack?
    var loadError: Error?
    let semaphore = DispatchSemaphore(value: 0)
    asset.loadTracks(withMediaType: .video) { tracks, error in
        videoTrack = tracks?.first
        loadError = error
        semaphore.signal()
    }
    _ = semaphore.wait(timeout: .distantFuture)
    
    guard let track = videoTrack, loadError == nil else {
        setLastError("No video track found or failed to load: \(String(describing: loadError))")
        return
    }
    
    // Synchronously load properties with async-await wrappers
    func syncLoad<T>(_ property: AVAsyncProperty<AVAssetTrack, T>) -> T? {
        var result: T?
        let group = DispatchGroup()
        group.enter()
        Task {
            do {
                result = try await track.load(property)
            } catch {
                setLastError("Failed to load property: \(error.localizedDescription)")
            }
            group.leave()
        }
        group.wait()
        return result
    }
    
    guard let naturalSize = syncLoad(.naturalSize),
          let preferredTransform = syncLoad(.preferredTransform) else {
        setLastError("Failed to load naturalSize or preferredTransform")
        return
    }
    
    let size = naturalSize.applying(preferredTransform)
    videoWidth = Int32(abs(size.width))
    videoHeight = Int32(abs(size.height))
    
    if metalTexturePtrs != nil && textureCount > 0 && width > 0 && height > 0 {
        for i in 0..<Int(textureCount) {
            guard let ptr = metalTexturePtrs!.advanced(by: i).pointee else {
                continue
            }
            metalTextures.append(Unmanaged<MTLTexture>.fromOpaque(ptr).takeUnretainedValue())
        }
      
        do {
            assetReader = try AVAssetReader(asset: asset)
            sampleBufferOutput = AVAssetReaderTrackOutput(track: track, outputSettings: nil)
            sampleBufferOutput.alwaysCopiesSampleData = false
            
            guard assetReader.canAdd(sampleBufferOutput) else {
                setLastError("Cannot add sample buffer output")
                return
            }
            assetReader.add(sampleBufferOutput)
            
            guard assetReader.startReading() else {
                setLastError("AssetReader failed to start reading")
                return
            }
        } catch {
            setLastError("AVAssetReader error: \(error.localizedDescription)")
            return
        }
        
        // Create decompression session
        func syncLoadTrackFormatDescriptions(_ track: AVAssetTrack) -> [CMFormatDescription]? {
            var result: [CMFormatDescription]?
            let group = DispatchGroup()
            group.enter()
            Task {
                do {
                    result = try await track.load(.formatDescriptions)
                } catch {
                    setLastError("Failed to load track formatDescriptions: \(error.localizedDescription)")
                }
                group.leave()
            }
            group.wait()
            return result
        }
        
        guard let formatDescriptions = syncLoadTrackFormatDescriptions(track),
              let firstDescription = formatDescriptions.first else {
            setLastError("No format descriptions available")
            return
        }
        let videoFormatDescription = firstDescription
        
        // Updated decode callback to copy decoded frames to Godot Metal textures
        let decompressionOutputCallback: VTDecompressionOutputCallback = {
            (
                decompressionOutputRefCon,
                sourceFrameRefCon,
                status,
                infoFlags,
                imageBuffer,
                presentationTimeStamp,
                presentationDuration
            ) in

            if status != noErr {
                print("Decode error: \(status) (\(vtStatusDescription(status)))")
                return
            }

            guard let pixelBuffer = imageBuffer else {
                print("Decode callback: no image buffer")
                return
            }

            setupTextureCacheIfNeeded()

            // Obtain Metal texture for plane 0 (assumes BGRA or similar, adjust if needed)
            guard let srcTexture = createMetalTexture(from: pixelBuffer, pixelFormat: .rgba8Unorm, planeIndex: 0) else {
                print("Failed to create source Metal texture from CVPixelBuffer")
                return
            }

            // Choose destination Godot Metal texture (cycle if multiple)
            guard metalTextures.count > 0 else {
                print("No Godot Metal textures available")
                return
            }

            let index = getWriteIndex()
            let dstTexture = metalTextures[index]

            // Copy using Metal GPU commands
            guard let commandQueue = metalDevice.makeCommandQueue(),
                let commandBuffer = commandQueue.makeCommandBuffer(),
                let blitEncoder = commandBuffer.makeBlitCommandEncoder() else {
                print("Failed to create Metal command queue or encoder")
                return
            }

            let width = min(srcTexture.width, dstTexture.width)
            let height = min(srcTexture.height, dstTexture.height)

            blitEncoder.copy(from: srcTexture,
                            sourceSlice: 0,
                            sourceLevel: 0,
                            sourceOrigin: MTLOrigin(x: 0, y: 0, z: 0),
                            sourceSize: MTLSize(width: width, height: height, depth: 1),
                            to: dstTexture,
                            destinationSlice: 0,
                            destinationLevel: 0,
                            destinationOrigin: MTLOrigin(x: 0, y: 0, z: 0))

            blitEncoder.endEncoding()
            commandBuffer.commit()
            commandBuffer.waitUntilCompleted()
            swapBuffers()
            //setLastError("\(presentationTimeStamp)")
            Thread.sleep(forTimeInterval: 0.016)
        }
        
        var callback = VTDecompressionOutputCallbackRecord(
            decompressionOutputCallback: decompressionOutputCallback,
            decompressionOutputRefCon: nil
        )
        
        let pixelBufferAttributes: CFDictionary = [
            kCVPixelBufferMetalCompatibilityKey: true,
            kCVPixelBufferWidthKey: NSNumber(value: width),
            kCVPixelBufferHeightKey: NSNumber(value: height),
            kCVPixelBufferPixelFormatTypeKey: NSNumber(value: kCVPixelFormatType_32BGRA)
        ] as CFDictionary
        
        var decompressionSessionLocal: VTDecompressionSession?
        let status = VTDecompressionSessionCreate(
            allocator: kCFAllocatorDefault,
            formatDescription: videoFormatDescription,
            decoderSpecification: nil,
            imageBufferAttributes: pixelBufferAttributes,
            outputCallback: &callback,
            decompressionSessionOut: &decompressionSessionLocal
        )
        decompressionSession = decompressionSessionLocal
        
        if status != noErr {
            setLastError("Failed to create VTDecompressionSession: \(status)")
            return
        }
        
        videoStartTime = CMTime.invalid
        playbackStartTimeHost = nil
        
        startDecodeThread()
    }
}

@_cdecl("swift_request_decode")
public func swift_request_decode(currentTimeNs: Int64, frameReadyOut: UnsafeMutablePointer<Int32>?) -> Int32 {
    let index = getReadIndex()
    if (currentIndex != index) {
        signalBufferConsumed()
        currentIndex = index
    }
    return Int32(index)
}

// Called at the end of swift_initialize_decoder after setup is complete
func startDecodeThread() {
    decodingShouldRun = true
    signalBufferConsumed()
    decodeThread = Thread {
        decodeLoop()
    }
    decodeThread?.start()
}

// Call this on app shutdown or cleanup
public func stopDecoder() {
    decodingShouldRun = false
    decodeThread?.cancel()
    signalBufferConsumed()
    decodeThread = nil
    decompressionSession = nil
    assetReader = nil
}

@_cdecl("swift_start")
public func swift_start() {
    decodingShouldRun = true
    startDecodeThread()
}

@_cdecl("swift_cleanup_decoder")
public func swift_cleanup_decoder() {
    stopDecoder()
    if let session = decompressionSession {
        VTDecompressionSessionInvalidate(session)
        decompressionSession = nil
    }
    assetReader?.cancelReading()
    assetReader = nil
    sampleBufferOutput = nil
    metalTextures.removeAll()
    metalTextureCache = nil
    setLastError("")
    errorBuffer?.deallocate()
}
