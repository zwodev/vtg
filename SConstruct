#!/usr/bin/env python
import os
import sys

env = SConscript("submodules/godot-cpp/SConstruct")

# For reference:
# - CCFLAGS are compilation flags shared between C and C++
# - CFLAGS are for C-specific compilation flags
# - CXXFLAGS are for C++-specific compilation flags
# - CPPFLAGS are for pre-processor flags
# - CPPDEFINES are for pre-processor defines
# - LINKFLAGS are for linking flags

env.Append(CPPPATH=["src/", "submodules/SAV1/include", "submodules/SAV1/subprojects/dav1d/include"])
env.Append(LIBPATH=["submodules/SAV1/builddir", "submodules/SAV1/builddir/subprojects/opus/src", "submodules/SAV1/builddir/subprojects/dav1d/src"])
env.Append(LIBS=["sav1", "dav1d", "opus"])

sources = [
    "src/register_types.cpp",
    "src/video_player.cpp",
    "src/video_player_soft.cpp"
]

if env["platform"] == "macos":
    # Disabling compilation of hardware decoding for now.
    # It's just a proof of concept. 

    # env['SWIFT'] = 'swiftc'
    # def build_swift(target, source, env):
    #     swift_command = f"{env['SWIFT']} -c {source[0]} -o {target[0]}"
    #     return os.system(swift_command)

    # # Add the custom builder for Swift
    # swift_builder = Builder(
    #     action=build_swift,
    #     suffix='.o',
    #     src_suffix='.swift'
    # )
    # env.Append(BUILDERS={'SwiftObj': swift_builder})
    
    # swift_header = "src/VideoDecoderMetal.h"
    # swift_source = "src/VideoDecoderMetal.swift"
    # swift_lib_name = "libvideo_decoder_metal.a"
    # swift_obj = env.SwiftObj(target='build/VideoDecoderMetal.o', source=swift_source)
    # video_decoder = env.StaticLibrary(target=os.path.join("build", swift_lib_name), source=[swift_obj])  
    # env.Append(LIBS=[video_decoder])

    library = env.SharedLibrary(
        "demo/bin/libvtg.{}.{}.framework/libvtg.{}.{}".format(
            env["platform"], env["target"], env["platform"], env["target"]
        ),
        source=sources
    )
else:
    library = env.SharedLibrary(
        "demo/bin/libvtg{}{}".format(env["suffix"], env["SHLIBSUFFIX"]),
        source=sources,
    )

Default(library)
