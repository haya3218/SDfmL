#include "shader.hpp"

// Loads a shader and prepends version/compatibility info before compiling it.
// Normally, you can just use GPU_LoadShader() for shader source files or GPU_CompileShader() for strings.
// However, some hardware (certain ATI/AMD cards) does not let you put non-#version preprocessing at the top of the file.
// Therefore, I need to prepend the version info here so I can support both GLSL and GLSLES with one shader file.
Uint32 loadShaderFromFile(GPU_ShaderEnum shader_type, const char* filename)
{
    SDL_RWops* rwops;
    Uint32 shader;
    char* source;
    int header_size, file_size;
    const char* header = "";
    GPU_Renderer* renderer = GPU_GetCurrentRenderer();
    
    // Open file
    rwops = SDL_RWFromFile(filename, "rb");
    if(rwops == NULL)
    {
        GPU_PushErrorCode("load_shader", GPU_ERROR_FILE_NOT_FOUND, "Shader file \"%s\" not found", filename);
        return 0;
    }
    
    // Get file size
    file_size = SDL_RWseek(rwops, 0, SEEK_END);
    SDL_RWseek(rwops, 0, SEEK_SET);
    
    // Get size from header
    if(renderer->shader_language == GPU_LANGUAGE_GLSL)
    {
        if(renderer->max_shader_version >= 120)
            header = "#version 120\n";
        else
            header = "#version 110\n";  // Maybe this is good enough?
    }
    else if(renderer->shader_language == GPU_LANGUAGE_GLSLES)
        header = "#version 100\nprecision mediump int;\nprecision mediump float;\n";
    
    header_size = strlen(header);
    
    // Allocate source buffer
    source = (char*)malloc(sizeof(char)*(header_size + file_size + 1));
    
    // Prepend header
    strcpy(source, header);
    
    // Read in source code
    SDL_RWread(rwops, source + strlen(source), 1, file_size);
    source[header_size + file_size] = '\0';
    
    // Compile the shader
    shader = GPU_CompileShader(shader_type, source);
    
    // Clean up
    free(source);
    SDL_RWclose(rwops);
    
    return shader;
}

Uint32 _loadShader(GPU_ShaderEnum shader_type, std::string source) {
    Uint32 shader;
    std::string header = "";
    GPU_Renderer* renderer = GPU_GetCurrentRenderer();

    // Get size from header
    if(renderer->shader_language == GPU_LANGUAGE_GLSL)
    {
        if(renderer->max_shader_version >= 120)
            header = "#version 120\n";
        else
            header = "#version 110\n";  // Maybe this is good enough?
    }
    else if(renderer->shader_language == GPU_LANGUAGE_GLSLES)
        header = "#version 100\nprecision mediump int;\nprecision mediump float;\n";

    std::string final_source = header + source;

    // Compile the shader
    shader = GPU_CompileShader(shader_type, final_source.c_str());

    return shader;
}

void sdfml::ShaderProg::loadShader(std::string path, SHADER_TYPE shader_type, GPU_Target* target, GPU_Image* image) {
    Uint32 s;

    switch (shader_type) {
        case FRAGMENT:
            s = loadShaderFromFile(GPU_FRAGMENT_SHADER, path.c_str());
            frag_int = GPU_LinkShaderProgram(s);

            fragment = GPU_LoadShaderBlock(frag_int, "gpu_Vertex", "gpu_TexCoord", "gpu_Color", "gpu_ModelViewProjectionMatrix");
            GPU_ActivateShaderProgram(frag_int, &fragment);

            if(image != NULL)
            {
                GPU_SetUniformf(GPU_GetUniformLocation(frag_int, "resolution_x"), image->w);
                GPU_SetUniformf(GPU_GetUniformLocation(frag_int, "resolution_y"), image->h);
            }
            GPU_SetUniformf(GPU_GetUniformLocation(frag_int, "screen_w"), target->w);
            GPU_SetUniformf(GPU_GetUniformLocation(frag_int, "screen_h"), target->h);
            iTime = GPU_GetUniformLocation(frag_int, "time");
            break;
        case VERTEX:
            s = loadShaderFromFile(GPU_VERTEX_SHADER, path.c_str());
            vert_int = GPU_LinkShaderProgram(s);

            vertex = GPU_LoadShaderBlock(vert_int, "gpu_Vertex", "gpu_TexCoord", "gpu_Color", "gpu_ModelViewProjectionMatrix");
            GPU_ActivateShaderProgram(vert_int, &vertex);
            if(image != NULL)
            {
                GPU_SetUniformf(GPU_GetUniformLocation(vert_int, "resolution_x"), image->w);
                GPU_SetUniformf(GPU_GetUniformLocation(vert_int, "resolution_y"), image->h);
            }
            GPU_SetUniformf(GPU_GetUniformLocation(vert_int, "screen_w"), target->w);
            GPU_SetUniformf(GPU_GetUniformLocation(vert_int, "screen_h"), target->h);
            iTime = GPU_GetUniformLocation(vert_int, "time");
            break;
    }
}

void sdfml::ShaderProg::loadShaderFromString(std::string source, SHADER_TYPE shader_type, GPU_Target* target, GPU_Image* image) {
    Uint32 s;

    switch (shader_type) {
        case FRAGMENT:
            s = _loadShader(GPU_FRAGMENT_SHADER, source);
            frag_int = GPU_LinkShaderProgram(s);

            fragment = GPU_LoadShaderBlock(frag_int, "gpu_Vertex", "gpu_TexCoord", "gpu_Color", "gpu_ModelViewProjectionMatrix");
            GPU_ActivateShaderProgram(frag_int, &fragment);

            if(image != NULL)
            {
                GPU_SetUniformf(GPU_GetUniformLocation(frag_int, "resolution_x"), image->w);
                GPU_SetUniformf(GPU_GetUniformLocation(frag_int, "resolution_y"), image->h);
            }
            GPU_SetUniformf(GPU_GetUniformLocation(frag_int, "screen_w"), target->w);
            GPU_SetUniformf(GPU_GetUniformLocation(frag_int, "screen_h"), target->h);
            iTime = GPU_GetUniformLocation(frag_int, "time");
            break;
        case VERTEX:
            s = _loadShader(GPU_VERTEX_SHADER, source);
            vert_int = GPU_LinkShaderProgram(s);

            vertex = GPU_LoadShaderBlock(vert_int, "gpu_Vertex", "gpu_TexCoord", "gpu_Color", "gpu_ModelViewProjectionMatrix");
            GPU_ActivateShaderProgram(vert_int, &vertex);
            if(image != NULL)
            {
                GPU_SetUniformf(GPU_GetUniformLocation(vert_int, "resolution_x"), image->w);
                GPU_SetUniformf(GPU_GetUniformLocation(vert_int, "resolution_y"), image->h);
            }
            GPU_SetUniformf(GPU_GetUniformLocation(vert_int, "screen_w"), target->w);
            GPU_SetUniformf(GPU_GetUniformLocation(vert_int, "screen_h"), target->h);
            iTime = GPU_GetUniformLocation(vert_int, "time");
            break;
    }
}

void sdfml::ShaderProg::freeShader() {
    if (frag_int != NULL)
        GPU_FreeShaderProgram(frag_int);
    if (vert_int != NULL)
        GPU_FreeShaderProgram(vert_int);
}

void sdfml::ShaderProg::updateShader(float elapsed) {
    if (frag_int != NULL)
        GPU_ActivateShaderProgram(frag_int, &fragment);
    if (vert_int != NULL)
        GPU_ActivateShaderProgram(vert_int, &vertex);
    if (frag_int != NULL || vert_int != NULL)
        GPU_SetUniformf(iTime, elapsed);
}

void sdfml::ShaderProg::postUpdate() {
    GPU_ActivateShaderProgram(0, NULL);
}