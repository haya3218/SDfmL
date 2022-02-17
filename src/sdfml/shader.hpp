#ifndef _SHADER_HPP
#define _SHADER_HPP
#include "../SDL2/SDL.h"
#include "../SDL_gpu/SDL_gpu.h"
#include <string>

enum SHADER_TYPE {
    FRAGMENT,
    VERTEX
};

namespace sdfml {
    class ShaderProg {
        public:
            GPU_ShaderBlock fragment;
            GPU_ShaderBlock vertex;

            void loadShader(std::string path, SHADER_TYPE shader_type, GPU_Target* target, GPU_Image* image);
            void loadShaderFromString(std::string source, SHADER_TYPE shader_type, GPU_Target* target, GPU_Image* image);
            void freeShader();
            void updateShader(float elapsed);
            void postUpdate();
        protected:
            Uint32 frag_int = NULL;
            Uint32 vert_int = NULL;

            int iTime = 0;
    };
}

#endif