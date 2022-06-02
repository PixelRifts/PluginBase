/* date = June 2nd 2022 2:33 pm */

#ifndef ASSET_H
#define ASSET_H

#include <stb/stb_truetype.h>
#include <stb/stb_image.h>

typedef u32 A_AssetType;
enum {
    AssetType_None,
    
    AssetType_Texture,
    AssetType_TextureWhite,
    AssetType_Font,
    
    AssetType_MaxCount,
};

typedef u32 A_Texture;

typedef struct A_FontInfo {
    A_Texture font_texture;
    stbtt_packedchar cdata[95];
    f32 scale;
    f32 font_size;
    i32 ascent;
    i32 descent;
    i32 baseline;
} A_FontInfo;

typedef struct A_AssetRequest {
    A_AssetType type;
    b8 fulfilled;
    void* data;
    
    union {
        u32 font_size;
    };
} A_AssetRequest;

HashTable_Prototype(A_AssetRequest, string, A_AssetRequest);

typedef struct A_AssetLoader {
    M_Arena arena;
    A_AssetRequest_hash_table asset_cache;
} A_AssetLoader;

void A_LoaderInit(A_AssetLoader* loader);
void A_LoaderFree(A_AssetLoader* loader);

void A_RequestLoadTexture(A_AssetLoader* loader, string path, A_Texture** texture);
void A_RequestLoadWhiteTexture(A_AssetLoader* loader, A_Texture** texture);
void A_RequestLoadFont(A_AssetLoader* loader, string path, u32 font_size, A_FontInfo** font);
void A_LoadAllFontsAndTextures(A_AssetLoader* loader);

#endif //ASSET_H
