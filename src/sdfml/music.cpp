#include "music.hpp"

std::array<SoLoud::WavStream, MAX_SFX> sdfml::sfxBanks;
std::pair<SoLoud::WavStream, SoLoud::Openmpt> sdfml::musicBank;
SoLoud::Soloud sdfml::audio;
sdfml::AudioHandler sdfml::sound;