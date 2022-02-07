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
    Object title;
    virtual void Create() {
        title.create(0, 0, "data/bg.png");
        AddObject(&title);
        objs.create(50, 50, "data/smile.png");
        AddObject(&objs);
        objs.AddAnimation("idle", {{0, 0, 50, 50}, {50, 0, 50, 50}});
        objs.PlayAnimation("idle");
        objs.framerate = 1;
        title.scale.y = 0;

        title.center.x = WINDOW_WIDTH/2;
        title.center.y = WINDOW_HEIGHT/2;

        text.create(50, 480 - 100, "funny");
        text.scale.x = 2;
        text.scale.y = 2;
        AddObject(&text);
    }
    int yvel = 0;
    int xvel = 0;
    float elaped = 0;
    virtual void Update(float dt) {
        const Uint8* kb = SDL_GetKeyboardState(NULL);

        yvel = -kb[SDL_SCANCODE_UP] + kb[SDL_SCANCODE_DOWN];
        xvel = -kb[SDL_SCANCODE_LEFT] + kb[SDL_SCANCODE_RIGHT];
        objs.x = objs.x + xvel*2;
        objs.y = objs.y + yvel*2;

        title.scale.y = sin(elaped);
        title.scale.x = cos(elaped);

        title.centerSelf();

        elaped += 0.01;
    }
    
};

int main() {
    Init("SDLflixel :)))))");

    MainState m;

    SwitchState(&m);

    playMusic("data/canyon.ogg");

    if (!Update()) {
        return 0;
    }
}