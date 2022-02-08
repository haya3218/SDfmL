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
    Object title2;
    Object title3;
    Object title4;
    SDL_Rect camera = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
    virtual void Create() {
        title.create(0, 0, "data/bg.png");
        AddObject(&title);
        
        title2.create(640, 0, "data/bg.png");
        AddObject(&title2);
        
        title3.create(640, 480, "data/bg.png");
        AddObject(&title3);
        
        title4.create(0, 480, "data/bg.png");
        AddObject(&title4);
        objs.create(0, 0, "data/smile.png");
        AddObject(&objs);
        objs.AddAnimation("idle", {{0, 0, 50, 50}, {50, 0, 50, 50}});
        objs.PlayAnimation("idle");
        objs.framerate = 1;
        objs.setCamera(&camera);
        title.setCamera(&camera);
        title2.setCamera(&camera);
        title3.setCamera(&camera);
        title4.setCamera(&camera);
        title.scale.y = 0;

        objs.centerSelf();

        title.center.x = WINDOW_WIDTH/2;
        title.center.y = WINDOW_HEIGHT/2;

        text.create(50, 480 - 100, "Welcome to the funny application\nMake yourself at home :)\n- haya", "data/monogram.ttf", {255, 255, 255, 255}, TTF_STYLE_NORMAL, 40);
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

        pointTo(&camera, objs);

        title.centerSelf();
        text.centerSelf();

        text.scale.y = cos(elaped*10)*2;
        text.offset.y = -240/2;

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