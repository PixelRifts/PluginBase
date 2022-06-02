#include <glad/glad.h>

#include <stdio.h>
#include <assert.h>

//~ Actually draw stuff

Array_Impl(D_BatchArray, D_Batch);

static D_Batch* D_NextBatch(D_Drawer* _draw2d_state) {
    D_BatchArray_add(&_draw2d_state->batches, (D_Batch) {0});
    D_Batch* next = &_draw2d_state->batches.elems[++_draw2d_state->current_batch];
    
    if (_draw2d_state->current_batch >= _draw2d_state->initialized_batches) {
        next->cache = R_VertexCacheCreate(&_draw2d_state->arena, R_MAX_INTERNAL_CACHE_VCOUNT);
        _draw2d_state->initialized_batches++;
    }
    return next;
}

static b8 D_BatchCanAddTexture(D_Drawer* _draw2d_state, D_Batch* batch, A_Texture texture) {
    if (batch->tex_count < 8) return true;
    for (u8 i = 0; i < batch->tex_count; i++) {
        if (batch->textures[i] == texture)
            return true;
    }
    return false;
}

static u8 D_BatchAddTexture(D_Drawer* _draw2d_state, D_Batch* batch, A_Texture tex) {
    for (u8 i = 0; i < batch->tex_count; i++) {
        if (batch->textures[i] == tex)
            return i;
    }
    batch->textures[batch->tex_count] = tex;
    return batch->tex_count++;
}

static D_Batch* D_GetCurrentBatch(D_Drawer* _draw2d_state, int num_verts, A_Texture tex) {
    D_Batch* batch = &_draw2d_state->batches.elems[_draw2d_state->current_batch];
    if (!D_BatchCanAddTexture(_draw2d_state, batch, tex) || batch->cache.count + num_verts >= batch->cache.max_verts)
        batch = D_NextBatch(_draw2d_state);
    return batch;
}

static void D_BatchDrawQuad(D_Drawer* drawer, rect quad, A_Texture texture, rect uvs, vec4 color, f32 rounding) {
    quad.x += drawer->offset.x;
    quad.y += drawer->offset.y;
    
    if (!rect_overlaps(quad, drawer->cull_quad)) return;
    
    D_Batch* batch = D_GetCurrentBatch(drawer, 6, texture);
    int idx = D_BatchAddTexture(drawer, batch, texture);
    rect uv_culled = rect_uv_cull(quad, uvs, drawer->cull_quad);
    R_Vertex vertices[6] = {
        {
            .pos = vec2_clamp(vec2_init(quad.x, quad.y), drawer->cull_quad),
            .tex_index = idx,
            .tex_coords = vec2_init(uv_culled.x, uv_culled.y),
            .color = color,
            .rounding = rounding,
            .uidims = vec2_init(quad.w, quad.h),
            .uicorner = vec2_init(0.f, 0.f),
        },
        {
            .pos = vec2_clamp(vec2_init(quad.x + quad.w, quad.y), drawer->cull_quad),
            .tex_index = idx,
            .tex_coords = vec2_init(uv_culled.x + uv_culled.w, uv_culled.y),
            .color = color,
            .rounding = rounding,
            .uidims = vec2_init(quad.w, quad.h),
            .uicorner = vec2_init(1.f, 0.f),
        },
        {
            .pos = vec2_clamp(vec2_init(quad.x + quad.w, quad.y + quad.h), drawer->cull_quad),
            .tex_index = idx,
            .tex_coords = vec2_init(uv_culled.x + uv_culled.w, uv_culled.y + uv_culled.h),
            .color = color,
            .rounding = rounding,
            .uidims = vec2_init(quad.w, quad.h),
            .uicorner = vec2_init(1.f, 1.f),
        },
        {
            .pos = vec2_clamp(vec2_init(quad.x, quad.y), drawer->cull_quad),
            .tex_index = idx,
            .tex_coords = vec2_init(uv_culled.x, uv_culled.y),
            .color = color,
            .rounding = rounding,
            .uidims = vec2_init(quad.w, quad.h),
            .uicorner = vec2_init(0.f, 0.f),
        },
        {
            .pos = vec2_clamp(vec2_init(quad.x + quad.w, quad.y + quad.h), drawer->cull_quad),
            .tex_index = idx,
            .tex_coords = vec2_init(uv_culled.x + uv_culled.w, uv_culled.y + uv_culled.h),
            .color = color,
            .rounding = rounding,
            .uidims = vec2_init(quad.w, quad.h),
            .uicorner = vec2_init(1.f, 1.f),
        },
        {
            .pos = vec2_clamp(vec2_init(quad.x, quad.y + quad.h), drawer->cull_quad),
            .tex_index = idx,
            .tex_coords = vec2_init(uv_culled.x, uv_culled.y + uv_culled.h),
            .color = color,
            .rounding = rounding,
            .uidims = vec2_init(quad.w, quad.h),
            .uicorner = vec2_init(0.f, 1.f),
        },
    };
    R_VertexCachePush(&batch->cache, vertices, 6);
}

//~ Command Buffer

static void D_CB_PushBytes(D_CommandBuffer* cb, u8* from, u32 count) {
    if (cb->idx + count > cb->cap) {
        // reallocate buffer
        u8* old_buffer = cb->buffer;
        cb->cap = D_COMMAND_BUFFER_INCREASE_CAPACITY(cb->cap);
        cb->buffer = calloc(cb->cap, sizeof(u8));
        if (old_buffer) {
            memmove(cb->buffer, old_buffer, cb->idx);
            free(old_buffer);
        }
    }
    memmove(cb->buffer + cb->idx, from, count);
    cb->idx += count;
}

static void D_CB_Opcode(D_CommandBuffer* cb, u32 op) {
    u32 op_a = op;
    D_CB_PushBytes(cb, (u8*)&op_a, sizeof(u32));
}

dll_plugin_api void D_CB_Init(D_Drawer* drawer, D_CommandBuffer* cb) {
    cb->white_texture = drawer->white;
    cb->old_cull_quad = (rect) { 0, 0, 1080, 720 };
    cb->old_offset = (vec2) { 0.f, 0.f };
}

dll_plugin_api void D_CB_DrawQuad(D_CommandBuffer* cb, rect quad, A_Texture texture, rect uvs, vec4 color, f32 rounding) {
    f32 f32_data_a[] = { quad.x, quad.y, quad.w, quad.h, uvs.x, uvs.y, uvs.w, uvs.h, color.x, color.y, color.z, color.w, rounding };
    u32 tex_a[] = { texture };
    
    D_CB_Opcode(cb, Command_DrawQuad);
    D_CB_PushBytes(cb, (u8*)f32_data_a, sizeof(f32_data_a));
    D_CB_PushBytes(cb, (u8*)tex_a, sizeof(tex_a));
}

dll_plugin_api void D_CB_SetCullRect(D_CommandBuffer* cb, rect new_cull) {
    rect new_cull_a = new_cull;
    D_CB_Opcode(cb, Command_SetCullRect);
    D_CB_PushBytes(cb, (u8*)&new_cull_a, sizeof(rect));
}

dll_plugin_api void D_CB_SetOffset(D_CommandBuffer* cb, vec2 offset) {
    vec2 offset_a = offset;
    D_CB_Opcode(cb, Command_SetOffset);
    D_CB_PushBytes(cb, (u8*)&offset_a, sizeof(vec2));
}

#define CommandBuffer_Read(type) *((type*) (cb->buffer + i))
dll_plugin_api void D_SubmitCommandBuffer(D_Drawer* drawer, D_CommandBuffer* cb) {
    for (u32 i = 0; i < cb->idx;) {
        u32 opcode = CommandBuffer_Read(u32);
        i += sizeof(u32);
        
        switch (opcode) {
            case Command_DrawQuad: {
                rect quad = CommandBuffer_Read(rect);
                i += sizeof(rect);
                rect uvs = CommandBuffer_Read(rect);
                i += sizeof(rect);
                vec4 color = CommandBuffer_Read(vec4);
                i += sizeof(vec4);
                f32 rounding = CommandBuffer_Read(f32);
                i += sizeof(f32);
                u32 texture = CommandBuffer_Read(u32);
                i += sizeof(u32);
                
                // Draw Quad Command
                D_BatchDrawQuad(drawer, quad, texture, uvs, color, rounding);
            } break;
            
            case Command_SetCullRect: {
                rect quad = CommandBuffer_Read(rect);
                i += sizeof(rect);
                
                drawer->cull_quad = quad;
            } break;
            
            
            case Command_SetOffset: {
                vec2 off = CommandBuffer_Read(vec2);
                i += sizeof(vec2);
                
                drawer->offset = off;
            } break;
        }
    }
}
#undef CommandBuffer_Read

dll_plugin_api void D_CB_Free(D_CommandBuffer* cb) {
    free(cb->buffer);
}

//~ The thing that draws

void D_Init(D_Drawer* _draw2d_state, A_AssetLoader* loader) {
    R_InitOpenGL(&_draw2d_state->renderer);
    arena_init(&_draw2d_state->arena);
    _draw2d_state->initialized_batches = 1;
    _draw2d_state->current_batch = 0;
    _draw2d_state->cull_quad = (rect) { 0, 0, 1080, 720 };
    _draw2d_state->offset = (vec2) { 0.f, 0.f };
    D_BatchArray_add(&_draw2d_state->batches, (D_Batch) {0});
    _draw2d_state->batches.elems[_draw2d_state->current_batch].cache = R_VertexCacheCreate(&_draw2d_state->arena, R_MAX_INTERNAL_CACHE_VCOUNT);
    A_RequestLoadWhiteTexture(loader, &_draw2d_state->white);
}

void D_Shutdown(D_Drawer* _draw2d_state) {
    arena_free(&_draw2d_state->arena);
    R_ShutdownOpenGL(&_draw2d_state->renderer);
}

void D_Resize(D_Drawer* _draw2d_state, rect new_rect) {
    _draw2d_state->cull_quad = new_rect;
    R_ResizeProjection(&_draw2d_state->renderer, new_rect);
}

dll_plugin_api rect D_PushCullRect(D_CommandBuffer* cb, rect new_quad) {
    rect ret = cb->old_cull_quad;
    D_CB_SetCullRect(cb, new_quad);
    cb->old_cull_quad = new_quad;
    return ret;
}

dll_plugin_api void D_PopCullRect(D_CommandBuffer* cb, rect old_quad) {
    cb->old_cull_quad = old_quad;
    D_CB_SetCullRect(cb, old_quad);
}

dll_plugin_api vec2 D_PushOffset(D_CommandBuffer* cb, vec2 new_offset) {
    vec2 ret = cb->old_offset;
    D_CB_SetOffset(cb, new_offset);
    cb->old_offset = new_offset;
    return ret;
}

dll_plugin_api void D_PopOffset(D_CommandBuffer* cb, vec2 old_offset) {
    cb->old_offset = old_offset;
    D_CB_SetOffset(cb, old_offset);
}

dll_plugin_api void D_DrawQuad(D_CommandBuffer* cb, rect quad, A_Texture texture, rect uvs, vec4 color, f32 rounding)  {
    D_CB_DrawQuad(cb, quad, texture, uvs, color, rounding);
}

dll_plugin_api void D_DrawQuadC(D_CommandBuffer* cb, rect quad, vec4 color, f32 rounding) {
    D_DrawQuad(cb, quad, *cb->white_texture, rect_init(0.f, 0.f, 1.f, 1.f), color, rounding);
}

dll_plugin_api void D_DrawQuadT(D_CommandBuffer* cb, rect quad, A_Texture texture, vec4 tint) {
    D_DrawQuad(cb, quad, texture, rect_init(0.f, 0.f, 1.f, 1.f), tint, 0);
}

dll_plugin_api void D_DrawQuadST(D_CommandBuffer* cb, rect quad, A_Texture texture, rect uvs, vec4 tint) {
    D_DrawQuad(cb, quad, texture, uvs, tint, 0);
}

dll_plugin_api void D_DrawStringC(D_CommandBuffer* cb, A_FontInfo* fontinfo, vec2 pos, string str, vec4 color) {
    for (u32 i = 0; i < str.size; i++) {
        if (str.str[i] >= 32 && str.str[i] < 128) {
            stbtt_packedchar* info = &fontinfo->cdata[str.str[i] - 32];
            rect uvs = { info->x0 / 512.f, info->y0 / 512.f, (info->x1 - info->x0) / 512.f, (info->y1 - info->y0) / 512.f };
            rect loc = { pos.x + info->xoff, pos.y + info->yoff, info->x1 - info->x0, info->y1 - info->y0 };
            D_DrawQuadST(cb, loc, fontinfo->font_texture, uvs, color);
            pos.x += info->xadvance;
        }
    }
}

dll_plugin_api void D_DrawString(D_CommandBuffer* cb, A_FontInfo* fontinfo, vec2 pos, string str) {
    for (u32 i = 0; i < str.size; i++) {
        if (str.str[i] >= 32 && str.str[i] < 128) {
            stbtt_packedchar* info = &fontinfo->cdata[str.str[i] - 32];
            rect uvs = {
                info->x0 / 512.f,
                info->y0 / 512.f,
                (info->x1 - info->x0) / 512.f,
                (info->y1 - info->y0) / 512.f
            };
            rect loc = { pos.x + info->xoff, pos.y + info->yoff, info->x1 - info->x0, info->y1 - info->y0 };
            D_DrawQuadST(cb, loc, fontinfo->font_texture, uvs, vec4_init(1.f, 1.f, 1.f, 1.f));
            pos.x += info->xadvance;
        }
    }
}

dll_plugin_api f32 D_GetStringSize(A_FontInfo* fontinfo, string str) {
    f32 sz = 0.f;
    for (u32 i = 0; i < str.size; i++) {
        if (str.str[i] >= 32 && str.str[i] < 128) {
            stbtt_packedchar* info = &fontinfo->cdata[str.str[i] - 32];
            sz += info->xadvance;
        }
    }
    return sz;
}

void D_BeginDraw(D_Drawer* _draw2d_state) {
    R_BeginRenderOpenGL(&_draw2d_state->renderer);
    for (u8 i = 0; i < _draw2d_state->initialized_batches; i++) {
        R_VertexCacheReset(&_draw2d_state->batches.elems[i].cache);
        _draw2d_state->batches.elems[i].tex_count = 0;
    }
    _draw2d_state->current_batch = 0;
}

void D_EndDraw(D_Drawer* _draw2d_state) {
    for (u8 i = 0; i <= _draw2d_state->current_batch; i++) {
        for (u8 t = 0; t < _draw2d_state->batches.elems[i].tex_count; t++)
            A_TextureBind(_draw2d_state->batches.elems[i].textures[t], t);
        
        R_VertexCacheRender(&_draw2d_state->batches.elems[i].cache);
    }
}
