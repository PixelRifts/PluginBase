/* date = April 5th 2022 1:18 pm */

#ifndef CLIENT_H
#define CLIENT_H

//~ Plugin Stuff

typedef void PluginGlobalInitProcedure();
typedef void PluginGlobalFreeProcedure();
typedef void* PluginInitProcedure();
typedef void PluginUpdateProcedure(void* context, I_InputState* input);
typedef void PluginFocusedUpdateProcedure(void* context, I_InputState* input);
typedef void PluginRenderProcedure(void* context, D_Drawer* drawer);
typedef void PluginFreeProcedure(void* context);

typedef struct C_Plugin {
    OS_Library lib;
    PluginGlobalInitProcedure* global_init;
    PluginGlobalFreeProcedure* global_free;
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
void C_PanelRender(D_Drawer* drawer, C_Panel* panel);
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
    D_FontInfo finfo;
} C_ClientState;

void C_Init(C_ClientState* cstate);
void C_Update(I_InputState* input);
void C_Render(D_Drawer* drawer);
void C_Shutdown();

#endif //CLIENT_H
