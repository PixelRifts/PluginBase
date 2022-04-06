#include <glad/glad.h>

#include <stb/stb_truetype.h>
#include <stdio.h>
#include <assert.h>

typedef struct D_Batch {
    R_VertexCache cache;
    R_Texture textures[8];
    u8 tex_count;
} D_Batch;

typedef struct D_FontInfo {
    R_Texture font_texture;
    stbtt_packedchar cdata[95];
    f32 scale;
    i32 ascent;
    i32 descent;
    i32 baseline;
} D_FontInfo;

typedef struct D_State {
    M_Arena arena;
    
    D_Batch batches[D_MAX_BATCHES];
    u8 current_batch;
    u8 initialized_batches;
    
    R_Texture white;
} D_State;

static D_State _draw2d_state = {0};
static D_FontInfo fontinfo = {0};

static D_Batch* D_NextBatch() {
    D_Batch* next = &_draw2d_state.batches[++_draw2d_state.current_batch];
    if (_draw2d_state.current_batch >= _draw2d_state.initialized_batches) {
        next->cache = R_VertexCacheCreate(&_draw2d_state.arena, R_MAX_INTERNAL_CACHE_VCOUNT);
        _draw2d_state.initialized_batches++;
    }
    return next;
}

static b8 D_BatchCanAddTexture(D_Batch* batch, R_Texture texture) {
    if (batch->tex_count < 8) return true;
    for (u8 i = 0; i < 8; i++) {
        if (batch->textures[i] == texture)
            return true;
    }
    return false;
}

static u8 D_BatchAddTexture(D_Batch* batch, R_Texture tex) {
    for (u8 i = 0; i < 8; i++) {
        if (batch->textures[i] == tex)
            return i;
    }
    batch->textures[batch->tex_count] = tex;
    return batch->tex_count++;
}

static D_Batch* D_GetCurrentBatch(int num_verts, R_Texture tex) {
    D_Batch* batch = &_draw2d_state.batches[_draw2d_state.current_batch];
    if (!D_BatchCanAddTexture(batch, tex) || batch->cache.count + num_verts >= batch->cache.max_verts)
        batch = D_NextBatch();
    return batch;
}

void D_Init() {
    R_InitOpenGL();
    arena_init(&_draw2d_state.arena);
    _draw2d_state.initialized_batches = 1;
    _draw2d_state.current_batch = 0;
    _draw2d_state.batches[_draw2d_state.current_batch] = (D_Batch) {0};
    _draw2d_state.batches[_draw2d_state.current_batch].cache = R_VertexCacheCreate(&_draw2d_state.arena, R_MAX_INTERNAL_CACHE_VCOUNT);
    _draw2d_state.white = R_TextureCreateWhite();
}

void D_Shutdown() {
    arena_free(&_draw2d_state.arena);
    R_ShutdownOpenGL();
}

void D_DrawQuadC(rect quad, vec4 color, f32 rounding) {
    D_Batch* batch = D_GetCurrentBatch(6, _draw2d_state.white);
    int idx = D_BatchAddTexture(batch, _draw2d_state.white);
    R_Vertex vertices[6] = {
        {
            .pos = vec2_init(quad.x, quad.y),
            .tex_index = idx,
            .tex_coords = vec2_init(0.f, 0.f),
            .color = color,
            .rounding = rounding,
            .uidims = vec2_init(quad.w, quad.h),
        },
        {
            .pos = vec2_init(quad.x + quad.w, quad.y),
            .tex_index = idx,
            .tex_coords = vec2_init(1.f, 0.f),
            .color = color,
            .rounding = rounding,
            .uidims = vec2_init(quad.w, quad.h),
        },
        {
            .pos = vec2_init(quad.x + quad.w, quad.y + quad.h),
            .tex_index = idx,
            .tex_coords = vec2_init(1.f, 1.f),
            .color = color,
            .rounding = rounding,
            .uidims = vec2_init(quad.w, quad.h),
        },
        {
            .pos = vec2_init(quad.x, quad.y),
            .tex_index = idx,
            .tex_coords = vec2_init(0.f, 0.f),
            .color = color,
            .rounding = rounding,
            .uidims = vec2_init(quad.w, quad.h),
        },
        {
            .pos = vec2_init(quad.x + quad.w, quad.y + quad.h),
            .tex_index = idx,
            .tex_coords = vec2_init(1.f, 1.f),
            .color = color,
            .rounding = rounding,
            .uidims = vec2_init(quad.w, quad.h),
        },
        {
            .pos = vec2_init(quad.x, quad.y + quad.h),
            .tex_index = idx,
            .tex_coords = vec2_init(0.f, 1.f),
            .color = color,
            .rounding = rounding,
            .uidims = vec2_init(quad.w, quad.h),
        },
    };
    R_VertexCachePush(&batch->cache, vertices, 6);
}

void D_DrawQuadT(rect quad, R_Texture texture, vec4 tint) {
    D_Batch* batch = D_GetCurrentBatch(6, texture);
    int idx = D_BatchAddTexture(batch, texture);
    R_Vertex vertices[6] = {
        {
            .pos = vec2_init(quad.x, quad.y),
            .tex_index = idx,
            .tex_coords = vec2_init(0.f, 0.f),
            .color = tint,
        },
        {
            .pos = vec2_init(quad.x + quad.w, quad.y),
            .tex_index = idx,
            .tex_coords = vec2_init(1.f, 0.f),
            .color = tint,
        },
        {
            .pos = vec2_init(quad.x + quad.w, quad.y + quad.h),
            .tex_index = idx,
            .tex_coords = vec2_init(1.f, 1.f),
            .color = tint,
        },
        {
            .pos = vec2_init(quad.x, quad.y),
            .tex_index = idx,
            .tex_coords = vec2_init(0.f, 0.f),
            .color = tint,
        },
        {
            .pos = vec2_init(quad.x + quad.w, quad.y + quad.h),
            .tex_index = idx,
            .tex_coords = vec2_init(1.f, 1.f),
            .color = tint,
        },
        {
            .pos = vec2_init(quad.x, quad.y + quad.h),
            .tex_index = idx,
            .tex_coords = vec2_init(0.f, 1.f),
            .color = tint,
        },
    };
    R_VertexCachePush(&batch->cache, vertices, 6);
}

void D_DrawQuadST(rect quad, R_Texture texture, rect uvs, vec4 tint) {
    D_Batch* batch = D_GetCurrentBatch(6, texture);
    int idx = D_BatchAddTexture(batch, texture);
    R_Vertex vertices[6] = {
        {
            .pos = vec2_init(quad.x, quad.y),
            .tex_index = idx,
            .tex_coords = vec2_init(uvs.x, uvs.y),
            .color = tint,
        },
        {
            .pos = vec2_init(quad.x + quad.w, quad.y),
            .tex_index = idx,
            .tex_coords = vec2_init(uvs.x + uvs.w, uvs.y),
            .color = tint,
        },
        {
            .pos = vec2_init(quad.x + quad.w, quad.y + quad.h),
            .tex_index = idx,
            .tex_coords = vec2_init(uvs.x + uvs.w, uvs.y + uvs.h),
            .color = tint,
        },
        {
            .pos = vec2_init(quad.x, quad.y),
            .tex_index = idx,
            .tex_coords = vec2_init(uvs.x, uvs.y),
            .color = tint,
        },
        {
            .pos = vec2_init(quad.x + quad.w, quad.y + quad.h),
            .tex_index = idx,
            .tex_coords = vec2_init(uvs.x + uvs.w, uvs.y + uvs.h),
            .color = tint,
        },
        {
            .pos = vec2_init(quad.x, quad.y + quad.h),
            .tex_index = idx,
            .tex_coords = vec2_init(uvs.x, uvs.y + uvs.h),
            .color = tint,
        },
    };
    R_VertexCachePush(&batch->cache, vertices, 6);
}


void D_DrawStringC(vec2 pos, string str, vec4 color) {
    for (u32 i = 0; i < str.size; i++) {
        if (str.str[i] >= 32 && str.str[i] < 128) {
            stbtt_packedchar* info = &fontinfo.cdata[str.str[i] - 32];
            rect uvs = { info->x0 / 512.f, info->y0 / 512.f, (info->x1 - info->x0) / 512.f, (info->y1 - info->y0) / 512.f };
            rect loc = { pos.x + info->xoff, pos.y + info->yoff, info->x1 - info->x0, info->y1 - info->y0 };
            D_DrawQuadST(loc, fontinfo.font_texture, uvs, color);
            pos.x += info->xadvance;
        }
    }
}

void D_DrawString(vec2 pos, string str) {
    for (u32 i = 0; i < str.size; i++) {
        if (str.str[i] >= 32 && str.str[i] < 128) {
            stbtt_packedchar* info = &fontinfo.cdata[str.str[i] - 32];
            rect uvs = {
                info->x0 / 512.f,
                info->y0 / 512.f,
                (info->x1 - info->x0) / 512.f,
                (info->y1 - info->y0) / 512.f
            };
            rect loc = { pos.x + info->xoff, pos.y + info->yoff, info->x1 - info->x0, info->y1 - info->y0 };
            D_DrawQuadST(loc, fontinfo.font_texture, uvs, vec4_init(1.f, 1.f, 1.f, 1.f));
            pos.x += info->xadvance;
        }
    }
}

void D_SetFont(string filename, f32 size) {
    if (fontinfo.font_texture) glDeleteTextures(1, &fontinfo.font_texture);
    
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
    stbtt_PackFontRange(&packctx, buffer, 0, size, 32, 95, fontinfo.cdata);
    stbtt_PackEnd(&packctx);
    
    glGenTextures(1, &fontinfo.font_texture);
    glBindTexture(GL_TEXTURE_2D, fontinfo.font_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, 512, 512, 0, GL_RED, GL_UNSIGNED_BYTE, temp_bitmap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    i32 swizzles[4] = { GL_ONE, GL_ONE, GL_ONE, GL_RED };
    glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzles);
    
    fontinfo.scale = stbtt_ScaleForPixelHeight(&finfo, size);
    stbtt_GetFontVMetrics(&finfo, &fontinfo.ascent, &fontinfo.descent, nullptr);
    fontinfo.baseline = (i32) (fontinfo.ascent * fontinfo.scale);
}

void D_BeginDraw() {
    R_BeginRenderOpenGL();
    for (u8 i = 0; i < _draw2d_state.initialized_batches; i++) {
        R_VertexCacheReset(&_draw2d_state.batches[i].cache);
        _draw2d_state.batches[i].tex_count = 0;
    }
}

void D_EndDraw() {
    for (u8 i = 0; i <= _draw2d_state.current_batch; i++) {
        for (u8 t = 0; t < 8; t++)
            R_TextureBind(_draw2d_state.batches[i].textures[t], t);
        
        R_VertexCacheRender(&_draw2d_state.batches[i].cache);
    }
}