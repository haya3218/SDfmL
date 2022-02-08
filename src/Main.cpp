#define SDL_MAIN_HANDLED // needs to be here or linker fucking dies
#undef _HAS_STD_BYTE
#include <iostream>
#include "SDL2/SDL.h"
#include "Render.hpp"
#include <cmath>
#include <Windows.h>

using namespace std;
using namespace Render;

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
        playMusic("data/canyon.ogg");
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
    int tick[2];
    virtual void Create() {
        bg.create(0, 0, "data/powered.png");
        AddObject(&bg);

        black.create(0, 0, "data/black.png");
        AddObject(&black);

        black.alpha = 0;

        playSound("data/flixel.ogg");

        // basic timer
        tick[0] = Sec2Tick(1.0);
        tick[1] = Sec2Tick(3.0);

        //SDL_SetRenderDrawColor( renderer, 0xFF, 0xFF, 0xFF, 0xFF );
    }
    int now[2] = {0, 0};
    virtual void Update(float dt) {
        for (int i = 0; i < 2; i++) {
            now[i]++;
        }
        if (!(now[0] < tick[0])) {
            black.alpha += 1;
        }
        if (!(now[1] < tick[1])) {
            SwitchState(&m2);
        }
    }
};

SplashScreen m;

int main() {
    Init("SDLflixel :)))))");

    SwitchState(&m);

    if (!Update()) {
        return 0;
    }
}