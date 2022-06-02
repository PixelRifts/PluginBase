b8 A_AssetRequest_IsNull(A_AssetRequest req) { return req.type == AssetType_None; }
b8 A_AssetRequest_IsTombstone(A_AssetRequest req) { return req.type == AssetType_MaxCount; }
HashTable_Impl(A_AssetRequest, str_is_null, str_eq, str_hash, ((A_AssetRequest) { .type = AssetType_MaxCount }), A_AssetRequest_IsNull, A_AssetRequest_IsTombstone);

//~ Assets

static A_Texture A_TextureLoad(const char* filename) {
    A_Texture texture;
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

static A_Texture A_TextureCreateWhite(void) {
    A_Texture texture;
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

static void A_LoadFont(A_FontInfo* fontinfo, string filename, f32 size) {
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


//~ Asset Loader

void A_LoaderInit(A_AssetLoader* loader) {
    arena_init(&loader->arena);
    A_AssetRequest_hash_table_init(&loader->asset_cache);
}

void A_LoaderFree(A_AssetLoader* loader) {
    IterateCap(loader->asset_cache, i) {
        A_AssetRequest_hash_table_entry* curr = &loader->asset_cache.elems[i];
        if (curr->key.size == 0) continue;
        
        if (curr->value.fulfilled) {
            switch (curr->value.type) {
                case AssetType_Texture: {
                    glDeleteTextures(1, (u32*)curr->value.data);
                } break;
                
                case AssetType_TextureWhite: {
                    glDeleteTextures(1, (u32*)curr->value.data);
                } break;
                
                case AssetType_Font: {
                    glDeleteTextures(1, &((A_FontInfo*)curr->value.data)->font_texture);
                } break;
            }
        }
    }
    
    A_AssetRequest_hash_table_free(&loader->asset_cache);
    arena_free(&loader->arena);
}

void A_RequestLoadTexture(A_AssetLoader* loader, string path, A_Texture** texture) {
    A_AssetRequest request;
    if (A_AssetRequest_hash_table_get(&loader->asset_cache, path, &request)) {
        *texture = ((A_Texture*)request.data);
        return;
    }
    
    request = (A_AssetRequest) {
        .type = AssetType_Texture,
        .fulfilled = false,
        .data = arena_alloc(&loader->arena, sizeof(A_Texture)),
    };
    *texture = ((A_Texture*)request.data);
    A_AssetRequest_hash_table_set(&loader->asset_cache, path, request);
}

void A_RequestLoadWhiteTexture(A_AssetLoader* loader, A_Texture** texture) {
    A_AssetRequest request;
    if (A_AssetRequest_hash_table_get(&loader->asset_cache, str_lit("__builtin_white"), &request)) {
        *texture = ((A_Texture*)request.data);
        return;
    }
    
    request = (A_AssetRequest) {
        .type = AssetType_TextureWhite,
        .fulfilled = false,
        .data = arena_alloc(&loader->arena, sizeof(A_Texture)),
    };
    *texture = ((A_Texture*)request.data);
    A_AssetRequest_hash_table_set(&loader->asset_cache, str_lit("__builtin_white"), request);
}

void A_RequestLoadFont(A_AssetLoader* loader, string path, u32 font_size, A_FontInfo** font) {
    A_AssetRequest request;
    if (A_AssetRequest_hash_table_get(&loader->asset_cache, path, &request)) {
        *font = ((A_FontInfo*)request.data);
        return;
    }
    
    request = (A_AssetRequest) {
        .type = AssetType_Font,
        .fulfilled = false,
        .font_size = font_size,
        .data = arena_alloc(&loader->arena, sizeof(A_FontInfo)),
    };
    *font = ((A_FontInfo*)request.data);
    A_AssetRequest_hash_table_set(&loader->asset_cache, path, request);
}

void A_LoadAllFontsAndTextures(A_AssetLoader* loader) {
    IterateCap(loader->asset_cache, i) {
        A_AssetRequest_hash_table_entry* curr = &loader->asset_cache.elems[i];
        if (curr->key.size == 0) continue;
        
        if (!curr->value.fulfilled) {
            switch (curr->value.type) {
                case AssetType_Texture: {
                    A_Texture t = A_TextureLoad((const char*)curr->key.str);
                    *(A_Texture*)curr->value.data = t;
                } break;
                
                case AssetType_TextureWhite: {
                    A_Texture t = A_TextureCreateWhite();
                    *(A_Texture*)curr->value.data = t;
                } break;
                
                case AssetType_Font: {
                    A_LoadFont((A_FontInfo*)curr->value.data, curr->key, curr->value.font_size);
                } break;
            }
            
            curr->value.fulfilled = true;
        }
    }
}
