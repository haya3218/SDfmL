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
    Object objs;
    Object title;
    virtual void Create() {
        title.create(0, 0, "data/bg.png");
        AddObject(&title);
        objs.create(50, 50, "data/smile.png");
        AddObject(&objs);
        title.scale.y = 0;
        title.center.x = WINDOW_WIDTH/2;
        title.center.y = WINDOW_HEIGHT/2;
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

        title.y = (480.0/2) - (title.h*title.scale.y/2);

        elaped += 0.01;
    }
};

int main() {
    Init("lmfao");

    MainState m;

    SwitchState(&m);

    playMusic("data/canyon.ogg");

    if (!Update()) {
        return 0;
    }
}