#ifndef _RENDER_H
#define _RENDER_H
#include <array>
#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <windows.h>
#include "SDL2/SDL.h"
#include "BASS/audio_out.h"

using namespace std;

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480

#define FRAMERATE 60

#define MAX_SE 10

struct Vector2
{
  float x = 1;
  float y = 1;
};

enum AXIS {
    X,
    Y,
    XY
};

namespace Render {
    extern SDL_Window* window;
    extern SDL_Renderer* renderer;

    /*
    * The object class is a class that contains functionality to make it do... stuff.
    */
    class Object {
        public:
            Object();
            ~Object();

            int id = NULL;

            void create(int x = 0, int y = 0, string path = "");

            virtual void Draw(float dt);

            int x, y, w, h;

            Vector2 scale;

            float angle = 0.00001;

            SDL_Point center = {0, 0};

            map<string, bool> get_properties() const;
            int _sc_x, _sc_y, _sc_w, _sc_h;
            SDL_Texture* _tex = nullptr;
            void set_property(string name, bool value);

            SDL_Rect get_src() {return src_rect;};

            SDL_Rect src_rect = {0, 0, 0, 0};

            void centerSelf(AXIS axis = XY);
        private:
            int _x, _y, _w, _h;
            int _ori_w, _ori_h;
            SDL_Rect cam_rect = {0, 0, 0, 0};
            map<string, bool> _properties;
    };

    class AnimatedObject : public Object {
        public:
            void create(int x = 0, int y = 0, string path = "");
            void AddAnimation(string anim_name, vector<SDL_Rect> points);
            void PlayAnimation(string anim_name);
            virtual void Draw(float dt);
            
            int framerate = 24;
            map<string, vector<SDL_Rect>> frameRects;
            string current_framename = "";
            int current_frame = 0;

        private:
            int startTime;
    };

    /*
    * A state is where you would contain said objects.
    */
    class State {
        public:
            State();
            ~State();
            /*
            * State entrypoint.
            */
            virtual void Create() {
                
            }
            /*
            * State update point.
            */
            virtual void Update(float dt) {}
            /*
            * State draw point. Make sure to call State::Draw() first when overriding this!!!
            */
            virtual void Draw(float dt){
                SDL_RenderClear(renderer);
                if (obj.size() > 0)
                    for (int i = 0; i < obj.size(); i++) {
                        obj[i]->Draw(dt);
                        SDL_Rect r = {obj[i]->_sc_x, obj[i]->_sc_y, obj[i]->_sc_w, obj[i]->_sc_h};
                        SDL_Rect r2 = obj[i]->src_rect;
                        if (r2.w != 0 && r2.h != 0)
                            SDL_RenderCopyEx(renderer, obj[i]->_tex, &r2, &r, obj[i]->angle, &obj[i]->center, SDL_FLIP_NONE);
                        else
                            SDL_RenderCopyEx(renderer, obj[i]->_tex, NULL, &r, obj[i]->angle, &obj[i]->center, SDL_FLIP_NONE);
                    }
                SDL_RenderPresent(renderer);
            }
            /*
            * Use this to add objects. Make sure to pass the address, not the object itself.
            */
            virtual void AddObject(Object* object);
            vector<Object*> get_obj();
        private:
            vector<Object*> obj;
    };

    /*
    * Init EVERYTHING. Also makes sure everything works.
    */
    bool Init(string window_name);
    /*
    * Internal main update loop. Only call once.
    */
    bool Update();
    /*
    * Switch our current state. Pass the address of the state, not the state itself.
    */
    void SwitchState(State* state);

    extern SDL_Event event;
    extern State* current_state;

    extern HWND hwnd;
    extern HWND consoleD;

    extern array<anshub::AudioOut, MAX_SE> audioArray;
    extern anshub::AudioOut music;
    extern string currentMusic;
    /*
    * Play a sound. If a sound is currently playing then it will proceed to a different sound id.
    * You can also specify the specific id, if you intentionally want the sound to override another.
    */
    bool playSound(string path, int id = NULL); 
    /*
    * Play music. Always loops.
    * Passing a blank string (e.g. "") will stop the current playing music.
    */
    bool playMusic(string path); 
}
#endif