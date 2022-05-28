#include <stdlib.h>

Array_Impl(panel_array, C_Panel*);
Array_Impl(plugin_array, C_Plugin);

static C_ClientState* _client_state;

C_Panel* C_PanelAlloc(M_Arena* arena, rect bounds, rect target) {
    C_Panel* panel = arena_alloc_zero(arena, sizeof(C_Panel));
    panel->bounds = bounds;
    panel->target_bounds = target;
    panel->is_leaf = true;
    return panel;
}

void C_PanelChop(M_Arena* arena, C_Panel* parent_panel, C_PanelChopDir chop_dir) {
    parent_panel->is_leaf = false;
    parent_panel->chop_dir = chop_dir;
    if (chop_dir == PanelChop_Horizontal) {
        rect a = parent_panel->target_bounds, b = parent_panel->target_bounds;
        a.h = parent_panel->target_bounds.h / 2.f - 0.5f;
        b = a;
        b.y = a.y + a.h + 1.f;
        C_Panel* ap = C_PanelAlloc(arena, (rect) { a.x, a.y, b.w, 0 }, a);
        C_Panel* bp = C_PanelAlloc(arena, parent_panel->target_bounds, b);
        ap->parent = parent_panel;
        bp->parent = parent_panel;
        parent_panel->a = ap;
        parent_panel->b = bp;
        
        ap->content = parent_panel->content;
        ap->context = parent_panel->context;
        
        if (parent_panel->lister.visible) {
            L_ListerInit(&ap->lister, _client_state->options);
            ap->lister.visible = true;
        }
    } else if (chop_dir == PanelChop_Vertical) {
        rect a = parent_panel->target_bounds, b = parent_panel->target_bounds;
        a.w = parent_panel->target_bounds.w / 2.f - 0.5f;
        b = a;
        b.x = a.x + a.w + 1.f;
        C_Panel* ap = C_PanelAlloc(arena, (rect) { a.x, a.y, 0, b.h }, a);
        C_Panel* bp = C_PanelAlloc(arena, parent_panel->target_bounds, b);
        ap->parent = parent_panel;
        bp->parent = parent_panel;
        parent_panel->a = ap;
        parent_panel->b = bp;
        
        ap->content = parent_panel->content;
        ap->context = parent_panel->context;
        
        if (parent_panel->lister.visible) {
            L_ListerInit(&ap->lister, _client_state->options);
            ap->lister.visible = true;
        }
    }
}

b32 C_PanelUpdate(C_Panel* panel, I_InputState* input) {
    animate_f32exp(&panel->bounds.x, panel->target_bounds.x, 0.04f, 1.0f);
    animate_f32exp(&panel->bounds.y, panel->target_bounds.y, 0.04f, 1.0f);
    animate_f32exp(&panel->bounds.w, panel->target_bounds.w, 0.04f, 1.0f);
    animate_f32exp(&panel->bounds.h, panel->target_bounds.h, 0.04f, 1.0f);
    
    if (!panel->is_leaf) {
        if (C_PanelUpdate(panel->a, input)) return false;
        if (C_PanelUpdate(panel->b, input)) return false;
    }
    
    if (panel->content.update)
        panel->content.update(panel->context, input);
    
    if (_client_state->selected == panel) {
        if (!panel->lister.visible) {
            if (panel->content.focused_update)
                panel->content.focused_update(panel->context, input);
        }
        
        i32 pick = L_ListerUpdate(&panel->lister, input);
        if (pick != -1) {
            if (panel->content.free)
                panel->content.free(panel->context);
            panel->context = nullptr;
            
            panel->content = _client_state->plugins.elems[pick];
            if (panel->content.init)
                panel->context = panel->content.init(panel->context);
            
            L_ListerFree(&panel->lister);
            panel->lister.visible = false;
        }
    }
    
    if (panel->test_close_to_zero) {
        if (panel->bounds.w <= EPSILON || panel->bounds.h <= EPSILON) {
            C_PanelDestroy(panel);
            return true;
        }
    }
    
    return false;
}

void C_PanelRender(D_Drawer* drawer, C_Panel* panel) {
    if (panel->is_leaf) {
        if (_client_state->selected == panel) { 
            D_DrawQuadC(drawer, panel->bounds, (vec4) { 0.15f, 0.15f, 0.15f, 1.0f }, 10);
        } else D_DrawQuadC(drawer, panel->bounds, (vec4) { 0.13f, 0.13f, 0.13f, 1.0f }, 10);
        
        vec2 old = D_PushOffset(drawer, (vec2) { panel->bounds.x, panel->bounds.y });
        rect old_cull = D_PushCullRect(drawer, panel->bounds);
        if (panel->content.render) panel->content.render(panel->context, drawer);
        D_PopCullRect(drawer, old_cull);
        D_PopOffset(drawer, old);
        
        rect panel_lister_rect = {
            panel->bounds.x + (panel->bounds.w / 2.f) - (panel->bounds.w / 2.5f),
            panel->bounds.y + (panel->bounds.h / 10.0f),
            panel->bounds.w / 1.25f,
            panel->bounds.h  - (panel->bounds.h / 5.f),
        };
        
        L_ListerRender(&panel->lister, drawer, &_client_state->finfo, panel_lister_rect, _client_state->selected == panel);
        
    } else {
        C_PanelRender(drawer, panel->a);
        C_PanelRender(drawer, panel->b);
    }
}

void C_PanelDestroy(C_Panel* panel) {
    if (!panel->is_leaf) return;
    if (!panel->parent) return;
    
    C_Panel* sibling = panel->parent->a == panel ? panel->parent->b : panel->parent->a;
    C_Panel* parent_parent = panel->parent->parent;
    *panel->parent = *sibling;
    panel->parent->parent = parent_parent;
    
    if (!panel->parent->is_leaf) {
        panel->parent->a->parent = panel->parent;
        panel->parent->b->parent = panel->parent;
    }
    
    if (panel->content.free) {
        panel->content.free(panel->context);
        panel->context = nullptr;
    }
    panel->is_destroyed = true;
}

void C_PanelClose(C_Panel* panel) {
    if (!panel->is_leaf) return;
    if (!panel->parent) return;
    
    panel->test_close_to_zero = true;
    C_Panel* sibling = panel->parent->a == panel ? panel->parent->b : panel->parent->a;
    b8 is_first = panel->parent->a == panel;
    rect target_copy = panel->target_bounds;
    
    if (panel->parent->chop_dir == PanelChop_Horizontal) {
        target_copy.h = 0;
        if (!is_first) {
            target_copy.y = panel->parent->bounds.y + panel->parent->bounds.h;
        }
    } else if (panel->parent->chop_dir == PanelChop_Vertical) {
        target_copy.w = 0;
        if (!is_first) {
            target_copy.x = panel->parent->bounds.x + panel->parent->bounds.w;
        }
    }
    
    C_PanelResize(panel, target_copy);
    C_PanelResize(sibling, panel->parent->target_bounds);
}

void C_PanelResize(C_Panel* panel, rect new_bounds) {
    panel->target_bounds = new_bounds;
    
    if (!panel->is_leaf) {
        if (panel->chop_dir == PanelChop_Horizontal) {
            rect a = panel->target_bounds, b = panel->target_bounds;
            a.h = panel->target_bounds.h / 2.f - 0.5f;
            b = a;
            b.y = a.y + a.h + 1.f;
            C_PanelResize(panel->a, a);
            C_PanelResize(panel->b, b);
        } else if (panel->chop_dir == PanelChop_Vertical) {
            rect a = panel->target_bounds, b = panel->target_bounds;
            a.w = panel->target_bounds.w / 2.f - 0.5f;
            b = a;
            b.x = a.x + a.w + 1.f;
            C_PanelResize(panel->a, a);
            C_PanelResize(panel->b, b);
        }
    }
}


void C_PanelFreeAll(C_Panel* panel) {
    if (panel->is_leaf) {
        if (panel->content.free)
            panel->content.free(panel->context);
    } else {
        C_PanelFreeAll(panel->a);
        C_PanelFreeAll(panel->b);
    }
}

static b32 ValidateSiblingSurviving(C_Panel* panel) {
    if (!panel->parent) return true;
    C_Panel* sibling = panel->parent->a == panel ? panel->parent->b : panel->parent->a;
    if (sibling->test_close_to_zero) return false;
    return true;
}

static void Refill(C_Panel* panel) {
    if (panel->is_leaf) {
        if (!panel->test_close_to_zero) 
            panel_array_add(&_client_state->panels, panel);
    } else {
        Refill(panel->a);
        Refill(panel->b);
    }
}

//~ Main Stuff

void C_Init(C_ClientState* cstate) {
    _client_state = cstate;
    D_LoadFont(&_client_state->finfo, str_lit("res/Inconsolata.ttf"), 22);
    
    arena_init(&_client_state->arena);
    _client_state->root = C_PanelAlloc(&_client_state->arena, (rect) { 0.f, 0.f, 1080.f, 720.f }, (rect) { 0.f, 0.f, 1080.f, 720.f });
    
    M_Scratch scratch = scratch_get();
    OS_FileIterator iterator = OS_FileIterInit(str_lit("plugins"));
    string name; OS_FileProperties props;
    while (OS_FileIterNext(&scratch.arena, &iterator, &name, &props)) {
        u64 last_dot = str_find_last(name, str_lit("."), 0);
        string ext = { name.str + last_dot, name.size - last_dot };
        if (str_eq(ext, str_lit("dll"))) {
            OS_Library lib = OS_LibraryLoad(str_cat(&scratch.arena, str_lit("plugins/"), name));
            
            PluginInitProcedure* init_proc = (PluginInitProcedure*) OS_LibraryGetFunction(lib, "Init");
            PluginUpdateProcedure* update_proc = (PluginUpdateProcedure*) OS_LibraryGetFunction(lib, "Update");
            PluginFocusedUpdateProcedure* focused_update_proc = (PluginFocusedUpdateProcedure*) OS_LibraryGetFunction(lib, "FocusedUpdate");
            PluginRenderProcedure* render_proc = (PluginRenderProcedure*) OS_LibraryGetFunction(lib, "Render");
            PluginFreeProcedure* free_proc = (PluginFreeProcedure*) OS_LibraryGetFunction(lib, "Free");
            
            PluginGlobalInitProcedure* global_init_proc = (PluginGlobalInitProcedure*) OS_LibraryGetFunction(lib, "GlobalInit");
            PluginGlobalFreeProcedure* global_free_proc = (PluginGlobalFreeProcedure*) OS_LibraryGetFunction(lib, "GlobalFree");
            
            C_Plugin plugin = { lib, global_init_proc, global_free_proc, init_proc, update_proc, focused_update_proc, render_proc, free_proc };
            
            if (global_init_proc) global_init_proc();
            
            plugin_array_add(&_client_state->plugins, plugin);
            string name_wo_extension = str_copy(&_client_state->arena, U_RemoveExtensionFromFilename(name));
            string_array_add(&_client_state->options, name_wo_extension);
        }
    }
    OS_FileIterEnd(&iterator);
    
    scratch_return(&scratch);
}

void C_Update(I_InputState* input) {
    if (I_Key(input, GLFW_KEY_LEFT_CONTROL) || I_Key(input, GLFW_KEY_RIGHT_CONTROL)) {
        if (I_KeyPressed(input, GLFW_KEY_COMMA)) {
            if (I_Key(input, GLFW_KEY_LEFT_SHIFT) || I_Key(input, GLFW_KEY_RIGHT_SHIFT)) {
                _client_state->selected_index--;
            } else {
                _client_state->selected_index++;
            }
            
            _client_state->selected_index = Wrap(0, _client_state->selected_index, (_client_state->panels.len - 1));
        }
        
        if (I_Key(input, GLFW_KEY_LEFT_SHIFT) || I_Key(input, GLFW_KEY_RIGHT_SHIFT)) {
            if (I_KeyPressed(input, GLFW_KEY_P)) {
                if (ValidateSiblingSurviving(_client_state->selected)) {
                    C_PanelClose(_client_state->selected);
                    _client_state->selected_index--;
                    _client_state->selected_index = Clamp(0, _client_state->selected_index, (_client_state->panels.len - 1));
                }
            }
        }
        
        if (I_KeyPressed(input, GLFW_KEY_MINUS)) {
            C_PanelChop(&_client_state->arena, _client_state->selected, PanelChop_Horizontal);
        }
        
        if (I_KeyPressed(input, GLFW_KEY_EQUAL)) {
            C_PanelChop(&_client_state->arena, _client_state->selected, PanelChop_Vertical);
        }
    }
    _client_state->panels.len = 0;
    Refill(_client_state->root);
    
    _client_state->selected = _client_state->panels.elems[_client_state->selected_index];
    
    if (I_Key(input, GLFW_KEY_LEFT_ALT) || I_Key(input, GLFW_KEY_RIGHT_ALT)) {
        if (I_KeyPressed(input, GLFW_KEY_X)) {
            L_ListerInit(&_client_state->selected->lister, _client_state->options);
            _client_state->selected->lister.visible = true;
        }
    }
    if (I_KeyPressed(input, GLFW_KEY_ESCAPE)) {
        _client_state->selected->lister.visible = false;
        L_ListerFree(&_client_state->selected->lister);
    }
    
    C_PanelUpdate(_client_state->root, input);
}

void C_Render(D_Drawer* drawer) {
    C_PanelRender(drawer, _client_state->root);
}

void C_Shutdown() {
    Iterate(_client_state->plugins, i) {
        if (_client_state->plugins.elems[i].global_free)
            _client_state->plugins.elems[i].global_free();
    }
    
    panel_array_free(&_client_state->panels);
    plugin_array_free(&_client_state->plugins);
    string_array_free(&_client_state->options);
    D_FreeFont(&_client_state->finfo);
    arena_free(&_client_state->arena);
}
