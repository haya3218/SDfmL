#include <fstream>
#include <functional>
#include <iostream>
#include <iterator>
#include <map>
#include <sstream>
#include <stdio.h>
#include <time.h>
#include <vector>
#include <string>
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_ttf.h"
#include "Render.hpp"
#include "SDL2/SDL_syswm.h"
#include "SoLoud/soloud.h"
#include "SoLoud/soloud_wav.h"
#include "SoLoud/soloud_openmpt.h"
#include <iomanip>

#include <ctime>
#include <winuser.h>

#include "toml.hpp"
#include <fstream>

using namespace std;
using namespace Render;

string SOUNDFONT = "data/gm.sf2";
SDL_Window* Render::window;
SDL_Renderer* Render::renderer;
SDL_Event Render::event;
State* Render::current_state = nullptr;
SoLoud::Soloud Render::music;
SoLoud::Soloud Render::se;
SoLoud::WavStream Render::waveLoader;
SoLoud::Openmpt Render::modLoader;
SoLoud::Midi Render::midiLoader;
SoLoud::SoundFont Render::current_sf;
string Render::currentMusic = "";
HWND Render::hwnd;
HWND Render::consoleD;
int Render::seIndex;

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
    SDL_SetTextureBlendMode(_tex, SDL_BLENDMODE_BLEND);
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
    _sc_x = x-cam_rect->x;
    _y = y;
    _sc_y = y-cam_rect->y;
    _w = w*scale.x;
    _sc_w = _w;
    _h = h*scale.y;
    _sc_h = _h;

    if (alpha > 100)
        alpha = 100;

    if (alpha < 0)
        alpha = 0;

    SDL_SetTextureAlphaMod(this->_tex, (this->alpha/100)*255);
}

void Render::Object::setCamera(SDL_Rect* cam_p) {
    cam_rect = cam_p;
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
    FC_SetFilterMode(font, (antialiasing ? FC_FILTER_LINEAR : FC_FILTER_NEAREST));
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
    std::ofstream logFile;
    logFile.open("log.txt", std::ofstream::out | std::ofstream::trunc);
    logFile.close();

    if (!touchFile("conf.toml"))
    {
        tomlExport("conf.toml", 
        {
            {"config", {{"soundfont", "data/gm.sf2"}}}
        });
    }
    SOUNDFONT = tomlParse<string>("conf.toml", "config", "soundfont");
    if (se.init() > 0) {
        log("SoLoud", " has failed to load. Is your dll broken?", ERROR_, __FILENAME__, __LINE__);
        return false;
    }
    if (music.init() > 0) {
        log("SoLoud", " has failed to load. Is your dll broken?", ERROR_, __FILENAME__, __LINE__);
        return false;
    }
    log("SoLoud", " has been successfully initialized.", NORMAL, __FILENAME__, __LINE__);
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        log("SDL", " has failed to load. Is your dll broken? " + string(SDL_GetError()), ERROR_, __FILENAME__, __LINE__);
        return false;
    }
    log("SDL", " has been successfully initialized.", NORMAL, __FILENAME__, __LINE__);
    if (IMG_Init(IMG_INIT_PNG) == 0) {
        log("SDL_image", " has failed to load. Is your dll broken? " + string(SDL_GetError()), ERROR_, __FILENAME__, __LINE__);
        return false;
    }
    log("SDL_image", " has been successfully initialized.", NORMAL, __FILENAME__, __LINE__);
    if (TTF_Init() < 0) {
        log("SDL_ttf", " has failed to load. Is your dll broken? " + string(SDL_GetError()), ERROR_, __FILENAME__, __LINE__);
        return false;
    }
    log("SDL_ttf", " has been successfully initialized.", NORMAL, __FILENAME__, __LINE__);
    window = SDL_CreateWindow(window_name.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL|SDL_WINDOW_SHOWN);
    if (window == nullptr) {
        log("A window", " failed to be created. " + string(SDL_GetError()), ERROR_, __FILENAME__, __LINE__);
        return false;
    }
    log("A window", " has been created.", NORMAL, __FILENAME__, __LINE__);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr) {
        log("A renderer", " failed to be created. " + string(SDL_GetError()), ERROR_, __FILENAME__, __LINE__);
        return false;
    }
    log("A renderer", " has been created.", NORMAL, __FILENAME__, __LINE__);

    current_sf.load(SOUNDFONT.c_str());

    SDL_SysWMinfo wmInfo;
    SDL_VERSION(&wmInfo.version);
    SDL_GetWindowWMInfo(window, &wmInfo);
    hwnd = wmInfo.info.win.window;

    log("", "Finalized initialization. Command over.", NORMAL, __FILENAME__, __LINE__);

    return true;
}

vector<float> Render::_sec;
vector<Func<int>> Render::_call;
vector<bool> Render::_repeats;
vector<int> Render::_ticks;

template <typename T>
void remove_a(std::vector<T>& vec, size_t pos)
{
    typename std::vector<T>::iterator it = vec.begin();
    std::advance(it, pos);
    vec.erase(it);
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

        // I know this is a shitty way to do this
        // but bear with me
        // it was hard to work with lambdas properly man
        // let me have this just one please :)
        if (_sec.size() > 0) {
            for (int i = 0; i < _sec.size(); i++) {
                if (_ticks[i] != -1)
                    _ticks[i]++;

                if (_sec[i] != -1) {
                    if (!(_ticks[i] < Sec2Tick(_sec[i]))) {
                        if (_call[i] != NULL)
                            _call[i](NULL);
                        if (!_repeats[i] && _repeats[i] != NULL)
                        {
                            _ticks[i] = -1;
                            _sec[i] = -1;
                            _call[i] = NULL;
                            _repeats[i] = NULL;
                        }
                    }
                }
                // cout << i << endl;
            }
        }

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
    se.stopAll();
    music.stopAll();
    se.deinit();
    music.deinit();
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();

    return false;
}

void Render::SwitchState(State* state) {
    string state_name = typeid(*state).name();
    log("", "Switching current state to " + state_name, NORMAL, __FILENAME__, __LINE__);
    if (current_state != nullptr) {
        _ticks = {};
        _call = {};
        _repeats = {};
        _sec = {};
        for (int i = 0; i < current_state->get_obj().size(); i++) {
            SDL_DestroyTexture(current_state->get_obj()[i]->_tex);
        }
    }
    current_state = state;
    log("Success!", " Calling Create()....", NORMAL, __FILENAME__, __LINE__);
    current_state->Create();
}

bool Render::playSound(string path, bool override) {
    log("", "Played sound from " + path, NORMAL, __FILENAME__, __LINE__);
    waveLoader.setLooping(false);
    if (override) {
        se.stop(seIndex);
    }
    waveLoader.load(path.c_str());
    seIndex = se.play(waveLoader);
    
    return true;
}

bool Render::playMusic(string path) {
    if (path == "") {
        log("", "Silence." + path, NORMAL, __FILENAME__, __LINE__);
        music.stopAll();
        return true;
    }
    log("", "Played music from " + path, NORMAL, __FILENAME__, __LINE__);
    if (currentMusic == path) {
        music.stopAll();
    }
    waveLoader.load(path.c_str());
    waveLoader.setLooping(true);
    music.play(waveLoader);
    currentMusic = path;

    return true;
}

bool Render::playModPlug(string path) {
    if (path == "") {
        log("", "Silence." + path, NORMAL, __FILENAME__, __LINE__);
        music.stopAll();
        return true;
    }
    if (currentMusic == path) {
        music.stopAll();
    }
    // midis
    if (path.find(".mid") != string::npos) {
        log("", "Played midi from " + path + " with soundfont " + SOUNDFONT, NORMAL, __FILENAME__, __LINE__);
        midiLoader.load(path.c_str(), current_sf);
        midiLoader.setLooping(true);
        music.play(midiLoader);
        currentMusic = path;

        return true;
    }
    log("", "Played mod tracker from " + path, NORMAL, __FILENAME__, __LINE__);
    modLoader.load(path.c_str());
    modLoader.setLooping(true);
    music.play(modLoader);
    currentMusic = path;

    return true;
}

void Render::pointTo(SDL_Rect* camera, Object object) {
    camera->x = ( object.x + (object.w / 2) ) - WINDOW_WIDTH / 2;
    camera->y = ( object.y + (object.h / 2) ) - WINDOW_HEIGHT / 2;

    if( camera->x < 0 )
    { 
        camera->x = 0;
    }
    if( camera->y < 0 )
    {
        camera->y = 0;
    }
    if( camera->x > camera->w )
    {
        camera->x = camera->w;
    }
    if( camera->y > camera->h )
    {
        camera->y = camera->h;
    }
}