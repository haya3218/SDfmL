# 🤨

A wrapper build around SDL2, some of its libraries, and SoLoud to make a lot of stuff
more easier to use.

Stuff like:

- Rendering functions
- Object system
- Sound
- Configuration
- Less dll binary clutter!
- and most possibly, more.... idk.

## Requirements

- [CMake](https://cmake.org) version 3.10 or above
- [SDL2 and SDL_ttf 2.0](https://libsdl.org)
- [SDL_gpu](https://github.com/grimfang4/sdl-gpu)
- [SoLoud](https://github.com/haya3218/soloud) (static library)
- [LuaJIT](http://luajit.org)

### Compiler Requirements

- VC++ (MSVC) version 14.2 or above
(GCC, clang, and MingW-w64 are not supported yet, [but feel free to make a pull request!](https://github.com/haya3218/SDfmL/compare))

#### Instructions

Copy all of the library files to the `lib/` folder.
Reminder that ALL header files are already included, so you don't need to worry about linker issues.

`src/Main.cpp` should contain example code with some explanations.
