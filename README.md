# Video Toolkit for Godot

## Building SAV1
```
cd ./src/submodules/SAV1/subprojects
chmod +x download.sh
./download.sh

meson setup buildir
meson compile -C

```

## Building godot-cpp
```
cd ./src/submodules/godot-cpp
scons platform=macos arch=arm64 target=template_release
scons platform=macos arch=arm64 target=template_debug

```

## Building VTG
```
scons platform=macos arch=arm64 target=template_release
scons platform=macos arch=arm64 target=template_debug

```
