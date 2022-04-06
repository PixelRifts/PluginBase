#include <stdlib.h>

C_Panel* C_PanelAlloc(M_Arena* arena, rect bounds) {
    C_Panel* panel = arena_alloc(arena, sizeof(C_Panel));
    panel->bounds = bounds;
    panel->is_leaf = true;
    return panel;
}

void C_PanelChop(M_Arena* arena, C_Panel* parent_panel, C_PanelChopDir chop_dir) {
    parent_panel->is_leaf = false;
    if (chop_dir == PanelChop_Horizontal) {
        rect a = parent_panel->bounds, b = parent_panel->bounds;
        a.h = parent_panel->bounds.h / 2.f - 0.5f;
        b = a;
        b.y = a.h + 1.f;
        C_Panel* ap = C_PanelAlloc(arena, a);
        C_Panel* bp = C_PanelAlloc(arena, b);
        ap->parent = parent_panel;
        bp->parent = parent_panel;
        parent_panel->a = ap;
        parent_panel->b = bp;
    } else {
        rect a = parent_panel->bounds, b = parent_panel->bounds;
        a.w = parent_panel->bounds.w / 2.f - 0.5f;
        b = a;
        b.x = a.x + a.w + 1.f;
        C_Panel* ap = C_PanelAlloc(arena, a);
        C_Panel* bp = C_PanelAlloc(arena, b);
        ap->parent = parent_panel;
        bp->parent = parent_panel;
        parent_panel->a = ap;
        parent_panel->b = bp;
    }
}

void C_PanelRender(C_Panel* panel) {
    if (panel->is_leaf) {
        D_DrawQuadC(panel->bounds, (vec4) { 0.2f, 0.2f, 0.2f, 1.0f }, 10);
    } else {
        C_PanelRender(panel->a);
        C_PanelRender(panel->b);
    }
}

void C_PanelResize(C_Panel* panel, rect new_bounds) {
    
}

void C_Init() {
    
}

void C_Update() {
    
}

void C_Render() {
    
}

void C_Shutdown() {
    
}
