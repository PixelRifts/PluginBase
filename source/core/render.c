#include <string.h>
#include <assert.h>
#include <stdio.h>

#include <stb/stb_image.h>

#include "shaders.h"

typedef struct R_State {
    M_Arena DefaultArena;
    
    u32 vao;
    u32 vbo;
    u32 program;
} R_State;

static R_State _render_state = {0};

void R_InitOpenGL() {
    _render_state.DefaultArena = (M_Arena) {0};
    arena_init(&_render_state.DefaultArena);
    
    //- Buffers 
    {
        glGenVertexArrays(1, &_render_state.vao);
        glBindVertexArray(_render_state.vao);
        
        glGenBuffers(1, &_render_state.vbo);
        glBindBuffer(GL_ARRAY_BUFFER, _render_state.vbo);
        glBufferData(GL_ARRAY_BUFFER, R_MAX_INTERNAL_CACHE_VCOUNT * sizeof(R_Vertex), nullptr, GL_DYNAMIC_DRAW);
        
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(R_Vertex), (void*)offsetof(R_Vertex, pos));
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(R_Vertex), (void*)offsetof(R_Vertex, tex_coords));
        glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(R_Vertex), (void*)offsetof(R_Vertex, tex_index));
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(R_Vertex), (void*)offsetof(R_Vertex, color));
        glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(R_Vertex), (void*)offsetof(R_Vertex, rounding));
        glVertexAttribPointer(5, 2, GL_FLOAT, GL_FALSE, sizeof(R_Vertex), (void*)offsetof(R_Vertex, uidims));
        
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);
        glEnableVertexAttribArray(3);
        glEnableVertexAttribArray(4);
        glEnableVertexAttribArray(5);
    }
    
    //- Shaders 
    {
        i32 ret;
        u32 vs = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vs, 1, &v_src, nullptr);
        glCompileShader(vs);
        glGetShaderiv(vs, GL_COMPILE_STATUS, &ret);
        if (ret == GL_FALSE) {
            i32 length;
            glGetShaderiv(vs, GL_INFO_LOG_LENGTH, &length);
            GLchar *info = calloc(length, sizeof(GLchar));
            glGetShaderInfoLog(vs, length, NULL, info);
            fprintf(stderr, "glCompileShader(vertex) failed:\n%s\n", info);
            free(info);
            exit(-20);
        }
        
        u32 fs = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fs, 1, &f_src, nullptr);
        glCompileShader(fs);
        glGetShaderiv(fs, GL_COMPILE_STATUS, &ret);
        if (ret == GL_FALSE) {
            i32 length;
            glGetShaderiv(fs, GL_INFO_LOG_LENGTH, &length);
            GLchar *info = calloc(length, sizeof(GLchar));
            glGetShaderInfoLog(fs, length, NULL, info);
            fprintf(stderr, "glCompileShader(fragment) failed:\n%s\n", info);
            free(info);
            exit(-21);
        }
        
        _render_state.program = glCreateProgram();
        glAttachShader(_render_state.program, vs);
        glAttachShader(_render_state.program, fs);
        glLinkProgram(_render_state.program);
        glGetProgramiv(_render_state.program, GL_LINK_STATUS, &ret);
        if (ret == GL_FALSE) {
            i32 length;
            glGetProgramiv(_render_state.program, GL_INFO_LOG_LENGTH, &length);
            GLchar *info = calloc(length, sizeof(GLchar));
            glGetProgramInfoLog(_render_state.program, length, NULL, info);
            fprintf(stderr, "glLinkProgram failed:\n%s\n", info);
            free(info);
            exit(-22);
        }
        glValidateProgram(_render_state.program);
        glGetProgramiv(_render_state.program, GL_VALIDATE_STATUS, &ret);
        if (ret == GL_FALSE) {
            i32 length;
            glGetProgramiv(_render_state.program, GL_INFO_LOG_LENGTH, &length);
            GLchar *info = calloc(length, sizeof(GLchar));
            glGetProgramInfoLog(_render_state.program, length, NULL, info);
            fprintf(stderr, "glValidateProgram failed:\n%s\n", info);
            free(info);
            exit(-23);
        }
        glDetachShader(_render_state.program, vs);
        glDetachShader(_render_state.program, fs);
        glDeleteShader(vs);
        glDeleteShader(fs);
        
        glUseProgram(_render_state.program);
        
        i32 textures[] = { 0, 1, 2, 3, 4, 5, 6, 7 };
        i32 loc = glGetUniformLocation(_render_state.program, "u_tex");
        glUniform1iv(loc, 8, textures);
        mat4 projection = mat4_transpose(mat4_ortho(0, 1080, 0, 720, -1, 1000));
        loc = glGetUniformLocation(_render_state.program, "u_projection");
        glUniformMatrix4fv(loc, 1, GL_FALSE, projection.a);
    }
    
    //- State 
    //glClearColor(.05f, .05f, .05f, 1.f);
    glClearColor(1.0f, 1.0f, 1.0f, 1.f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void R_BeginRenderOpenGL() {
    glClear(GL_COLOR_BUFFER_BIT);
    glBindBuffer(GL_ARRAY_BUFFER, _render_state.vbo);
    glBindVertexArray(_render_state.vao);
    glUseProgram(_render_state.program);
}

void R_ShutdownOpenGL() {
    glDeleteBuffers(1, &_render_state.vbo);
    glDeleteVertexArrays(1, &_render_state.vao);
    glDeleteProgram(_render_state.program);
    
    arena_free(&_render_state.DefaultArena);
}

R_VertexCache R_VertexCacheCreate(M_Arena* arena, u32 max_verts) {
    return (R_VertexCache) {
        .vertices = arena_alloc(arena, sizeof(R_Vertex) * max_verts),
        .count = 0,
        .max_verts = max_verts
    };
}

void R_VertexCacheReset(R_VertexCache* cache) {
    cache->count = 0;
}

b8 R_VertexCachePush(R_VertexCache* cache, R_Vertex* vertices, u32 vertex_count) {
    if (cache->max_verts < cache->count + vertex_count)
        return false;
    memcpy(cache->vertices + cache->count, vertices, sizeof(R_Vertex) * vertex_count);
    cache->count += vertex_count;
    return true;
}

void R_VertexCacheRender(R_VertexCache* cache) {
    glBufferSubData(GL_ARRAY_BUFFER, 0, cache->count * sizeof(R_Vertex), (void*) cache->vertices);
    glDrawArrays(GL_TRIANGLES, 0, cache->count);
}

R_Texture R_TextureLoad(const char* filename) {
    R_Texture texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    i32 width, height, channels;
    stbi_set_flip_vertically_on_load(true);
    u8* data = stbi_load(filename, &width, &height, &channels, 0);
    if (data != nullptr) {
        if (channels == 3) {
            // RGB
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        } else if (channels == 4) {
            // RGBA
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        }
    }
    stbi_image_free(data);
    return texture;
}

R_Texture R_TextureCreateWhite() {
    R_Texture texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    u8 data[4] = { 255, 255, 255, 255 };
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    return texture;
}

void R_TextureBind(R_Texture texture, u32 s) {
    glActiveTexture(GL_TEXTURE0 + s);
    glBindTexture(GL_TEXTURE_2D, texture);
}
