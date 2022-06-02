void L_ListerInit(L_Lister* lister, string_array options) {
    lister->options = options;
    lister->current_index = 0;
    lister->visible = false;
}

i32 L_ListerUpdate(L_Lister* lister, I_InputState* input) {
    if (!lister->visible) return -1;
    
    if (I_KeyPressed(input, GLFW_KEY_DOWN) || I_KeyHeld(input, GLFW_KEY_DOWN)) {
        lister->current_index++;
    } else if (I_KeyPressed(input, GLFW_KEY_UP) || I_KeyHeld(input, GLFW_KEY_UP)) {
        lister->current_index--;
    }
    
    lister->current_index = Wrap(0, lister->current_index, lister->options.len - 1);
    
    if (I_KeyPressed(input, GLFW_KEY_ENTER)) {
        return lister->current_index;
    }
    return -1;
}

void L_ListerRender(L_Lister* lister, D_CommandBuffer* cb, A_FontInfo* font, rect draw_area, b8 is_on_focused_panel) {
    if (!lister->visible) return;
    
    vec4 color = {0};
    if (is_on_focused_panel) { 
        color = (vec4) { 0.15f, 0.15f, 0.15f, 1.0f };
    } else color = (vec4) { 0.13f, 0.13f, 0.13f, 1.0f };
    D_DrawQuadC(cb, draw_area, color, 3.f);
    
    {
        static const vec4 border_color = (vec4) { 1.f, 0.58f, 0.16f, 1.f };
        static const f32 half_border_thickness = 1.5f;
        
        D_DrawQuadC(cb, (rect) { draw_area.x - half_border_thickness, draw_area.y - half_border_thickness, half_border_thickness * 2, draw_area.h + half_border_thickness * 2 }, border_color, 0.2f);
        D_DrawQuadC(cb, (rect) { draw_area.x + draw_area.w - half_border_thickness, draw_area.y - half_border_thickness, half_border_thickness * 2, draw_area.h + half_border_thickness * 2 }, border_color, 0.2f);
        D_DrawQuadC(cb, (rect) { draw_area.x - half_border_thickness, draw_area.y - half_border_thickness, draw_area.w + half_border_thickness * 2, half_border_thickness * 2 }, border_color, 0.2f);
        D_DrawQuadC(cb, (rect) { draw_area.x - half_border_thickness, draw_area.y + draw_area.h - half_border_thickness, draw_area.w + half_border_thickness * 2, half_border_thickness * 2 }, border_color, 0.2f);
    }
    
    rect last_rect = D_PushCullRect(cb, draw_area);
    vec2 last_offset = D_PushOffset(cb, (vec2) { draw_area.x, draw_area.y });
    
    //- Render options 
    f32 y = 15.f + font->font_size;
    Iterate(lister->options, i) {
        string current = lister->options.elems[i];
        f32 current_string_size = D_GetStringSize(font, current);
        if (i == lister->current_index) {
            D_DrawQuadC(cb, (rect) { 14.f, y - font->font_size + 3.f, current_string_size + 8.f, font->font_size + 2.f }, (vec4) { .3f, .3f, .3f, 1.f }, 4.f);
        }
        D_DrawString(cb, font, (vec2) { 18.f, y }, current);
        y += font->font_size;
    }
    
    D_PopOffset(cb, last_offset);
    D_PopCullRect(cb, last_rect);
}

void L_ListerFree(L_Lister* lister) {
    // NOTE(voxel): Nothing as of now
}
