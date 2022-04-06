
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

static I_InputState _input_state;

inline static void I_SetPressedBit(u32 index)    { _input_state.key_states[index] |= 0b00000001; }
inline static void I_SetReleasedBit(u32 index)   { _input_state.key_states[index] |= 0b00000010; }
inline static void I_SetHeldBit(u32 index)       { _input_state.key_states[index] |= 0b00000100; }

inline static void I_ResetPressedBit(u32 index)  { _input_state.key_states[index] &= 0b11111110; }
inline static void I_ResetReleasedBit(u32 index) { _input_state.key_states[index] &= 0b11111101; }
inline static void I_ResetHeldBit(u32 index)     { _input_state.key_states[index] &= 0b11111011; }

static void I_KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key < 0 || key >= 350) return;
    
    switch (action) {
        case GLFW_PRESS: {
            I_SetPressedBit(key);
            I_ResetReleasedBit(key);
            I_ResetHeldBit(key);
        } break;
        
        case GLFW_RELEASE: {
            I_ResetPressedBit(key);
            I_SetReleasedBit(key);
            I_ResetHeldBit(key);
        } break;
        
        case GLFW_REPEAT: {
            I_ResetPressedBit(key);
            I_ResetReleasedBit(key);
            I_SetHeldBit(key);
        } break;
    }
}

void I_CursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
    _input_state.mouse_x = (f32)xpos;
    _input_state.mouse_y = (f32)ypos;
}

void I_ScrollCallback(GLFWwindow* window, double xscroll, double yscroll) {
    _input_state.mouse_scrollx = (f32)xscroll;
    _input_state.mouse_scrolly = (f32)yscroll;
    _input_state.mouse_absscrollx += (f32)xscroll;
    _input_state.mouse_absscrolly += (f32)yscroll;
}

b32 I_Init(GLFWwindow* window) {
    _input_state.window = window;
    glfwSetKeyCallback(_input_state.window, I_KeyCallback);
    glfwSetCursorPosCallback(_input_state.window, I_CursorPosCallback);
    glfwSetScrollCallback(_input_state.window, I_ScrollCallback);
    return true;
}

void I_Reset() {
    memset(_input_state.key_states, 0, 350 * sizeof(u8));
    _input_state.mouse_scrollx = 0;
    _input_state.mouse_scrolly = 0;
}

b32 I_Key(i32 key) { return glfwGetKey(_input_state.window, key) != GLFW_RELEASE; }
b32 I_KeyPressed(i32 key) { return ((_input_state.key_states[key] & 0b00000001) >> 0) != 0; }
b32 I_KeyReleased(i32 key) { return ((_input_state.key_states[key] & 0b00000010) >> 1) != 0; }
b32 I_KeyHeld(i32 key) { return ((_input_state.key_states[key] & 0b00000100) >> 2) != 0; }

f32 I_GetMouseScroll() { return _input_state.mouse_scrolly; }
f32 I_GetAbsMouseScroll() { return _input_state.mouse_absscrolly; }

f32 I_GetMouseXScroll() { return _input_state.mouse_scrollx; }
f32 I_GetAbsMouseXScroll() { return _input_state.mouse_absscrollx; }

vec2 I_GetMousePos() { return (vec2) { _input_state.mouse_x, _input_state.mouse_y }; }
f32  I_GetMouseX() { return _input_state.mouse_x; }
f32  I_GetMouseY() { return _input_state.mouse_y; }
