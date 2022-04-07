#include <stdlib.h>

Array_Prototype(panel_array, C_Panel*);
Array_Impl(panel_array, C_Panel*);

typedef struct C_ClientState {
    M_Arena arena;
    C_Panel* root;
    C_Panel* selected;
    i32 selected_index;
    panel_array panels;
} C_ClientState;

static C_ClientState _client_state = {0};

C_Panel* C_PanelAlloc(M_Arena* arena, rect bounds) {
    C_Panel* panel = arena_alloc(arena, sizeof(C_Panel));
    panel->bounds = bounds;
    panel->is_leaf = true;
    return panel;
}

void C_PanelChop(M_Arena* arena, C_Panel* parent_panel, C_PanelChopDir chop_dir) {
    parent_panel->is_leaf = false;
    parent_panel->chop_dir = chop_dir;
    if (chop_dir == PanelChop_Horizontal) {
        rect a = parent_panel->bounds, b = parent_panel->bounds;
        a.h = parent_panel->bounds.h / 2.f - 0.5f;
        b = a;
        b.y = a.y + a.h + 1.f;
        C_Panel* ap = C_PanelAlloc(arena, a);
        C_Panel* bp = C_PanelAlloc(arena, b);
        ap->parent = parent_panel;
        bp->parent = parent_panel;
        parent_panel->a = ap;
        parent_panel->b = bp;
    } else if (chop_dir == PanelChop_Vertical) {
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
        if (_client_state.selected == panel) {
            D_DrawQuadC(panel->bounds, (vec4) { 0.3f, 0.3f, 0.3f, 1.0f }, 10);
        } else D_DrawQuadC(panel->bounds, (vec4) { 0.2f, 0.2f, 0.2f, 1.0f }, 10);
    } else {
        C_PanelRender(panel->a);
        C_PanelRender(panel->b);
    }
}

void C_PanelDestroy(C_Panel* panel) {
    if (!panel->is_leaf) return;
    if (!panel->parent) return;
    
    C_Panel* sibling = panel->parent->a == panel ? panel->parent->b : panel->parent->a;
    rect parent_bounds = panel->parent->bounds;
    C_Panel* parent_parent = panel->parent->parent;
    *panel->parent = *sibling;
    panel->parent->parent = parent_parent;
    
    C_PanelResize(panel->parent, parent_bounds);
    if (!panel->parent->is_leaf) {
        panel->parent->a->parent = panel->parent;
        panel->parent->b->parent = panel->parent;
    }
}

void C_PanelResize(C_Panel* panel, rect new_bounds) {
    panel->bounds = new_bounds;
    
    if (!panel->is_leaf) {
        if (panel->chop_dir == PanelChop_Horizontal) {
            rect a = panel->bounds, b = panel->bounds;
            a.h = panel->bounds.h / 2.f - 0.5f;
            b = a;
            b.y = a.y + a.h + 1.f;
            C_PanelResize(panel->a, a);
            C_PanelResize(panel->b, b);
        } else if (panel->chop_dir == PanelChop_Vertical) {
            rect a = panel->bounds, b = panel->bounds;
            a.w = panel->bounds.w / 2.f - 0.5f;
            b = a;
            b.x = a.x + a.w + 1.f;
            C_PanelResize(panel->a, a);
            C_PanelResize(panel->b, b);
        }
    }
}

void C_Init() {
    arena_init(&_client_state.arena);
    _client_state.root = C_PanelAlloc(&_client_state.arena, (rect) { 0.f, 0.f, 1080.f, 720.f });
}

static void Refill(C_Panel* panel) {
    if (panel->is_leaf)
        panel_array_add(&_client_state.panels, panel);
    else {
        Refill(panel->a);
        Refill(panel->b);
    }
}

void C_Update() {
    if (I_Key(GLFW_KEY_LEFT_CONTROL) || I_Key(GLFW_KEY_RIGHT_CONTROL)) {
        if (I_KeyPressed(GLFW_KEY_COMMA)) {
            if (I_Key(GLFW_KEY_LEFT_SHIFT) || I_Key(GLFW_KEY_RIGHT_SHIFT)) {
                _client_state.selected_index--;
            } else {
                _client_state.selected_index++;
            }
            
            _client_state.selected_index = Wrap(0, _client_state.selected_index, (_client_state.panels.len - 1));
        }
        
        if (I_Key(GLFW_KEY_LEFT_SHIFT) || I_Key(GLFW_KEY_RIGHT_SHIFT)) {
            if (I_KeyPressed(GLFW_KEY_P)) {
                C_PanelDestroy(_client_state.selected);
                _client_state.selected_index--;
                _client_state.selected_index = Clamp(0, _client_state.selected_index, (_client_state.panels.len - 1));
            }
        }
        
        if (I_KeyPressed(GLFW_KEY_MINUS)) {
            C_PanelChop(&_client_state.arena, _client_state.selected, PanelChop_Horizontal);
        }
        
        if (I_KeyPressed(GLFW_KEY_EQUAL)) {
            C_PanelChop(&_client_state.arena, _client_state.selected, PanelChop_Vertical);
        }
        
        
    }
    _client_state.panels.len = 0;
    Refill(_client_state.root);
    
    _client_state.selected = _client_state.panels.elems[_client_state.selected_index];
}

void C_Render() {
    C_PanelRender(_client_state.root);
}

void C_Shutdown() {
    panel_array_free(&_client_state.panels);
    arena_free(&_client_state.arena);
}
