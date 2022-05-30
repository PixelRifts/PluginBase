#include "plugins/plugin.h"

typedef struct fexp_context {
    D_FontInfo font;
    M_Arena arena;
    i32 selected_index;
    u32 latest_count;
    string current_filepath;
    string current_query;
    i32 current_query_idx;
} fexp_context;

dll_export void* Init() {
    fexp_context* ctx = malloc(sizeof(fexp_context));
    memset(ctx, 0, sizeof(fexp_context));
    arena_init(&ctx->arena);
    ctx->current_filepath = OS_Filepath(&ctx->arena, SystemPath_CurrentDir);
    ctx->current_query = str_alloc(&ctx->arena, PATH_MAX);
    ctx->current_query_idx = 0;
    D_LoadFont(&ctx->font, str_lit("res/Inconsolata.ttf"), 22);
    return ctx;
}

dll_export void FocusedUpdate(void* context, I_InputState* input) {
    fexp_context* ctx = (fexp_context*) context;
    
    M_Scratch scratch = scratch_get();
    string fixed = str_cat(&scratch.arena, ctx->current_filepath, str_lit("/*"));
    string fixed_query = { .str = ctx->current_query.str, .size = ctx->current_query_idx };
    
    if (I_KeyPressed(input, GLFW_KEY_DOWN) || I_KeyHeld(input, GLFW_KEY_DOWN)) {
        ctx->selected_index++;
    } else if (I_KeyPressed(input, GLFW_KEY_UP) || I_KeyHeld(input, GLFW_KEY_UP)) {
        ctx->selected_index--;
    }
    
    ctx->selected_index = Wrap(0, ctx->selected_index, ctx->latest_count - 1);
    
    b8 folder_changed = false;
    
    OS_FileIterator iter = OS_FileIterInitPattern(fixed);
    string name; OS_FileProperties props;
    u32 k = 0;
    while (OS_FileIterNext(&scratch.arena, &iter, &name, &props)) {
        if (str_find_first(name, fixed_query, 0) != name.size) {
            
            if (k == ctx->selected_index) {
                if (I_KeyPressed(input, GLFW_KEY_ENTER)) {
                    // Open File/Folder
                    if (props.flags & FileProperty_IsFolder) {
                        ctx->current_filepath = str_cat(&scratch.arena, ctx->current_filepath, str_lit("/"));
                        ctx->current_filepath = str_cat(&scratch.arena, ctx->current_filepath, name);
                        ctx->current_filepath = U_FixFilepath(&ctx->arena, ctx->current_filepath);
                    } else
                        OS_FileOpen(name);
                    
                    folder_changed = true;
                    break;
                }
            }
            k++;
            
        }
    }
    OS_FileIterEnd(&iter);
    
    if ((I_Key(input, GLFW_KEY_LEFT_CONTROL) || I_Key(input, GLFW_KEY_RIGHT_CONTROL)) && I_KeyPressed(input, GLFW_KEY_BACKSPACE)) {
        ctx->current_filepath = str_cat(&scratch.arena, ctx->current_filepath, str_lit("/"));
        ctx->current_filepath = str_cat(&scratch.arena, ctx->current_filepath, str_lit(".."));
        ctx->current_filepath = U_FixFilepath(&ctx->arena, ctx->current_filepath);
        folder_changed = true;
    }
    
    if (folder_changed) {
        ctx->selected_index = 0;
        ctx->current_query_idx = 0;
    }
    
    scratch_return(&scratch);
}

dll_export void Update(void* context, I_InputState* input) {
    fexp_context* ctx = (fexp_context*) context;
    
    ctx->latest_count = 0;
    
    M_Scratch scratch = scratch_get();
    string fixed = str_cat(&scratch.arena, ctx->current_filepath, str_lit("/*"));
    string fixed_query = { .str = ctx->current_query.str, .size = ctx->current_query_idx };
    
    OS_FileIterator iter = OS_FileIterInitPattern(fixed);
    string name; OS_FileProperties props;
    while (OS_FileIterNext(&scratch.arena, &iter, &name, &props)) {
        if (str_find_first(name, fixed_query, 0) != name.size) {
            ctx->latest_count++;
        }
    }
    OS_FileIterEnd(&iter);
    
    scratch_return(&scratch);
}

dll_export void Render(void* context, D_Drawer* drawer, rect panel_size) {
    fexp_context* ctx = (fexp_context*) context;
    
    M_Scratch scratch = scratch_get();
    string fixed_to_render = str_cat(&scratch.arena, ctx->current_filepath, str_lit("/"));
    string fixed = str_cat(&scratch.arena, ctx->current_filepath, str_lit("/*"));
    string fixed_query = { .str = ctx->current_query.str, .size = ctx->current_query_idx };
    string fixed_full_query = str_cat(&scratch.arena, fixed_to_render, fixed_query);
    
    f32 y = ctx->font.font_size * 2.5;
    D_DrawStringC(drawer, &ctx->font, (vec2) { 16, ctx->font.font_size * 1.15f }, fixed_full_query, (vec4) { .3f, .4f, .8f, 1.f });
    D_DrawQuadC(drawer, (rect) { 0, ctx->font.font_size * 1.55f, panel_size.w, 1.f }, (vec4) { .8f, .4, .3f, 2.f }, 1.f);
    
    OS_FileIterator iter = OS_FileIterInitPattern(fixed);
    u32 idx = 0;
    string name; OS_FileProperties props;
    
    while (OS_FileIterNext(&scratch.arena, &iter, &name, &props)) {
        if (str_find_first(name, fixed_query, 0) != name.size) {
            f32 size = D_GetStringSize(&ctx->font, name);
            if (idx == ctx->selected_index) {
                D_DrawQuadC(drawer, (rect) { 10, y - 16, 8 + size, 22 }, (vec4) { .3f, .3f, .3f, 1.f },
                            4.f);
            }
            D_DrawString(drawer, &ctx->font, (vec2) { 14, y }, name);
            y += 24;
            idx++;
        }
    }
    
    OS_FileIterEnd(&iter);
    
    scratch_return(&scratch);
}

dll_export void KeyInput(void* context, i32 key, i32 scancode, i32 action, i32 mods) {
    fexp_context* ctx = (fexp_context*) context;
    
    if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_X && mods & GLFW_MOD_ALT) return;
        
        if ((key >= GLFW_KEY_A && key <= GLFW_KEY_Z)) {
            u8 c = 65;
            if (mods & GLFW_MOD_SHIFT) {
                c = (u8) key;
            } else
                c = (u8) key + 32;
            
            ctx->current_query.str[ctx->current_query_idx++] = c;
        } else if ((
                    key == GLFW_KEY_PERIOD ||
                    key == GLFW_KEY_MINUS ||
                    key == GLFW_KEY_APOSTROPHE
                    ) && !(mods & GLFW_MOD_SHIFT)) {
            u8 c = key;
            ctx->current_query.str[ctx->current_query_idx++] = c;
        } else if ((key == GLFW_KEY_MINUS) && (mods & GLFW_MOD_SHIFT)) {
            u8 c = 95; // ascii _
            ctx->current_query.str[ctx->current_query_idx++] = c;
        } else if (key == GLFW_KEY_BACKSPACE && !(mods & GLFW_MOD_CONTROL)) {
            ctx->current_query_idx--;
            ctx->current_query_idx = Max(0, ctx->current_query_idx);
        }
    }
}

dll_export void Free(void* context) {
    fexp_context* ctx = (fexp_context*) context;
    D_FreeFont(&ctx->font);
    arena_free(&ctx->arena);
    free(context);
}
