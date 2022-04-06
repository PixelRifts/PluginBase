/* date = September 27th 2021 11:37 am */

#ifndef RENDER_H
#define RENDER_H

#define R_MAX_INTERNAL_CACHE_VCOUNT 1024

void R_InitOpenGL();
void R_BeginRenderOpenGL();
void R_ShutdownOpenGL();

typedef u32 R_Texture;

typedef struct R_Vertex {
    vec2 pos;
    vec2 tex_coords;
    f32  tex_index;
    vec4 color;
    f32  rounding;
    vec2 uidims;
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

R_Texture R_TextureLoad(const char* filename);
R_Texture R_TextureCreateWhite();
void R_TextureBind(R_Texture texture, u32 s);

#endif //RENDER_H
