#include <glad/glad.h>

#include <stdio.h>
#include <assert.h>

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

static b8 D_BatchCanAddTexture(D_Drawer* _draw2d_state, D_Batch* batch, R_Texture texture) {
    if (batch->tex_count < 8) return true;
    for (u8 i = 0; i < batch->tex_count; i++) {
        if (batch->textures[i] == texture)
            return true;
    }
    return false;
}

static u8 D_BatchAddTexture(D_Drawer* _draw2d_state, D_Batch* batch, R_Texture tex) {
    for (u8 i = 0; i < batch->tex_count; i++) {
        if (batch->textures[i] == tex)
            return i;
    }
    batch->textures[batch->tex_count] = tex;
    return batch->tex_count++;
}

static D_Batch* D_GetCurrentBatch(D_Drawer* _draw2d_state, int num_verts, R_Texture tex) {
    D_Batch* batch = &_draw2d_state->batches.elems[_draw2d_state->current_batch];
    if (!D_BatchCanAddTexture(_draw2d_state, batch, tex) || batch->cache.count + num_verts >= batch->cache.max_verts)
        batch = D_NextBatch(_draw2d_state);
    return batch;
}

void D_Init(D_Drawer* _draw2d_state) {
    R_InitOpenGL(&_draw2d_state->renderer);
    arena_init(&_draw2d_state->arena);
    _draw2d_state->initialized_batches = 1;
    _draw2d_state->current_batch = 0;
    _draw2d_state->cull_quad = (rect) { 0, 0, 1080, 720 }; // @resize CULLQUAD
    _draw2d_state->offset = (vec2) { 0.f, 0.f };
    D_BatchArray_add(&_draw2d_state->batches, (D_Batch) {0});
    _draw2d_state->batches.elems[_draw2d_state->current_batch].cache = R_VertexCacheCreate(&_draw2d_state->arena, R_MAX_INTERNAL_CACHE_VCOUNT);
    _draw2d_state->white = R_TextureCreateWhite();
}

void D_Shutdown(D_Drawer* _draw2d_state) {
    arena_free(&_draw2d_state->arena);
    R_ShutdownOpenGL(&_draw2d_state->renderer);
}

rect D_PushCullRect(D_Drawer* _draw2d_state, rect new_quad) {
    rect ret = _draw2d_state->cull_quad;
    _draw2d_state->cull_quad = new_quad;
    return ret;
}

void D_PopCullRect(D_Drawer* _draw2d_state, rect old_quad) {
    _draw2d_state->cull_quad = old_quad;
}

vec2 D_PushOffset(D_Drawer* _draw2d_state, vec2 new_offset) {
    vec2 ret = _draw2d_state->offset;
    _draw2d_state->offset = new_offset;
    return ret;
}

void D_PopOffset(D_Drawer* _draw2d_state, vec2 old_offset) {
    _draw2d_state->offset = old_offset;
}

dll_plugin_api void D_DrawQuad(D_Drawer* _draw2d_state, rect quad, R_Texture texture, rect uvs, vec4 color, f32 rounding)  {
    quad.x += _draw2d_state->offset.x;
    quad.y += _draw2d_state->offset.y;
    
    if (!rect_overlaps(quad, _draw2d_state->cull_quad)) return;
    
    D_Batch* batch = D_GetCurrentBatch(_draw2d_state, 6, texture);
    int idx = D_BatchAddTexture(_draw2d_state, batch, texture);
    rect uv_culled = rect_uv_cull(quad, uvs, _draw2d_state->cull_quad);
    R_Vertex vertices[6] = {
        {
            .pos = vec2_clamp(vec2_init(quad.x, quad.y), _draw2d_state->cull_quad),
            .tex_index = idx,
            .tex_coords = vec2_init(uv_culled.x, uv_culled.y),
            .color = color,
            .rounding = rounding,
            .uidims = vec2_init(quad.w, quad.h),
            .uicorner = vec2_init(0.f, 0.f),
        },
        {
            .pos = vec2_clamp(vec2_init(quad.x + quad.w, quad.y), _draw2d_state->cull_quad),
            .tex_index = idx,
            .tex_coords = vec2_init(uv_culled.x + uv_culled.w, uv_culled.y),
            .color = color,
            .rounding = rounding,
            .uidims = vec2_init(quad.w, quad.h),
            .uicorner = vec2_init(1.f, 0.f),
        },
        {
            .pos = vec2_clamp(vec2_init(quad.x + quad.w, quad.y + quad.h), _draw2d_state->cull_quad),
            .tex_index = idx,
            .tex_coords = vec2_init(uv_culled.x + uv_culled.w, uv_culled.y + uv_culled.h),
            .color = color,
            .rounding = rounding,
            .uidims = vec2_init(quad.w, quad.h),
            .uicorner = vec2_init(1.f, 1.f),
        },
        {
            .pos = vec2_clamp(vec2_init(quad.x, quad.y), _draw2d_state->cull_quad),
            .tex_index = idx,
            .tex_coords = vec2_init(uv_culled.x, uv_culled.y),
            .color = color,
            .rounding = rounding,
            .uidims = vec2_init(quad.w, quad.h),
            .uicorner = vec2_init(0.f, 0.f),
        },
        {
            .pos = vec2_clamp(vec2_init(quad.x + quad.w, quad.y + quad.h), _draw2d_state->cull_quad),
            .tex_index = idx,
            .tex_coords = vec2_init(uv_culled.x + uv_culled.w, uv_culled.y + uv_culled.h),
            .color = color,
            .rounding = rounding,
            .uidims = vec2_init(quad.w, quad.h),
            .uicorner = vec2_init(1.f, 1.f),
        },
        {
            .pos = vec2_clamp(vec2_init(quad.x, quad.y + quad.h), _draw2d_state->cull_quad),
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

dll_plugin_api void D_DrawQuadC(D_Drawer* _draw2d_state, rect quad, vec4 color, f32 rounding) {
    D_DrawQuad(_draw2d_state, quad, _draw2d_state->white, rect_init(0.f, 0.f, 1.f, 1.f), color, rounding);
}

dll_plugin_api void D_DrawQuadT(D_Drawer* _draw2d_state, rect quad, R_Texture texture, vec4 tint) {
    D_DrawQuad(_draw2d_state, quad, texture, rect_init(0.f, 0.f, 1.f, 1.f), tint, 0);
}

dll_plugin_api void D_DrawQuadST(D_Drawer* _draw2d_state, rect quad, R_Texture texture, rect uvs, vec4 tint) {
    D_DrawQuad(_draw2d_state, quad, texture, uvs, tint, 0);
}

dll_plugin_api void D_DrawStringC(D_Drawer* _draw2d_state, D_FontInfo* fontinfo, vec2 pos, string str, vec4 color) {
    for (u32 i = 0; i < str.size; i++) {
        if (str.str[i] >= 32 && str.str[i] < 128) {
            stbtt_packedchar* info = &fontinfo->cdata[str.str[i] - 32];
            rect uvs = { info->x0 / 512.f, info->y0 / 512.f, (info->x1 - info->x0) / 512.f, (info->y1 - info->y0) / 512.f };
            rect loc = { pos.x + info->xoff, pos.y + info->yoff, info->x1 - info->x0, info->y1 - info->y0 };
            D_DrawQuadST(_draw2d_state, loc, fontinfo->font_texture, uvs, color);
            pos.x += info->xadvance;
        }
    }
}

dll_plugin_api void D_DrawString(D_Drawer* _draw2d_state, D_FontInfo* fontinfo, vec2 pos, string str) {
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
            D_DrawQuadST(_draw2d_state, loc, fontinfo->font_texture, uvs, vec4_init(1.f, 1.f, 1.f, 1.f));
            pos.x += info->xadvance;
        }
    }
}

dll_plugin_api f32 D_GetStringSize(D_FontInfo* fontinfo, string str) {
    f32 sz = 0.f;
    for (u32 i = 0; i < str.size; i++) {
        if (str.str[i] >= 32 && str.str[i] < 128) {
            stbtt_packedchar* info = &fontinfo->cdata[str.str[i] - 32];
            sz += info->xadvance;
        }
    }
    return sz;
}

void D_LoadFont(D_FontInfo* fontinfo, string filename, f32 size) {
    if (fontinfo->font_texture) glDeleteTextures(1, &fontinfo->font_texture);
    
    FILE* ttfile = fopen((char*)filename.str, "rb");
    assert(ttfile != null && "Font file couldn't be opened");
    fseek(ttfile, 0, SEEK_END);
    u64 length = ftell(ttfile);
    rewind(ttfile);
    u8 buffer[length * sizeof(u8)];
    fread(buffer, length, 1, ttfile);
    fclose(ttfile);
    
    u8 temp_bitmap[512 * 512];
    
    stbtt_fontinfo finfo;
    stbtt_pack_context packctx;
    stbtt_InitFont(&finfo, buffer, 0);
    stbtt_PackBegin(&packctx, temp_bitmap, 512, 512, 0, 1, 0);
    stbtt_PackSetOversampling(&packctx, 1, 1);
    stbtt_PackFontRange(&packctx, buffer, 0, size, 32, 95, fontinfo->cdata);
    stbtt_PackEnd(&packctx);
    
    glGenTextures(1, &fontinfo->font_texture);
    glBindTexture(GL_TEXTURE_2D, fontinfo->font_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, 512, 512, 0, GL_RED, GL_UNSIGNED_BYTE, temp_bitmap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    i32 swizzles[4] = { GL_ONE, GL_ONE, GL_ONE, GL_RED };
    glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzles);
    
    fontinfo->scale = stbtt_ScaleForPixelHeight(&finfo, size);
    stbtt_GetFontVMetrics(&finfo, &fontinfo->ascent, &fontinfo->descent, nullptr);
    fontinfo->baseline = (i32) (fontinfo->ascent * fontinfo->scale);
    fontinfo->font_size = size;
}

void D_FreeFont(D_FontInfo* font) {
    glDeleteTextures(1, &font->font_texture);
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
            R_TextureBind(_draw2d_state->batches.elems[i].textures[t], t);
        
        R_VertexCacheRender(&_draw2d_state->batches.elems[i].cache);
    }
}
