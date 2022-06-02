/* date = September 27th 2021 11:37 am */

#ifndef RENDER_H
#define RENDER_H

#define R_MAX_INTERNAL_CACHE_VCOUNT 1024

typedef struct R_Renderer {
    M_Arena DefaultArena;
    
    u32 vao;
    u32 vbo;
    u32 program;
} R_Renderer;

void R_InitOpenGL(R_Renderer* _render_state);
void R_BeginRenderOpenGL(R_Renderer* _render_state);
void R_ShutdownOpenGL(R_Renderer* _render_state);
void R_ResizeProjection(R_Renderer* _render_state, rect new_rect);

typedef struct R_Vertex {
    vec2 pos;
    vec2 tex_coords;
    f32  tex_index;
    vec4 color;
    f32  rounding;
    vec2 uidims;
    vec2 uicorner;
} R_Vertex;

typedef struct R_VertexCache {
    R_Vertex* vertices;
    u32 count;
    u32 max_verts;
} R_VertexCache;

R_VertexCache R_VertexCacheCreate(M_Arena* arena, u32 size);
void R_VertexCacheReset(R_VertexCache* cache);
b8   R_VertexCachePush(R_VertexCache* cache, R_Vertex* vertices, u32 vertex_count);
void R_VertexCacheRender(R_VertexCache* cache);

void A_TextureBind(A_Texture texture, u32 s);

#endif //RENDER_H
