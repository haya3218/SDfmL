# What the shit is this

A wrapper build around SDL2, some of its libraries, and SoLoud to make a lot of stuff
more easier to use.

Stuff like:
- Rendering functions
- Object system
- Sound
- Configuration
- and most possibly, more.... idk.
- Less dll binary clutter!

# How the shit to use it den

Currently, it only supports MSVC, but you could probably tweak CMakeLists.txt to be able to use
MingW, or GCC.

## Requirements
- CMake, to build the damn thing
- SDL2 and SDL_ttf 2.0, duh
- SoLoud (static library)
- A VC++ (MSVC) compiler that atleast supports C++ std 17

To use it (in windows atleast),

- Get SDL2 and SDL_ttf 2.0, and put em on the respective x86 and x64 folders.
You know how to place them. It's fucking common sense.
- [SoLoud fork.](https://github.com/haya3218/soloud) Follow GENie instructions and build as a static library.
- You also need to build libmodplug with it as well.
- If you are gonna be using the OpenMPT module, you'll need the libopenmpt dll in your exe directory as well.
- Prebuilt binaries (with OpenMPT) are [here.](https://cdn.discordapp.com/attachments/872338952483381258/942306540159909918/libraries.7z)
- Clone the dam repo somewhere on ur machine.
- Go to where you cloned it, and run `cmake -B build -DCMAKE_BUILD_TYPE=Release`
- After successfully making the build files, run `cmake --build --config Release` to build it without the use of VStudio.
- If everything worked well, congratulations! You have just compiled a shitty program.

`src/Main.cpp` should contain example code with some explanations.