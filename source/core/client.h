/* date = April 5th 2022 1:18 pm */

#ifndef CLIENT_H
#define CLIENT_H

//~ Plugin Stuff

typedef void  PluginGlobalInitProcedure();
typedef void  PluginGlobalFreeProcedure();
typedef void  PluginKeyInputProcedure(void* context, i32 key, i32 scancode, i32 action, i32 mods);

typedef void* PluginInitProcedure(A_AssetLoader* loader);
typedef void  PluginUpdateProcedure(void* context, I_InputState* input, f32 dt);
typedef void  PluginFocusedUpdateProcedure(void* context, I_InputState* input);
typedef void  PluginRenderProcedure(void* context, D_CommandBuffer* cb, rect panel_size);
typedef void  PluginFreeProcedure(void* context);

typedef struct C_Plugin {
    OS_Library lib;
    PluginGlobalInitProcedure* global_init;
    PluginGlobalFreeProcedure* global_free;
    PluginKeyInputProcedure* key_input;
    PluginInitProcedure* init;
    PluginUpdateProcedure* update;
    PluginFocusedUpdateProcedure* focused_update;
    PluginRenderProcedure* render;
    PluginFreeProcedure* free;
} C_Plugin;

//~ Panel Stuff

typedef u32 C_PanelChopDir;
enum {
    PanelChop_None,
    PanelChop_Horizontal,
    PanelChop_Vertical,
};

typedef struct C_Panel C_Panel;
struct C_Panel {
    rect bounds;
    rect target_bounds;
    C_Panel* parent;
    C_Panel* a;
    C_Panel* b;
    b32 is_leaf;
    b32 test_close_to_zero;
    b32 is_destroyed;
    C_PanelChopDir chop_dir;
    
    C_Plugin content;
    void* context;
    
    L_Lister lister;
};

C_Panel* C_PanelAlloc(M_Arena* arena, rect bounds, rect target);
void C_PanelChop(M_Arena* arena, C_Panel* parent_panel, C_PanelChopDir chop_dir);
b32  C_PanelUpdate(C_Panel* panel, I_InputState* input, A_AssetLoader* loader, f32 dt);
void C_PanelRender(D_CommandBuffer* cb, C_Panel* panel);
void C_PanelDestroy(C_Panel* panel);
void C_PanelResize(C_Panel* panel, rect new_bounds);

//~ Client Routines

Array_Prototype(panel_array, C_Panel*);
Array_Prototype(plugin_array, C_Plugin);

typedef struct C_ClientState {
    M_Arena arena;
    C_Panel* root;
    C_Panel* selected;
    i32 selected_index;
    panel_array panels;
    plugin_array plugins;
    string_array options;
    A_FontInfo* finfo;
} C_ClientState;

void C_Init(C_ClientState* cstate, A_AssetLoader* loader);
void C_Update(I_InputState* input, A_AssetLoader* loader, f32 dt);
void C_Render(D_CommandBuffer* cb);
void C_KeyCallback(i32 key, i32 scancode, i32 action, i32 mods);
void C_Shutdown();

#endif //CLIENT_H
