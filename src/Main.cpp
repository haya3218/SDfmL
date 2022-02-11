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
#include "Render.hpp"
#include <cmath>
#include <Windows.h>
#include <stdlib.h>
#include "argh.h"
#include "guicon.h"
#include <random>

using namespace std;
using namespace Render;
using namespace argh;

bool randomBool() {
   return rand() % 2;
}

class MainState : public State {
    AnimatedObject objs;
    TextObject text;
    // objects need to be outside the function to work
    Object title;
    Object title2;
    Object title3;
    Object title4;
    // cameras are rects
    SDL_Rect camera = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
    // to override a function, just state back the name of it with a virtual keyword
    // before it
    virtual void Create() {
        // SPEAK("eyeyeyayeyayeyaeyaeyayeayeyaeyaeyayeyaeyayeyayeyayeyayeaye", 3000.0f, 5.0f);
        srand(time(0));
        if (randomBool())
            playModPlug("data/canyon.mid");
        else
        {
            playModPlug("data/solarbeams.xm");
        }
        // i know that this isnt the best idea to do this
        // yeah, im just lazy
        title.create(0, 0, "data/bg.png");
        AddObject(&title);       
        title2.create(640, 0, "data/bg.png");
        AddObject(&title2);      
        title3.create(640, 480, "data/bg.png");
        AddObject(&title3);
        title4.create(0, 480, "data/bg.png");
        AddObject(&title4);
        // example of animated object
        objs.create(0, 0, "data/smile.png");
        AddObject(&objs);
        objs.AddAnimation("idle", {{0, 0, 50, 50}, {50, 0, 50, 50}});
        objs.PlayAnimation("idle");
        objs.framerate = 1;
        // set cameras :)
        objs.setCamera(&camera);
        title.setCamera(&camera);
        title2.setCamera(&camera);
        title3.setCamera(&camera);
        title4.setCamera(&camera);
        title.scale.y = 0;

        // center object
        objs.centerSelf();

        // offsets :)
        title2.offset.x = 640;
        title3.offset = {640, 480};
        title4.offset.y = 480;

        // example of a text object
        text.create(50, 480 - 100, "Welcome to the funny application\nMake yourself at home :)\n- haya", "data/monogram.ttf", {255, 255, 255, 255}, TTF_STYLE_NORMAL, 40);
        AddObject(&text);
    }
    int yvel = 0;
    int xvel = 0;
    float elaped = 0;
    // update
    virtual void Update(float dt) {
        // to get current keyboard input, use SDL_GetKeyboardState
        const Uint8* kb = SDL_GetKeyboardState(NULL);

        // :)
        yvel = -kb[SDL_SCANCODE_UP] + kb[SDL_SCANCODE_DOWN];
        xvel = -kb[SDL_SCANCODE_LEFT] + kb[SDL_SCANCODE_RIGHT];
        objs.x = objs.x + xvel*2;
        objs.y = objs.y + yvel*2;

        // cool scaling
        title.scale.y = sin(elaped/100);
        title.scale.x = cos(elaped/100);
        title2.scale.y = cos(elaped/100);
        title2.scale.x = sin(elaped/100);
        title3.scale.y = sin(elaped/100);
        title3.scale.x = sin(elaped/100);
        title4.scale.y = cos(elaped/100);
        title4.scale.x = cos(elaped/100);

        // make camera point to an object
        pointTo(&camera, objs);

        // center shit
        title.centerSelf();
        title2.centerSelf();
        title3.centerSelf();
        title4.centerSelf();
        text.centerSelf();

        // :)
        text.scale.y = cos(elaped/10)*2;
        text.offset.y = -240/2;

        // for the sin and cos shit
        elaped += 1;
    }
};

MainState m2;

class SplashScreen : public State {
    Object bg;
    Object black;
    virtual void Create() {
        bg.create(0, 0, "data/powered.png");
        AddObject(&bg);

        black.create(0, 0, "data/black.png");
        AddObject(&black);

        black.alpha = 0;

        Timer time;
        Timer time2;
        time2.start(1.0, [this](int dummy) {black.alpha += 1; return 0;}, true);
        time.start(3.0, [](int dummy) {SwitchState(&m2); return 0;});

        playSound("data/flixel.ogg");

        //SDL_SetRenderDrawColor( renderer, 0xFF, 0xFF, 0xFF, 0xFF );
    }
};

SplashScreen m;

int main(int argc, char* argv[]) 
{
    if (!Init("SDfmL :))))))")){
        return EXIT_FAILURE;
    }

    parser pa(argc, argv);
    // Verbose redirects io output to console, if available
    if (pa[{"-v", "--verbose"}]) {
        RedirectIOToConsole();
        log("", "Verbose mode enabled.", NORMAL, __FILENAME__, __LINE__);
    }

    SwitchState(&m);

    if (!Update()) {
        return EXIT_SUCCESS;
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

    log("", to_string(argc));
    
    result = main(argc, argv);
    
    free(argv);
    return result;
}