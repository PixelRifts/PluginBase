#include "defines.h"
#include "base/mem.h"
#include "core/core.h"
#include "core/core.c"

#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>

#include "plugins/plugin.h"

typedef struct fexp_context {
    D_FontInfo font;
    M_Arena arena;
    i32 selected_index;
    u32 latest_count;
    string current_filepath;
} fexp_context;

dll_export void* Init() {
    gladLoadGL();
    fexp_context* ctx = malloc(sizeof(fexp_context));
    memset(ctx, 0, sizeof(fexp_context));
    arena_init(&ctx->arena);
    ctx->current_filepath = U_GetFullFilepath(&ctx->arena, str_lit("."));
    D_LoadFont(&ctx->font, str_lit("res/Inconsolata.ttf"), 22);
    return ctx;
}

dll_export void FocusedUpdate(void* context, I_InputState* input) {
    fexp_context* ctx = (fexp_context*) context;
    
    M_Scratch scratch = scratch_get();
    
    if (I_KeyPressed(input, GLFW_KEY_DOWN)) {
        ctx->selected_index++;
    } else if (I_KeyPressed(input, GLFW_KEY_UP)) {
        ctx->selected_index--;
    }
    
    ctx->selected_index = Wrap(0, ctx->selected_index, ctx->latest_count - 1);
    
    OS_FileIterator iter = OS_FileIterInit(ctx->current_filepath);
    string name; OS_FileProperties props;
    u32 k = 0;
    while (OS_FileIterNext(&scratch.arena, &iter, &name, &props)) {
        if (k == ctx->selected_index) {
            if (I_KeyPressed(input, GLFW_KEY_ENTER)) {
                // Open File/Folder
                if (props.flags & FileProperty_IsFolder) {
                    ctx->current_filepath = str_cat(&scratch.arena, ctx->current_filepath, str_lit("/"));
                    ctx->current_filepath = str_cat(&scratch.arena, ctx->current_filepath, name);
                    ctx->current_filepath = U_FixFilepath(&ctx->arena, ctx->current_filepath);
                } else
                    OS_FileOpen(name);
                
                break;
            }
        }
        k++;
    }
    OS_FileIterEnd(&iter);
    
    if (I_KeyPressed(input, GLFW_KEY_BACKSPACE)) {
        ctx->current_filepath = str_cat(&scratch.arena, ctx->current_filepath, str_lit("/"));
        ctx->current_filepath = str_cat(&scratch.arena, ctx->current_filepath, str_lit(".."));
        ctx->current_filepath = U_FixFilepath(&ctx->arena, ctx->current_filepath);
    }
    
    scratch_return(&scratch);
}

dll_export void Update(void* context, I_InputState* input) {
    fexp_context* ctx = (fexp_context*) context;
    
    ctx->latest_count = 0;
    
    M_Scratch scratch = scratch_get();
    
    OS_FileIterator iter = OS_FileIterInit(ctx->current_filepath);
    string name; OS_FileProperties props;
    while (OS_FileIterNext(&scratch.arena, &iter, &name, &props)) {
        ctx->latest_count++;
    }
    OS_FileIterEnd(&iter);
    
    scratch_return(&scratch);
}

dll_export void Render(void* context, D_Drawer* drawer) {
    fexp_context* ctx = (fexp_context*) context;
    
    f32 y = 36;
    M_Scratch scratch = scratch_get();
    
    OS_FileIterator iter = OS_FileIterInit(ctx->current_filepath);
    u32 idx = 0;
    string name; OS_FileProperties props;
    while (OS_FileIterNext(&scratch.arena, &iter, &name, &props)) {
        f32 size = D_GetStringSize(&ctx->font, name);
        if (idx == ctx->selected_index) {
            D_DrawQuadC(drawer, (rect) { 4, y - 16, 8 + size, 22 }, (vec4) { .5f, .5f, .5f, 1.f },
                        4.f);
        }
        D_DrawString(drawer, &ctx->font, (vec2) { 8, y }, name);
        y += 24;
        idx++;
    }
    OS_FileIterEnd(&iter);
    scratch_return(&scratch);
}

dll_export void Free(void* context) {
    fexp_context* ctx = (fexp_context*) context;
    D_FreeFont(&ctx->font);
    arena_free(&ctx->arena);
    free(context);
}
