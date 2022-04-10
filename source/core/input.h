/* date = April 5th 2022 6:25 pm */

#include <GLFW/glfw3.h>

#ifndef INPUT_H
#define INPUT_H

typedef struct I_InputState {
    GLFWwindow* window;
    u8 key_states[350];
    
    f32 mouse_x;
    f32 mouse_y;
    f32 mouse_scrollx;
    f32 mouse_scrolly;
    f32 mouse_absscrollx;
    f32 mouse_absscrolly;
} I_InputState;

b32 I_Init(I_InputState* _input_state, GLFWwindow* window);
void I_Reset();

b32 I_Key(i32 key);
b32 I_KeyPressed(i32 key);
b32 I_KeyReleased(i32 key);
b32 I_KeyHeld(i32 key);

#endif //INPUT_H
