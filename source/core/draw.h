/* date = October 2nd 2021 11:17 am */

#ifndef DRAW_H
#define DRAW_H

//~ Command Buffer

#define D_COMMAND_BUFFER_GROWTH_FACTOR 4
#define D_COMMAND_BUFFER_INCREASE_CAPACITY(cap) ((cap) < 64 ? 64 : cap * D_COMMAND_BUFFER_GROWTH_FACTOR)

struct D_Drawer;

typedef u32 D_CommandType;
enum {
    Command_Null, // Expects nothing
    
    Command_DrawQuad, // Expects quad(rect), uvs(rect), vec4(color), rounding(f32), texture(u32)
    Command_SetCullRect, // Expects rect(rect)
    Command_SetOffset, // Expects offset(vec2)
    
    Command_MaxCount,
};

typedef struct D_CommandBuffer {
    u8* buffer;
    u32 cap;
    u32 idx;
    
    rect old_cull_quad;
    vec2 old_offset;
    A_Texture* white_texture;
} D_CommandBuffer;

dll_plugin_api void D_CB_Init(struct D_Drawer* drawer, D_CommandBuffer* cb);
dll_plugin_api void D_CB_DrawQuad(D_CommandBuffer* cb, rect quad, A_Texture texture, rect uvs, vec4 color, f32 rounding);
dll_plugin_api void D_CB_SetCullRect(D_CommandBuffer* cb, rect new_cull);
dll_plugin_api void D_CB_SetOffset(D_CommandBuffer* cb, vec2 offset);
dll_plugin_api void D_CB_Free(D_CommandBuffer* cb);

//~ The thing that draws

typedef struct D_Batch {
    R_VertexCache cache;
    A_Texture textures[8];
    u8 tex_count;
} D_Batch;

Array_Prototype(D_BatchArray, D_Batch);

typedef struct D_Drawer {
    M_Arena arena;
    
    D_BatchArray batches;
    u8 current_batch;
    u8 initialized_batches;
    rect cull_quad;
    vec2 offset;
    
    R_Renderer renderer;
    
    A_Texture* white;
} D_Drawer;

void D_Init(D_Drawer* _draw2d_state, A_AssetLoader* loader);
void D_Shutdown(D_Drawer* _draw2d_state);
void D_Resize(D_Drawer* _draw2d_state, rect new_rect);

dll_plugin_api rect D_PushCullRect(D_CommandBuffer* _draw2d_state, rect new_quad);
dll_plugin_api void D_PopCullRect(D_CommandBuffer* _draw2d_state, rect old_quad);
dll_plugin_api vec2 D_PushOffset(D_CommandBuffer* _draw2d_state, vec2 new_offset);
dll_plugin_api void D_PopOffset(D_CommandBuffer* _draw2d_state, vec2 old_offset);

dll_plugin_api void D_DrawQuad(D_CommandBuffer* _draw2d_state, rect quad, A_Texture texture, rect uvs, vec4 color, f32 rounding);
dll_plugin_api void D_DrawQuadC(D_CommandBuffer* _draw2d_state, rect quad, vec4 color, f32 rounding);
dll_plugin_api void D_DrawQuadT(D_CommandBuffer* _draw2d_state, rect quad, A_Texture texture, vec4 tint);
dll_plugin_api void D_DrawQuadST(D_CommandBuffer* _draw2d_state, rect quad, A_Texture texture, rect uvs, vec4 tint);

dll_plugin_api void D_DrawString(D_CommandBuffer* _draw2d_state, A_FontInfo* fontinfo, vec2 pos, string str);
dll_plugin_api void D_DrawStringC(D_CommandBuffer* _draw2d_state, A_FontInfo* fontinfo, vec2 pos, string str, vec4 color);
dll_plugin_api f32 D_GetStringSize(A_FontInfo* fontinfo, string str);

dll_plugin_api void D_SubmitCommandBuffer(D_Drawer* drawer, D_CommandBuffer* command_buffer);

void D_BeginDraw(D_Drawer* _draw2d_state);
void D_EndDraw(D_Drawer* _draw2d_state);

#endif //DRAW_H
