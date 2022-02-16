#include "music.hpp"

std::array<SoLoud::WavStream, MAX_SFX> sdfml::sfxBanks;
#ifdef PREFER_MODPLUG
std::pair<SoLoud::WavStream, SoLoud::Modplug> sdfml::musicBank;
#else
std::pair<SoLoud::WavStream, SoLoud::Openmpt> sdfml::musicBank;
#endif
SoLoud::Soloud sdfml::audio;
sdfml::AudioHandler sdfml::sound;