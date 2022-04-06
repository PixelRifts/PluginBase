/* date = April 5th 2022 6:25 pm */

#include <GLFW/glfw3.h>

#ifndef INPUT_H
#define INPUT_H

b32 I_Init(GLFWwindow* window);
void I_Reset();

b32 I_Key(i32 key);
b32 I_KeyPressed(i32 key);
b32 I_KeyReleased(i32 key);
b32 I_KeyHeld(i32 key);

f32 I_GetMouseScroll();
f32 I_GetAbsMouseScroll();

f32 I_GetMouseXScroll();
f32 I_GetAbsMouseXScroll();

vec2 I_GetMousePos();
f32  I_GetMouseX();
f32  I_GetMouseY();


#endif //INPUT_H
