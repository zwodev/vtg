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

# tweak this if you want to use different folders, or more folders, to store your source code in.
env.Append(CPPPATH=["src/", "submodules/SAV1/include", "submodules/SAV1/subprojects/dav1d/include"])
env.Append(LIBPATH=["submodules/SAV1/builddir", "submodules/SAV1/builddir/subprojects/opus/src", "submodules/SAV1/builddir/subprojects/dav1d/src"])
env.Append(LIBS=["sav1", "dav1d", "opus"])
sources = Glob("src/*.cpp")

if env["platform"] == "macos":
    library = env.SharedLibrary(
        "demo/bin/libvtg.{}.{}.framework/libvtg.{}.{}".format(
            env["platform"], env["target"], env["platform"], env["target"]
        ),
        source=sources,
    )
else:
    library = env.SharedLibrary(
        "demo/bin/libvtg{}{}".format(env["suffix"], env["SHLIBSUFFIX"]),
        source=sources,
    )

Default(library)
