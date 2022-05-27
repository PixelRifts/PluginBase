void L_ListerInit(L_Lister* lister, string_array options) {
    lister->options = options;
    lister->current_index = 0;
    lister->visible = false;
}

void L_ListerUpdate(L_Lister* lister, I_InputState* input) {
    if (!lister->visible) return;
    
    if (I_KeyPressed(input, GLFW_KEY_DOWN)) {
        lister->current_index++;
    } else if (I_KeyPressed(input, GLFW_KEY_UP)) {
        lister->current_index--;
    }
    lister->current_index = Wrap(0, lister->current_index, lister->options.len - 1);
}

void L_ListerRender(L_Lister* lister, D_Drawer* drawer, D_FontInfo* font, rect draw_area) {
    if (!lister->visible) return;
    
    D_DrawQuadC(drawer, draw_area, (vec4) { 0.8f, 0.2f, 0.3f, 1.f }, 10.f);
    
    rect last_rect = D_PushCullRect(drawer, draw_area);
    vec2 last_offset = D_PushOffset(drawer, (vec2) { draw_area.x, draw_area.y });
    
    //- Render options 
    f32 y = 10.f;
    for (u32 i = 0; i < lister->options.len - 1; i++) {
        string current = lister->options.elems[i];
        D_DrawString(drawer, font, (vec2) { 5.f, y }, current);
        y += font->scale;
    }
    
    D_PopOffset(drawer, last_offset);
    D_PopCullRect(drawer, last_rect);
}

void L_ListerFree(L_Lister* lister) {
    // NOTE(voxel): Nothing as of now
}
