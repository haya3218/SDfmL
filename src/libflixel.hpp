#ifndef LIB_FLIXEL_IMPLEMENTATION
#define LIB_FLIXEL_IMPLEMENTATION
#include <array>
#include <functional>
#include <iostream>
#include <map>
#include <stdint.h>
#include <system_error>
#include <unordered_map>
#include <utility>
#include <vector>
#include <string>
#include <windows.h>
#include "SDL2/SDL.h"
#include "SDL2/SDL_ttf.h"
#include "SDL2/SDL_FontCache.h"
#include "SoLoud/soloud.h"
#include "SoLoud/soloud_wav.h"
#include "SoLoud/soloud_wavstream.h"
#include "SoLoud/soloud_modplug.h"
#include "SoLoud/soloud_openmpt.h"
#include "SoLoud/MIDI/soloud_midi.h"
#include "SoLoud/soloud_speech.h"
#include "SoLoud/soloud_vizsn.h"
#include "SoLoud/soloud_waveshaperfilter.h"
#include "SoLoud/soloud_biquadresonantfilter.h"
#include "SoLoud/soloud_fftfilter.h"
#include "SDL2/SDL_syswm.h"

#include "SDL2/SDL_stbimage.h"

#include "toml.hpp"
#include <fstream>

#include "guicon.h"

#include "sdfml/music.hpp"
#include "sdfml/shader.hpp"

#include "SDL_gpu/SDL_gpu.h"

#include <thread>         // std::this_thread::sleep_for
#include <chrono>         // std::chrono::seconds

#ifdef _WIN32
#define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#else
#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#endif

using namespace std;

#define DEFAULT_WINDOW_WIDTH 640
#define DEFAULT_WINDOW_HEIGHT 480

#define FRAMERATE 60

#define DEFAULT_SF "data/gm.sf2"

struct Vector2f
{
    float x = 0;
    float y = 0;
};

struct Vector2
{
    int x = 0;
    int y = 0;
};

template <typename T>
struct Vector3 {
    T r;
    T g;
    T b;
};

enum lLOG_TYPE {
    NORMAL,
    WARNING,
    ERROR_
};

enum AXIS {
    X,
    Y,
    XY
};

typedef HWND mWin;

inline std::string _GetCurrentDirectory()
{
	char buffer[MAX_PATH];
	GetModuleFileNameA(NULL, buffer, MAX_PATH);
	std::string::size_type pos = std::string(buffer).find_last_of("\\/");
	
	return std::string(buffer).substr(0, pos);
}

namespace sdfml {

    static double elapsed;

    inline int llog(string prefix, string msg, lLOG_TYPE type = NORMAL, string file = "???.cpp", int line = 0) {
        clock_t now = std::clock();

        double now_n = (double)now / CLOCKS_PER_SEC;

        string typeName = "LOG";

        switch (type) {
            case NORMAL:
                break;
            case WARNING:
                typeName = "WARNING";
                break;
            case ERROR_:
                typeName = "ERROR";
                break;
        }

        std::stringstream buf;

        buf << (int)(now_n/60) << ":"
            << std::setfill('0') << std::setw(2) << (int)((int)now_n % 60) << "."
            << std::setfill('0') << std::setw(3) << (int)((now_n - (int)now_n) * 1000) << " "
            << typeName << ": (" << file << ":" << line << ") " << prefix << msg << endl;

        std::ofstream logFile;
        logFile.open("log.txt", std::ios::app);
        logFile << buf.str();
        logFile.close();

        if (type != ERROR_) {
            cout << buf.str();
            return 0;
        } 
        cerr << buf.str();
        return 1;
    }

    static Vector2f win_size = {DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT};
    static string soundfont = DEFAULT_SF;

    struct context {
        GPU_Target* gpu_render;
        SDL_Window* window;
        SDL_Renderer* renderer;
        SDL_Event events;
        mWin direct_win;
        Vector2f size;
    };

    struct sdCam {
        Vector2f pos;
        Vector2f size;
    };

    static context mContext;

    template <typename T>
    inline int getIndex(vector<T> v, T K)
    {
        auto it = find(v.begin(), v.end(), K);
    
        // If element was found
        if (it != v.end())
        {
        
            // calculating the index
            // of K
            int index = it - v.begin();
            return index;
        }
        else {
            return -1;
        }
    }

    class sdSprite {
        public:
            int x, y, width, height;
            double angle = 0.000001;
            double alpha = 1.0;

            Vector2f scale = {1, 1};
            Vector2 offset;

            Vector3<Uint8> color;

            ShaderProg shader;

            virtual void create(int x, int y, string path) {
                this->x = x;
                this->y = y;
                _tex_gpu = GPU_LoadImage(path.c_str());
                GPU_SetBlendMode(_tex_gpu, GPU_BLEND_NORMAL);
                width = _tex_gpu->w;
                height = _tex_gpu->h;
                color.r = 255;
                color.g = 255;
                color.b = 255;
            }
            virtual void addShader(std::string path) {
                shader.loadShader(path, FRAGMENT, mContext.gpu_render, _tex_gpu);
            }
            GPU_Rect *r;
            virtual void update(float elapsed) {
                _x = x+offset.x;
                _y = y+offset.y;
                GPU_SetRGBA(_tex_gpu, color.r, color.g, color.b, alpha*255);
                r = &_src_rect;
                if (r->w == 0)
                    r = NULL;
                shader.updateShader(static_cast<float>(sdfml::elapsed));
                GPU_Rect dst = {static_cast<float>(_x-_camera->x), static_cast<float>(_y-_camera->y), width*scale.x, height*scale.y};
                GPU_BlitRectX(_tex_gpu, r, mContext.gpu_render, &dst, angle, NULL, NULL, GPU_FLIP_NONE);
                shader.postUpdate();
            }
            virtual void destroy() {
                _x = 0;
                _y = 0;
                _w = 0;
                _h = 0;
                shader.freeShader();
                GPU_FreeImage(_tex_gpu);
            }
            virtual void updateCamera(SDL_Rect* camera) {
                _camera = camera;
            }
            virtual void screenCenter(AXIS axis = XY) {
                switch (axis) {
                    case X:
                        x = (mContext.size.x/2) - (width*scale.x/2);
                        break;
                    case Y:
                        y = (mContext.size.y/2) - (height*scale.y/2);
                        break;
                    case XY:
                        x = (mContext.size.x/2) - (width*scale.x/2);
                        y = (mContext.size.y/2) - (height*scale.y/2);
                        break;
                }
            }
        protected:
            int _x, _y, _w, _h;

            GPU_Rect _src_rect = {0, 0, 0, 0};
            SDL_Rect dummy = {0, 0, 0, 0};
            sdCam _cam;
            SDL_Rect* _camera = &dummy;
            GPU_Image* _tex_gpu;
    };

    inline void focusCamera(SDL_Rect* camera, sdSprite sprite) {
        camera->x = ( sprite.x + (sprite.width / 2) ) - mContext.size.x / 2;
        camera->y = ( sprite.y + (sprite.height / 2) ) - mContext.size.y / 2;

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

    class sdAnimatedSprite : public sdSprite {
        public:
            virtual void create(int x, int y, string path) {
                sdSprite::create(x, y, path);

                startTime = SDL_GetTicks();
            }
            void AddAnimation(string anim_name, vector<SDL_Rect> points) {
                frameRects.insert({anim_name, points});
            }
            void PlayAnimation(string anim_name) {
                current_framename = anim_name;
            }
            virtual void update(float elapsed) {
                sdSprite::update(elapsed);

                if (current_framename != "") {
                    // this will make it so that current_frame will only advance when it needs to
                    int frameToDraw = ((SDL_GetTicks() - startTime) * framerate / 1000) % frameRects[current_framename].size();
                    current_frame = frameToDraw;

                    int sx = frameRects[current_framename][current_frame].x;
                    int sy = frameRects[current_framename][current_frame].y;
                    int sw = frameRects[current_framename][current_frame].w;
                    int sh = frameRects[current_framename][current_frame].h;

                    // support scaling :)
                    width = (frameRects[current_framename][current_frame].w)*scale.x;
                    height = (frameRects[current_framename][current_frame].h)*scale.y;

                    // after setting shit up, we then store it in src_rect.
                    _src_rect = {static_cast<float>(sx), static_cast<float>(sy), static_cast<float>(sw), static_cast<float>(sh)};
                }
            }
            
            int framerate = 24;
            map<string, vector<SDL_Rect>> frameRects;
            string current_framename = "";
            int current_frame = 0;

        private:
            int startTime;
    };

    class sdState {
        public:
            virtual void create() {

            }
            virtual void update(float elapsed) {

            }
            virtual void draw(float elapsed) {
                GPU_Clear(mContext.gpu_render);
                for (int i = 0; i < _sprites.size(); i++) {
                    _sprites[i]->update(elapsed);
                }
            }
            virtual void add(sdSprite* p_spr, bool mut = false) {
                // a mutable sprite cannot be changed after being added.
                if (mut) {
                    _mut_sprites.push_back(*p_spr);
                    _sprites.push_back(&_mut_sprites[_mut_sprites.size() - 1]);
                    return;
                } 
                _sprites.push_back(p_spr);
            }
            vector<sdSprite*> get_spr() {
                return _sprites;
            }
            vector<sdSprite> get_mspr() {
                return _mut_sprites;
            }
            void freeSprites() {
                _mut_sprites.clear();
                _sprites.clear();
            }
            vector<sdSprite> _mut_sprites;
            vector<sdSprite*> _sprites;
    };

    static sdState* curState = nullptr;

    // Parse a TOML at a given path with a table and key.
    // Setting table name to "" will look for the key without a table.
    template <typename T>
    T tomlParse(string path, string table_name = "", string key = "") {
        auto parsed = toml::parse(path);
        if (table_name != "") {
            auto& config_table = toml::find(parsed, table_name);
            return toml::find<T>(config_table, key);
        }
        return toml::find<T>(parsed, key);
    }

    // "touch" a file at a given path.
    // Returns false if file is empty, returns true if not
    inline bool touchFile(string path) {
        std::ofstream file;
        file.open(path, std::ios::app);
        file.close();
        fstream oFile(path);
        oFile.seekg(0,std::ios::end);
        unsigned int size = oFile.tellg();
        if (!size) {
            oFile.close();
            return false;
        }
        oFile.close();
        return true;
    }

    // Export a TOML file to a path from a toml::value.
    inline void tomlExport(string path, toml::value values) {
        string export_ = toml::format(values, 0, 2);
        std::ofstream config;
        config.open(path, std::ofstream::out | std::ofstream::trunc);
        config << export_ << endl;
        config.close();
    }

    inline int init(float width = DEFAULT_WINDOW_WIDTH, float height = DEFAULT_WINDOW_HEIGHT, string window_name = "Unknown", 
                    int win_flags = SDL_WINDOW_OPENGL|SDL_WINDOW_ALLOW_HIGHDPI|SDL_WINDOW_SHOWN) {
        std::ofstream logFile;
        logFile.open("log.txt", std::ofstream::out | std::ofstream::trunc);
        logFile.close();

        if (!touchFile("conf.toml"))
            tomlExport("conf.toml", 
            {
                {"config", {{"soundfont", "data/gm.sf2"}}}
            });
        soundfont = tomlParse<string>("conf.toml", "config", "soundfont");

        sound.init();
        llog("SoLoud", " is now initialized.", NORMAL, __FILENAME__, __LINE__);
        if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
            return llog("SDL", " has failed to initialize.", ERROR_, __FILENAME__, __LINE__);
        llog("SDL", " is now initialized.", NORMAL, __FILENAME__, __LINE__);
        if (TTF_Init() < 0)
            return llog("SDL_ttf", " has failed to initialize.", ERROR_, __FILENAME__, __LINE__);
        llog("SDL_ttf", " is now initialized.", NORMAL, __FILENAME__, __LINE__);
        llog("", "Initialized libraries. Creating a window context.", NORMAL, __FILENAME__, __LINE__);
        GPU_SetPreInitFlags(GPU_INIT_DISABLE_VSYNC);
        mContext.gpu_render = GPU_Init(width, height, GPU_DEFAULT_INIT_FLAGS);

        SDL_SetWindowTitle(SDL_GetWindowFromID(mContext.gpu_render->context->windowID), window_name.c_str());
        //sound.music.loadSoundfont(soundfont);

        SDL_SysWMinfo wmInfo;
        SDL_VERSION(&wmInfo.version);
        SDL_GetWindowWMInfo(SDL_GetWindowFromID(mContext.gpu_render->context->windowID), &wmInfo);
        mContext.direct_win = wmInfo.info.win.window;

        SDL_EventState(SDL_SYSWMEVENT, SDL_ENABLE);

        mContext.size = {width, height};

        return llog("", "Fully finalized initialization. Command over.", NORMAL, __FILENAME__, __LINE__);
    }

    template <typename T>
    using Func = std::function<T()>;

    static vector<float> _sec;
    static vector<Func<int>> _call;
    static vector<bool> _repeats;
    static vector<int> _ticks;

    class sdTimer {
        public:
            void start(float seconds, Func<int> callback, bool repeat = false) {
                _sec.push_back(seconds);
                _call.push_back(callback);
                _repeats.push_back(repeat);
                _ticks.push_back(0);
            }
    };

    static const Uint8* kb;
    static const Uint8* kb_jp;
    static const Uint8* kb_jr;

    static bool lockKeys = false;

    inline bool key_just_pressed(SDL_Scancode code) {
        return kb_jp[code] && !lockKeys;
    }

    inline bool key_just_released(SDL_Scancode code) {
        return kb_jr[code] && !lockKeys;
    }

    inline bool key_pressed(SDL_Scancode code) {
        return kb[code] && !lockKeys;
    }

    inline int Sec2Tick(float time) {
        return FRAMERATE*time;
    }

    inline int update() {
        int lastUpdate = SDL_GetTicks();
        bool run = true;
        kb_jp = SDL_GetKeyboardState(NULL);
        kb_jr = SDL_GetKeyboardState(NULL);
        while (run) {
            while(SDL_PollEvent(&mContext.events)) {
                if(mContext.events.type == SDL_QUIT) {
                    run = false;
                    break;
                }
                if (mContext.events.type == SDL_KEYDOWN) {
                    kb_jp = SDL_GetKeyboardState(NULL);
                    break;
                }
                if (mContext.events.type == SDL_KEYUP) {
                    kb_jr = SDL_GetKeyboardState(NULL);
                    break;
                }
            } 

            int start = SDL_GetPerformanceCounter();

            int current = SDL_GetTicks();
            float dT = (current - lastUpdate) / 1000.0f;

            kb = SDL_GetKeyboardState(NULL);

            if (curState != nullptr)
                curState->update(dT);

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
                                _call[i]();
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

            if (curState != nullptr)
                curState->draw(dT);

            int end = SDL_GetPerformanceCounter();

            float elapsedMS = (float)(end - start) / SDL_GetPerformanceFrequency() * 1000.0f;

            elapsed += 1;

            SDL_Delay(floor((1000.0f/FRAMERATE) - elapsedMS));

            GPU_Flip(mContext.gpu_render);
        }

        if (curState->get_spr().size() > 0) {
            for (auto texture : curState->get_spr()) {
                texture->destroy();
            }
        }
        if (curState->get_mspr().size() > 0) {
            for (auto texture : curState->get_mspr()) {
                texture.destroy();
            }
        }
        GPU_Quit();
        sound.deinit();
        ReleaseConsole();
        TTF_Quit();
        SDL_Quit();

        return 0;
    }

    static sdSprite transitionSprite;
    static sdSprite transitionSprite2;
    static sdTimer fadeTimer;
    static sdTimer switchTimer;

    inline double clamp(double d, double min, double max) {
        const double t = d < min ? min : d;
        return t > max ? max : t;
    }

    inline int lmao(sdState* state) {
        if (curState != nullptr) {
            if (curState->get_spr().size() > 0) {
                for (auto texture : curState->get_spr()) {
                    texture->destroy();
                }
            }
            if (curState->get_mspr().size() > 0) {
                for (auto texture : curState->get_mspr()) {
                    texture.destroy();
                }
            }
            curState->freeSprites();
        }
        curState = state;
        _ticks = {};
        _call = {};
        _repeats = {};
        _sec = {};
        curState->create();
        transitionSprite2.create(0, 0, "data/images/black.png");
        curState->add(&transitionSprite2);
        fadeTimer.start(0, []() {
            transitionSprite2.x = clamp(transitionSprite2.x - (mContext.size.x/FRAMERATE), -mContext.size.x, 0);
            return 0;
        }, true);
        lockKeys = false;

        return 0;
    }

    inline void switchState(sdState* state) {
        lockKeys = true;
        try {
            if (curState != nullptr) {
                transitionSprite.create(mContext.size.x, 0, "data/images/black.png");
                curState->add(&transitionSprite);
                fadeTimer.start(0, []() {
                    transitionSprite.x = clamp(transitionSprite.x - (mContext.size.x/FRAMERATE), 0, mContext.size.x);
                    return 0;
                }, true);
                switchTimer.start(1, [state](){
                    lmao(state);
                    return 0;
                });
            } else {
                lmao(state);
            }
        }
        catch (...) {
            lmao(state);
        }
    }
}
#endif