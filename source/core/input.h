/* date = April 5th 2022 6:25 pm */

#ifdef PLUGIN
#define GLFW_DLL
#endif
#include <GLFW/glfw3.h>

#ifndef INPUT_H
#define INPUT_H

typedef void I_KeyCallbackProc(i32 key, i32 scancode, i32 action, i32 mods);

Array_Prototype(I_KeyCallbackProcArray, I_KeyCallbackProc*);

typedef struct I_InputState {
    GLFWwindow* window;
    u8 key_states[350];
    u8 key_held_state[350];
    
    f32 mouse_x;
    f32 mouse_y;
    f32 mouse_scrollx;
    f32 mouse_scrolly;
    f32 mouse_absscrollx;
    f32 mouse_absscrolly;
    
    I_KeyCallbackProcArray key_callbacks;
} I_InputState;

void I_Init(I_InputState* _input_state, GLFWwindow* window);
void I_Reset(I_InputState* input);
void I_Free(I_InputState* _input_state);

void I_RegisterKeyCallback(I_InputState* input, I_KeyCallbackProc* proc);

b32 I_Key(I_InputState* input, i32 key);
b32 I_KeyPressed(I_InputState* input, i32 key);
b32 I_KeyReleased(I_InputState* input, i32 key);
b32 I_KeyHeld(I_InputState* input, i32 key);

#endif //INPUT_H
