#include "shaders.h"

void R_InitOpenGL(R_Renderer* _render_state) {
    _render_state->DefaultArena = (M_Arena) {0};
    arena_init(&_render_state->DefaultArena);
    
    //- Buffers 
    {
        glGenVertexArrays(1, &_render_state->vao);
        glBindVertexArray(_render_state->vao);
        
        glGenBuffers(1, &_render_state->vbo);
        glBindBuffer(GL_ARRAY_BUFFER, _render_state->vbo);
        glBufferData(GL_ARRAY_BUFFER, R_MAX_INTERNAL_CACHE_VCOUNT * sizeof(R_Vertex), nullptr, GL_DYNAMIC_DRAW);
        
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(R_Vertex), (void*)offsetof(R_Vertex, pos));
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(R_Vertex), (void*)offsetof(R_Vertex, tex_coords));
        glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(R_Vertex), (void*)offsetof(R_Vertex, tex_index));
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(R_Vertex), (void*)offsetof(R_Vertex, color));
        glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(R_Vertex), (void*)offsetof(R_Vertex, rounding));
        glVertexAttribPointer(5, 2, GL_FLOAT, GL_FALSE, sizeof(R_Vertex), (void*)offsetof(R_Vertex, uidims));
        glVertexAttribPointer(6, 2, GL_FLOAT, GL_FALSE, sizeof(R_Vertex), (void*)offsetof(R_Vertex, uicorner));
        
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);
        glEnableVertexAttribArray(3);
        glEnableVertexAttribArray(4);
        glEnableVertexAttribArray(5);
        glEnableVertexAttribArray(6);
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
        
        _render_state->program = glCreateProgram();
        glAttachShader(_render_state->program, vs);
        glAttachShader(_render_state->program, fs);
        glLinkProgram(_render_state->program);
        glGetProgramiv(_render_state->program, GL_LINK_STATUS, &ret);
        if (ret == GL_FALSE) {
            i32 length;
            glGetProgramiv(_render_state->program, GL_INFO_LOG_LENGTH, &length);
            GLchar *info = calloc(length, sizeof(GLchar));
            glGetProgramInfoLog(_render_state->program, length, NULL, info);
            fprintf(stderr, "glLinkProgram failed:\n%s\n", info);
            free(info);
            exit(-22);
        }
        glValidateProgram(_render_state->program);
        glGetProgramiv(_render_state->program, GL_VALIDATE_STATUS, &ret);
        if (ret == GL_FALSE) {
            i32 length;
            glGetProgramiv(_render_state->program, GL_INFO_LOG_LENGTH, &length);
            GLchar *info = calloc(length, sizeof(GLchar));
            glGetProgramInfoLog(_render_state->program, length, NULL, info);
            fprintf(stderr, "glValidateProgram failed:\n%s\n", info);
            free(info);
            exit(-23);
        }
        glDetachShader(_render_state->program, vs);
        glDetachShader(_render_state->program, fs);
        glDeleteShader(vs);
        glDeleteShader(fs);
        
        glUseProgram(_render_state->program);
        
        i32 textures[] = { 0, 1, 2, 3, 4, 5, 6, 7 };
        i32 loc = glGetUniformLocation(_render_state->program, "u_tex");
        glUniform1iv(loc, 8, textures);
        mat4 projection = mat4_transpose(mat4_ortho(0, 1080, 0, 720, -1, 1000));
        loc = glGetUniformLocation(_render_state->program, "u_projection");
        glUniformMatrix4fv(loc, 1, GL_FALSE, projection.a);
    }
    
    //- State 
    //glClearColor(.05f, .05f, .05f, 1.f);
    glClearColor(1.0f, 1.0f, 1.0f, 1.f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void R_BeginRenderOpenGL(R_Renderer* _render_state) {
    glClear(GL_COLOR_BUFFER_BIT);
    glBindBuffer(GL_ARRAY_BUFFER, _render_state->vbo);
    glBindVertexArray(_render_state->vao);
    glUseProgram(_render_state->program);
}

void R_ShutdownOpenGL(R_Renderer* _render_state) {
    glDeleteBuffers(1, &_render_state->vbo);
    glDeleteVertexArrays(1, &_render_state->vao);
    glDeleteProgram(_render_state->program);
    
    arena_free(&_render_state->DefaultArena);
}

void R_ResizeProjection(R_Renderer* _render_state, rect new_rect) {
    mat4 projection = mat4_transpose(mat4_ortho(new_rect.x, new_rect.x + new_rect.w, new_rect.y, new_rect.y + new_rect.h, -1, 1000));
    glUseProgram(_render_state->program);
    u32 loc = glGetUniformLocation(_render_state->program, "u_projection");
    glUniformMatrix4fv(loc, 1, GL_FALSE, projection.a);
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

void A_TextureBind(A_Texture texture, u32 s) {
    glActiveTexture(GL_TEXTURE0 + s);
    glBindTexture(GL_TEXTURE_2D, texture);
}
