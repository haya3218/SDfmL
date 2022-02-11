![bitch](logo.png)

# SDfmL

*Simple DirectMedia Flixel-like Multipurpose Layer*

*A basic wrapper meant to make creating SHIT easier*

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
- [SoLoud.](https://sol.gfxile.net/soloud/index.html) Follow GENie instructions and build as a static library.
Since it's currently confusing to do, [here's some prebuilt lib files for use with this wrapper.](https://cdn.discordapp.com/attachments/872338952483381258/940791426797686804/SoLoud_Static.zip)
- Clone the dam repo somewhere on ur machine.
- Go to where you cloned it, and run `cmake -B build -DCMAKE_BUILD_TYPE=Release`
- After successfully making the build files, run `cmake --build --config Release` to build it without the use of VStudio.
- If everything worked well, congratulations! You have just compiled a shitty program.

`src/Main.cpp` should contain example code with some explanations.

# Licensing

The sole libraries themselves (SDL, SoLoud) are under the zLib/LibPNG licenses ofc, so you'll have to deal with that

stb_image is under the Public Domain/MIT.

TinySoundfont and toml11 under the same license as this wrapper (MIT License)

## SDfmL license
MIT License

Copyright 2022 haya3218

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.