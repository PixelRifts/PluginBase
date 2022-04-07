/* date = April 5th 2022 1:18 pm */

#ifndef CLIENT_H
#define CLIENT_H

//~ Plugin Stuff

typedef void PluginInitProcedure();
typedef void PluginUpdateProcedure();
typedef void PluginRenderProcedure();
typedef void PluginFreeProcedure();

typedef struct C_Plugin {
    OS_Library lib;
    PluginInitProcedure* init;
    PluginUpdateProcedure* update;
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
    C_Panel* parent;
    C_Panel* a;
    C_Panel* b;
    b32 is_leaf;
    C_PanelChopDir chop_dir;
};

C_Panel* C_PanelAlloc(M_Arena* arena, rect bounds);
void C_PanelChop(M_Arena* arena, C_Panel* parent_panel, C_PanelChopDir chop_dir);
void C_PanelRender(C_Panel* panel);
void C_PanelDestroy(C_Panel* panel);
void C_PanelResize(C_Panel* panel, rect new_bounds);

//~ Client Routines

void C_Init();
void C_Update();
void C_Render();
void C_Shutdown();

#endif //CLIENT_H
