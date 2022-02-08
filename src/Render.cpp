#include <iostream>
#include <map>
#include <stdio.h>
#include <vector>
#include <string>
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_ttf.h"
#include "Render.hpp"
#include "SDL2/SDL_syswm.h"

using namespace std;
using namespace Render;

SDL_Window* Render::window;
SDL_Renderer* Render::renderer;
SDL_Event Render::event;
State* Render::current_state;
array<anshub::AudioOut, MAX_SE> Render::audioArray;
anshub::AudioOut Render::music;
string Render::currentMusic = "";
HWND Render::hwnd;
HWND Render::consoleD;

Render::Object::Object() {
    
}

Render::Object::~Object() {
    // placeholder
}

Render::State::State() {
    // placeholder
}

Render::State::~State() {
    // placeholder
}

void Render::Object::create(int x, int y, string path){
    this->_tex = IMG_LoadTexture(renderer, path.c_str());
    if (_tex == nullptr) {
        cout << "texture failed to lod" << endl;
    }
    _x = x;
    _y = y;
    _sc_x = x;
    _sc_y = y;
    int w_, h_;
    SDL_QueryTexture(this->_tex, NULL, NULL, &w_, &h_);
    _sc_w = w_;
    _sc_h = h_;
    this->x = x;
    this->y = y;
    w = w_;
    h = h_;
    _ori_w = w_;
    _ori_h = h_;
}

void Render::AnimatedObject::create(int x, int y, string path){
    Render::Object::create(x,y,path);
    startTime = SDL_GetTicks();
}

void Render::State::AddObject(Render::Object* object) {
    object->id = obj.size();
    this->obj.push_back(object);
}

vector<Object*> Render::State::get_obj() {
    return this->obj;
}

void Render::Object::Draw(float dt) {
    _x = x;
    _sc_x = x-cam_rect.x;
    _y = y;
    _sc_y = y-cam_rect.y;
    _w = w*scale.x;
    _sc_w = _w-cam_rect.w;
    _h = h*scale.y;
    _sc_h = _h-cam_rect.h;
}

void Render::AnimatedObject::Draw(float dt) {
    Render::Object::Draw(dt);

    if (current_framename != "") {
        // this will make it so that current_frame will only advance when it needs to
        int frameToDraw = ((SDL_GetTicks() - startTime) * framerate / 1000) % frameRects[current_framename].size();
        current_frame = frameToDraw;

        int sx = frameRects[current_framename][current_frame].x;
        int sy = frameRects[current_framename][current_frame].y;
        int sw = frameRects[current_framename][current_frame].w;
        int sh = frameRects[current_framename][current_frame].h;

        // support scaling :)
        _sc_w = frameRects[current_framename][current_frame].w*scale.x;
        _sc_h = frameRects[current_framename][current_frame].h*scale.y;

        // after setting shit up, we then store it in src_rect.
        src_rect = {sx, sy, sw, sh};
    }
}

void Render::AnimatedObject::AddAnimation(string anim_name, vector<SDL_Rect> points) {
    frameRects.insert({anim_name, points});
}

void Render::AnimatedObject::PlayAnimation(string anim_name) {
    current_framename = anim_name;
}

void Render::TextObject::create(int x, int y, string text, string font_name, SDL_Color color, int style, int size) {
    Render::Object::create(x,y,"data/smile.png"); // dummy
    this->text = text;
    _tex = nullptr;
    _sc_x = x;
    _sc_y = y;
    _sc_w = strlen(text.c_str())*size;
    _sc_h = strlen(text.c_str())*size;
    this->x = x;
    this->y = y;
    w = strlen(text.c_str())*size;
    h = strlen(text.c_str())*size;

    font_size = size;
    this->color = color;

    font = FC_CreateFont();
    FC_LoadFont(font, renderer, font_name.c_str(), size, color, style);
}

void Render::TextObject::Draw(float dt) {
    Render::Object::Draw(dt);

    FC_Effect eff;
    eff.alignment = alignment;
    eff.color = color;
    eff.scale.x = scale.x;
    eff.scale.y = scale.y;
    FC_DrawEffect(font, renderer, x-offset.x, y-offset.y, eff, text.c_str());

    SDL_Rect rec = FC_GetBounds(font, x-offset.x, y-offset.y, alignment, eff.scale, text.c_str());

    w = rec.w;
    h = rec.h;
}

void Render::Object::centerSelf(AXIS axis) {
    switch (axis) {
        case X:
            x = (WINDOW_WIDTH/2) - (w*scale.x/2);
            break;
        case Y:
            y = (WINDOW_HEIGHT/2) - (h*scale.y/2);
            break;
        case XY:
            x = (WINDOW_WIDTH/2) - (w*scale.x/2);
            y = (WINDOW_HEIGHT/2) - (h*scale.y/2);
            break;
    }
}

bool Render::Init(string window_name) {
    consoleD = GetConsoleWindow();
    SetWindowTextA(consoleD, "Logging window");
    if (!BASS_Init(1, 44100, BASS_DEVICE_8BITS|BASS_DEVICE_REINIT, 0, NULL)) {
        cout << "BASS has failed to initialize. Is your dll broken? <NO ERROR>" << endl;
        return false;
    }
    cout << "Successfully initialized the BASS audio system. Command next." << endl;
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        cout << "SDL has failed to initialize. Is your dll broken? " << SDL_GetError() << endl;
        return false;
    }
    cout << "Successfully initialized SDL. Command next." << endl;
    if (IMG_Init(IMG_INIT_PNG) == 0) {
        cout << "SDL_image has failed to initialize. Is your dll broken? " << SDL_GetError() << endl;
        return false;
    }
    cout << "Successfully initialized SDL_image. Command next." << endl;
    if (TTF_Init() < 0) {
        cout << "SDL_ttf has failed to initialize. Is your dll broken? " << SDL_GetError() << endl;
        return false;
    }
    cout << "Successfully initialized SDL_ttf. Command next." << endl;
    window = SDL_CreateWindow(window_name.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL|SDL_WINDOW_SHOWN);
    if (window == nullptr) {
        cout << "Window has failed to initialize. "<< SDL_GetError() << endl;
        return false;
    }
    cout << "Successfully made a window. Command next." << endl;
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr) {
        cout << "Renderer has failed to initialize. "<< SDL_GetError() << endl;
        return false;
    }
    cout << "Successfully made a renderer. Command next." << endl;

    SDL_SysWMinfo wmInfo;
    SDL_VERSION(&wmInfo.version);
    SDL_GetWindowWMInfo(window, &wmInfo);
    hwnd = wmInfo.info.win.window;

    cout << "Finalized initialization. Command over." << endl;

    return true;
}

bool Render::Update() {
    int lastUpdate = SDL_GetTicks();
    bool run = true;
    while (run) {
        while(SDL_PollEvent(&event)) {
            if(event.type == SDL_QUIT) {
                run = false;
                break;
            }
        }

        SetWindowTextA(consoleD, "Logging window");

        int start = SDL_GetPerformanceCounter();

        int current = SDL_GetTicks();
        float dT = (current - lastUpdate) / 1000.0f;

        current_state->Update(dT);

        lastUpdate = current;
        
        current_state->Draw(dT);

        int end = SDL_GetPerformanceCounter();

	    float elapsedMS = (end - start) / SDL_GetPerformanceFrequency() * 1000.0f;

        SDL_Delay(floor((1000.0f/FRAMERATE) - elapsedMS));
    }

    for (int i = 0; i < current_state->get_obj().size(); i++) {
        SDL_DestroyTexture(current_state->get_obj()[i]->_tex);
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();

    return false;
}

void Render::SwitchState(State* state) {
    current_state = state;
    current_state->Create();
}

bool Render::playSound(string path, int id) {
    if (id == NULL) {
        for (int i = 0; i < MAX_SE; i++) {
            if (audioArray[i].NowPlaying(false).size() == 0) {
                audioArray[i].Play(path);
                cout << "Played dat boi. audio id no: " << i << endl;
                break;
            }
        }
    } else {
        audioArray[id].Play(path);
    }
    
    return true;
}

bool Render::playMusic(string path) {
    if (path == "") {
        music.Stop(currentMusic);
    }
    if (currentMusic != "") {
        music.Stop(currentMusic);
    }
    music.Play(path, true);
    currentMusic = path;

    return true;
}