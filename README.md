# Video Toolkit for Godot

## 

## Build Instruction
### Building godot-cpp
```
cd ./src/submodules/godot-cpp
scons platform=macos arch=arm64 target=template_release
scons platform=macos arch=arm64 target=template_debug

```

### Building SAV1
```
cd ./src/submodules/SAV1/subprojects
chmod +x download.sh
./download.sh

cd ..

meson setup buildir
meson compile -C ./builddir

```

### Building VTG
```
scons platform=macos arch=arm64 target=template_release
scons platform=macos arch=arm64 target=template_debug

```
