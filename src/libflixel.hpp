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

enum lLOG_TYPE {
    NORMAL,
    WARNING,
    ERROR_
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

    static SoLoud::Soloud audio;
    static SoLoud::SoundFont sf;

    class sdSprite {
        public:
            int x, y, width, height;
            double angle = 0;
            double alpha = 1.0;

            Vector2f scale = {1, 1};
            Vector2 offset;

            virtual void create(int x, int y, string path) {
                this->x = x;
                this->y = y;
                _tex = STBIMG_LoadTexture(mContext.renderer, path.c_str());
                SDL_SetTextureBlendMode(_tex, SDL_BLENDMODE_BLEND);
                SDL_QueryTexture(_tex, NULL, NULL, &width, &height);
            }
            SDL_Rect emptyRect = {0, 0, 0, 0};
            SDL_Rect *r;
            virtual void update(float elapsed) {
                _x = x+offset.x;
                _y = y+offset.y;
                _w = width*scale.x;
                _h = height*scale.y;

                _sc.x = _x-_cam.pos.x;
                _sc.y = _y-_cam.pos.y;
                _sc.w = _w;
                _sc.h = _h;

                SDL_SetTextureAlphaMod(_tex, alpha*255);

                r = &_src_rect;

                if (r == &emptyRect)
                    r = NULL;
                SDL_RenderCopyEx(mContext.renderer, _tex, r, &_sc, angle, NULL, SDL_FLIP_NONE);
            }
            virtual void destroy() {
                _x = 0;
                _y = 0;
                _w = 0;
                _h = 0;
                _sc = {0, 0, 0, 0};
                SDL_DestroyTexture(_tex);
            }
            virtual SDL_Rect get_sc() {
                return _sc;
            }
            virtual void set_sc(SDL_Rect rect) {
                _sc = rect;
            }
            virtual void src_rect(SDL_Rect rect) {
                _src_rect = rect;
            }
            virtual void updateCamera(sdCam camera) {
                _cam = camera;
            }
        private:
            int _x, _y, _w, _h;

            SDL_Rect _sc;
            SDL_Rect _src_rect = {0, 0, 0, 0};
            sdCam _cam;
            SDL_Texture* _tex;
    };

    inline void focusCamera(sdCam* camera, sdSprite sprite) {
        camera->pos.x = ( sprite.x + (sprite.width / 2) ) - mContext.size.x / 2;
        camera->pos.y = ( sprite.y + (sprite.height / 2) ) - mContext.size.y / 2;

        if( camera->pos.x < 0 )
        { 
            camera->pos.x = 0;
        }
        if( camera->pos.y < 0 )
        {
            camera->pos.y = 0;
        }
        if( camera->pos.x > camera->size.x )
        {
            camera->pos.x = camera->size.x;
        }
        if( camera->pos.y > camera->size.y )
        {
            camera->pos.y = camera->size.y;
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
                    src_rect({sx, sy, sw, sh});
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
                SDL_RenderClear(mContext.renderer);
                for (int i = 0; i < _sprites.size(); i++) {
                    _sprites[i]->update(elapsed);
                }
                SDL_RenderPresent(mContext.renderer);
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
        private:
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

        if (audio.init() > 0)
            return llog("SoLoud", " has failed to initialize.", ERROR_, __FILENAME__, __LINE__);
        llog("SoLoud", " is now initialized.", NORMAL, __FILENAME__, __LINE__);
        if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
            return llog("SDL", " has failed to initialize.", ERROR_, __FILENAME__, __LINE__);
        llog("SDL", " is now initialized.", NORMAL, __FILENAME__, __LINE__);
        if (TTF_Init() < 0)
            return llog("SDL_ttf", " has failed to initialize.", ERROR_, __FILENAME__, __LINE__);
        llog("SDL_ttf", " is now initialized.", NORMAL, __FILENAME__, __LINE__);
        llog("", "Initialized libraries. Creating a window context.", NORMAL, __FILENAME__, __LINE__);
        mContext.window = SDL_CreateWindow(window_name.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, (int)width, (int)height, win_flags);
        if (mContext.window == nullptr)
            return llog("", "Failed to create a window.", ERROR_, __FILENAME__, __LINE__);
        mContext.renderer = SDL_CreateRenderer(mContext.window, -1, SDL_RENDERER_ACCELERATED);
        if (mContext.renderer == nullptr)
            return llog("", "Failed to create a renderer.", ERROR_, __FILENAME__, __LINE__);

        sf.load(soundfont.c_str());

        SDL_SysWMinfo wmInfo;
        SDL_VERSION(&wmInfo.version);
        SDL_GetWindowWMInfo(mContext.window, &wmInfo);
        mContext.direct_win = wmInfo.info.win.window;

        SDL_EventState(SDL_SYSWMEVENT, SDL_ENABLE);

        mContext.size = {width, height};

        return llog("", "Fully finalized initialization. Command over.", NORMAL, __FILENAME__, __LINE__);
    }

    static const Uint8* kb;
    static const Uint8* kb_last;

    inline bool key_just_pressed(SDL_Scancode code) {
        if (kb[code] && !kb_last[code])
            return true;
        return false;
    }

    inline bool key_just_released(SDL_Scancode code) {
        if (!kb[code] && kb_last[code])
            return true;
        return false;
    }

    inline bool key_pressed(SDL_Scancode code) {
        if (kb[code])
            return true;
        return false;
    }

    inline int update() {
        int lastUpdate = SDL_GetTicks();
        bool run = true;
        while (run) {
            while(SDL_PollEvent(&mContext.events)) {
                if(mContext.events.type == SDL_QUIT) {
                    run = false;
                    break;
                }
            } 

            int start = SDL_GetPerformanceCounter();

            int current = SDL_GetTicks();
            float dT = (current - lastUpdate) / 1000.0f;

            kb = SDL_GetKeyboardState(NULL);

            if (curState != nullptr)
                curState->update(dT);

            lastUpdate = current;

            if (curState != nullptr)
                curState->draw(dT);

            int end = SDL_GetPerformanceCounter();

            float elapsedMS = (float)(end - start) / SDL_GetPerformanceFrequency() * 1000.0f;

            kb_last = SDL_GetKeyboardState(NULL);

            SDL_Delay(floor((1000.0f/FRAMERATE) - elapsedMS));
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
        SDL_DestroyRenderer(mContext.renderer);
        SDL_DestroyWindow(mContext.window);
        audio.stopAll();
        audio.deinit();
        TTF_Quit();
        SDL_Quit();

        return 0;
    }

    inline void switchState(sdState* state) {
        curState = state;
        curState->create();
    }

    static pair<string, int> curMusic = pair<string, int>("", 0);

    class musplayer {
        public:
            SoLoud::WavStream wav;
            SoLoud::WavStream sfxWav;
            SoLoud::Openmpt mod;

            void playSFX(string path) {
                //audio.stopAudioSource(sfxWav);
                sfxWav.load(path.c_str());
                audio.play(sfxWav);
            }

            void playMus(string path, bool loop = true) {
                audio.stopAudioSource(wav);
                wav.load(path.c_str());
                wav.setLooping(loop);
                curMusic.second = audio.playBackground(wav);
                curMusic.first = path;
            }

            void playMod(string path, bool loop = true) {
                //audio.stopAudioSource(mod);
                mod.load(path.c_str());
                mod.setLooping(loop);
                curMusic.second = audio.playBackground(mod);
                curMusic.first = path;
            }
    };
}
#endif