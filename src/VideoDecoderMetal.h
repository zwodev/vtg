#ifndef VIDEO_DECODER_METAL
#define VIDEO_DECODER_METAL

#include <stdlib.h>  // For strdup
#include <stdint.h>  // For int32_t, etc.

extern "C" {
    // Function declarations exposed for C/C++
    const char* swift_get_last_error();
    void swift_initialize_decoder(
        const char* videoPath,
        void** metalTexturePtrs,
        int32_t textureCount,
        int32_t width,
        int32_t height
    );
    void swift_get_video_size(int32_t* widthOut, int32_t* heightOut);
    int32_t swift_request_decode(int64_t currentTimeNs, int32_t* frameReadyOut);
    void swift_start();
    void swift_cleanup_decoder();
}

#endif /* VIDEO_DECODER_METAL */