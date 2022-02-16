#ifndef _MUSIC_H
#define _MUSIC_H
#include "../SoLoud/soloud.h"
#include "../SoLoud/soloud_wavstream.h"
#include "../SoLoud/soloud_speech.h"
#include "../SoLoud/soloud_modplug.h"
#include "../SoLoud/soloud_openmpt.h"
#include "../SoLoud/MIDI/soloud_midi.h"
#include <array>
#include <string>
#include <utility>

#define MAX_SFX 10

namespace sdfml {

    extern std::array<SoLoud::WavStream, MAX_SFX> sfxBanks;
    #ifdef PREFER_MODPLUG
    extern std::pair<SoLoud::WavStream, SoLoud::Modplug> musicBank;
    #else
    extern std::pair<SoLoud::WavStream, SoLoud::Openmpt> musicBank;
    #endif
    
    extern SoLoud::Soloud audio;

    class MusicHandler {
        public:
        
            void playMusic(std::string path) {
                musicBank.first.stop();
                audio.stopAudioSource(musicBank.first);
                musicBank.first.load(path.c_str());
                musicBank.first.setLooping(true);
                audio.play(musicBank.first);
            }

            void playMod(std::string path) {
                musicBank.second.stop();
                audio.stopAudioSource(musicBank.second);
                musicBank.second.load(path.c_str());
                musicBank.second.setLooping(true);
                audio.playBackground(musicBank.second);
            }
    };

    class AudioHandler {
        public:
            MusicHandler music;

            void init() {
                audio.init();
            }

            void deinit() {
                audio.stopAll();
                audio.deinit();
            }

            void play(std::string path) {
                for (int i = 0; i < MAX_SFX; i++) {
                    if (sfxBanks[i].getLength() > 0) {
                        sfxBanks[i].load(path.c_str());
                        audio.play(sfxBanks[i]);
                        break;
                    } else {
                        sfxBanks[i].stop();
                        audio.stopAudioSource(sfxBanks[i]);
                        sfxBanks[i].load(path.c_str());
                        audio.play(sfxBanks[i]);
                        break;
                    }
                }
            }
    };

    extern AudioHandler sound;
}
#endif