/* date = May 27th 2022 8:10 pm */

#ifndef LISTER_H
#define LISTER_H

typedef struct L_Lister {
    string_array options;
    u32 current_index;
    b8  visible;
} L_Lister;

void L_ListerInit(L_Lister* lister, string_array options);
void L_ListerUpdate(L_Lister* lister, I_InputState* input);
void L_ListerRender(L_Lister* lister, D_Drawer* drawer, D_FontInfo* font, rect draw_area);
void L_ListerFree(L_Lister* lister);

#endif //LISTER_H
