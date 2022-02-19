/**
 * @file Main.cpp
 * @author haya3218
 * @brief This is an example on how to use SDfmL. Code may suck. beware
 * @version 0.1
 * @date 2022-02-11
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#define SDL_STBIMAGE_IMPLEMENTATION // to use sdl_stbimage
#undef _HAS_STD_BYTE // avoid std::byte hijacking native byte type
#include <iostream>
#include "SDL2/SDL.h"
#undef main
#include <cmath>
#include <Windows.h>
#include <stdlib.h>
#include "argh.h"
#include "guicon.h"
#include <random>

#include "libflixel.hpp"

class ExampleState : public sdfml::sdState {
    public:
        sdfml::sdAnimatedSprite example;
        sdfml::sdSprite bg1;
        SDL_Rect camera = {0, 0, 640, 480};
        // vector<sdfml::sdSprite> bgs;
        virtual void create() {
            example.create(50, 50, "data/images/smile.png");
            example.AddAnimation("idle", {{0, 0, 50, 50}, {50, 0, 50, 50}});
            example.PlayAnimation("idle");
            example.framerate = 1;
            sdfml::sound.music.playMusic("data/music/canyon.ogg");
            example.updateCamera(&camera);
            bg1.create(0, 0, "data/images/bg.png");
            add(&bg1);
            bg1.updateCamera(&camera);
            add(&example);
            //bg1.screenCenter();
        }
        virtual void update(float elapsed) {
            if (sdfml::key_pressed(SDL_SCANCODE_LEFT))
                example.x -= 1;
            if (sdfml::key_pressed(SDL_SCANCODE_RIGHT))
                example.x += 1;
            if (sdfml::key_pressed(SDL_SCANCODE_UP))
                example.y -= 1;
            if (sdfml::key_pressed(SDL_SCANCODE_DOWN))
                example.y += 1;
            if (sdfml::key_just_pressed(SDL_SCANCODE_R))
                sdfml::switchState(this);

            bg1.scale.x = sin(sdfml::elapsed);
            bg1.scale.y = cos(sdfml::elapsed);

            bg1.screenCenter();

            sdfml::focusCamera(&camera, example);
        }
};

ExampleState m;

int main(int argc, char* argv[]) 
{
    if (sdfml::init() > 0) {
        return EXIT_FAILURE;
    }

    argh::parser pa(argc, argv);
    // Verbose redirects io output to console, if available
    if (pa[{"-v", "--verbose"}]) {
        RedirectIOToConsole();
        sdfml::llog("Verbose mode", " enabled.", NORMAL, __FILENAME__, __LINE__);
    }

    sdfml::switchState(&m);

    if (sdfml::update() > 0) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

// This is the main entry point of a program. Currently all this does is parse arguments and call main().
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow)
{
    int    argc;
    char** argv;

    char*  arg;
    int    index;
    int    result;

    // count the arguments
    
    argc = 1;
    arg  = cmdline;
    
    while (arg[0] != 0) {

        while (arg[0] != 0 && arg[0] == ' ') {
            arg++;
        }

        if (arg[0] != 0) {
        
            argc++;
        
            while (arg[0] != 0 && arg[0] != ' ') {
                arg++;
            }
        
        }
    
    }    
    
    // tokenize the arguments

    argv = (char**)malloc(argc * sizeof(char*));

    arg = cmdline;
    index = 1;

    while (arg[0] != 0) {

        while (arg[0] != 0 && arg[0] == ' ') {
            arg++;
        }

        if (arg[0] != 0) {
        
            argv[index] = arg;
            index++;
        
            while (arg[0] != 0 && arg[0] != ' ') {
                arg++;
            }
        
            if (arg[0] != 0) {
                arg[0] = 0;    
                arg++;
            }
        
        }
    
    }    

    // put the program name into argv[0]

    char filename[_MAX_PATH];
    
    GetModuleFileName(NULL, filename, _MAX_PATH);
    argv[0] = filename;

    // call the user specified main function   
    
    result = main(argc, argv);
    
    free(argv);
    return result;
}