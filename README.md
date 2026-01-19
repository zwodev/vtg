# Video Toolkit for Godot

## Clone Repository
```
git clone --recurse-submodules https://github.com/zwodev/vtg.git

```

## Building on Linux
### Installing dependencies
On Ubuntu:

```
sudo apt-get update
sudo apt-get install -y \
  build-essential \
  scons \
  pkg-config \
  libx11-dev \
  libxcursor-dev \
  libxinerama-dev \
  libgl1-mesa-dev \
  libglu1-mesa-dev \
  libasound2-dev \
  libpulse-dev \
  libudev-dev \
  libxi-dev \
  libxrandr-dev \
  libwayland-dev
```

Other Linux distros:
https://docs.godotengine.org/en/latest/engine_details/development/compiling/compiling_for_linuxbsd.html

### Building godot-cpp
```
cd ./src/submodules/godot-cpp
scons platform=linux arch=x86_64 target=template_release
scons platform=linux arch=x86_64 target=template_debug

```

### Building SAV1
```
cd ./src/submodules/SAV1/subprojects
chmod +x download.sh
./download.sh

cd ..

meson setup builddir
meson compile -C ./builddir

```

### Building VTG
```
scons platform=linux arch=x86_64 target=template_release
scons platform=linux arch=x86_64 target=template_debug

```

## Building on macOS

### Installing dependencies
Using Homebrew:
```
brew install scons
```

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

meson setup builddir
meson compile -C ./builddir

```

### Building VTG
```
scons platform=macos arch=arm64 target=template_release
scons platform=macos arch=arm64 target=template_debug

```
