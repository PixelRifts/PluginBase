/* date = October 2nd 2021 11:17 am */

#ifndef DRAW_H
#define DRAW_H

#include <stb/stb_truetype.h>

typedef struct D_Batch {
    R_VertexCache cache;
    R_Texture textures[8];
    u8 tex_count;
} D_Batch;

Array_Prototype(D_BatchArray, D_Batch);

typedef struct D_FontInfo {
    R_Texture font_texture;
    stbtt_packedchar cdata[95];
    f32 scale;
    f32 font_size;
    i32 ascent;
    i32 descent;
    i32 baseline;
} D_FontInfo;

typedef struct D_Drawer {
    M_Arena arena;
    
    D_BatchArray batches;
    u8 current_batch;
    u8 initialized_batches;
    rect cull_quad;
    vec2 offset;
    
    R_Renderer renderer;
    
    R_Texture white;
} D_Drawer;

void D_Init(D_Drawer* _draw2d_state);
void D_Shutdown(D_Drawer* _draw2d_state);

rect D_PushCullRect(D_Drawer* _draw2d_state, rect new_quad);
void D_PopCullRect(D_Drawer* _draw2d_state, rect old_quad);
vec2 D_PushOffset(D_Drawer* _draw2d_state, vec2 new_offset);
void D_PopOffset(D_Drawer* _draw2d_state, vec2 old_offset);

dll_plugin_api void D_DrawQuad(D_Drawer* _draw2d_state, rect quad, R_Texture texture, rect uvs, vec4 color, f32 rounding);
dll_plugin_api void D_DrawQuadC(D_Drawer* _draw2d_state, rect quad, vec4 color, f32 rounding);
dll_plugin_api void D_DrawQuadT(D_Drawer* _draw2d_state, rect quad, R_Texture texture, vec4 tint);
dll_plugin_api void D_DrawQuadST(D_Drawer* _draw2d_state, rect quad, R_Texture texture, rect uvs, vec4 tint);

dll_plugin_api void D_DrawString(D_Drawer* _draw2d_state, D_FontInfo* fontinfo, vec2 pos, string str);
dll_plugin_api void D_DrawStringC(D_Drawer* _draw2d_state, D_FontInfo* fontinfo, vec2 pos, string str, vec4 color);
dll_plugin_api f32 D_GetStringSize(D_FontInfo* fontinfo, string str);

void D_LoadFont(D_FontInfo* fontinfo, string filename, f32 size);
void D_FreeFont(D_FontInfo* font);

void D_BeginDraw(D_Drawer* _draw2d_state);
void D_EndDraw(D_Drawer* _draw2d_state);

#endif //DRAW_H
