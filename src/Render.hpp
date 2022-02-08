#ifndef _RENDER_H
#define _RENDER_H
#include <array>
#include <functional>
#include <iostream>
#include <map>
#include <stdint.h>
#include <vector>
#include <string>
#include <windows.h>
#include "SDL2/SDL.h"
#include "SDL2/SDL_ttf.h"
#include "BASS/audio_out.h"
#include "SDL2/SDL_FontCache.h"

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

struct Vector2i
{
  int x = 1;
  int y = 1;
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

            /*
            * Create a new Object instance.
            */
            virtual void create(int x = 0, int y = 0, string path = "");

            virtual void Draw(float dt);

            int x, y, w, h;

            Vector2i offset = {0, 0};

            Vector2 scale;

            float angle = 0.00001;

            float alpha = 100;

            SDL_Point center = {0, 0};

            map<string, bool> get_properties() const;
            int _sc_x, _sc_y, _sc_w, _sc_h;
            SDL_Texture* _tex = nullptr;
            void set_property(string name, bool value);

            SDL_Rect get_src() {return src_rect;};

            SDL_Rect src_rect = {0, 0, 0, 0};

            /*
            * Center object on the center of the screen on a certain axis. Defaults to both X and Y.
            */
            void centerSelf(AXIS axis = XY);

            void setCamera(SDL_Rect* cam_p);
        private:
            int _x, _y, _w, _h;
            int _ori_w, _ori_h;
            SDL_Rect du = {0, 0, 0, 0};
            SDL_Rect* cam_rect = &du;
            map<string, bool> _properties;
    };

    class AnimatedObject : public Object {
        public:
            /*
            * Create a new AnimatedObject instance.
            */
            virtual void create(int x = 0, int y = 0, string path = "");
            /*
            * Add an animation to said object. Uses SDL_Rects for frames.
            */
            void AddAnimation(string anim_name, vector<SDL_Rect> points);
            /*
            * Play an animation.
            */
            void PlayAnimation(string anim_name);
            virtual void Draw(float dt);
            
            int framerate = 24;
            map<string, vector<SDL_Rect>> frameRects;
            string current_framename = "";
            int current_frame = 0;

        private:
            int startTime;
    };

    class TextObject : public Object {
        public:
            /*
            * Create a new TextObject instance.
            */
            virtual void create(int x = 0, int y = 0, string text = "", string font_name = "data/monogram.ttf", SDL_Color color = {255, 255, 255, 255}, int style = TTF_STYLE_NORMAL, int size = 20);

            virtual void Draw(float dt);

            FC_Font* font;
            FC_AlignEnum alignment = FC_ALIGN_LEFT;
            SDL_Color color;
            string text = "";
            bool antialiasing = false;
        private:
            int font_size = 20;
    };

    template <typename T>
    using Func = std::function<T(T)>;

    extern vector<float> _sec;
    extern vector<Func<int>> _call;
    extern vector<bool> _repeats;
    extern vector<int> _ticks;

    class Timer {
        public:
            void start(float seconds, Func<int> callback, bool repeat = false) {
                _sec.push_back(seconds);
                _call.push_back(callback);
                _repeats.push_back(repeat);
                _ticks.push_back(0);
            }
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
                        if (obj[i]->_tex != nullptr) {
                            SDL_Rect r = {obj[i]->_sc_x+obj[i]->offset.x, obj[i]->_sc_y+obj[i]->offset.y, obj[i]->_sc_w, obj[i]->_sc_h};
                            SDL_Rect r2 = obj[i]->src_rect;
                            if (r2.w != 0 && r2.h != 0)
                                SDL_RenderCopyEx(renderer, obj[i]->_tex, &r2, &r, obj[i]->angle, &obj[i]->center, SDL_FLIP_NONE);
                            else
                                SDL_RenderCopyEx(renderer, obj[i]->_tex, NULL, &r, obj[i]->angle, &obj[i]->center, SDL_FLIP_NONE);
                        }
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

    extern HWND hwnd;
    extern HWND consoleD;

    extern State* current_state;

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

    /*
    * Make the camera center itself on an object.
    */
    void pointTo(SDL_Rect* camera, Object object);

    inline int Sec2Tick(float time) {
        return FRAMERATE*time;
    }
}
#endif