
inline static void I_SetPressedBit(I_InputState* input, u32 index) {
    input->key_states[index] |= 0b00000001;
}
inline static void I_SetReleasedBit(I_InputState* input, u32 index) {
    input->key_states[index] |= 0b00000010;
}
inline static void I_SetHeldBit(I_InputState* input, u32 index) {
    input->key_states[index] |= 0b00000100;
}

inline static void I_ResetPressedBit(I_InputState* input, u32 index) {
    input->key_states[index] &= 0b11111110;
}
inline static void I_ResetReleasedBit(I_InputState* input, u32 index) {
    input->key_states[index] &= 0b11111101;
}
inline static void I_ResetHeldBit(I_InputState* input, u32 index) {
    input->key_states[index] &= 0b11111011;
}

static void I_KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key < 0 || key >= 350) return;
    
    I_InputState* input = glfwGetWindowUserPointer(window);
    switch (action) {
        case GLFW_PRESS: {
            I_SetPressedBit(input, key);
            I_ResetReleasedBit(input, key);
            I_ResetHeldBit(input, key);
            
            input->key_held_state[key] = 1;
        } break;
        
        case GLFW_RELEASE: {
            I_ResetPressedBit(input, key);
            I_SetReleasedBit(input, key);
            I_ResetHeldBit(input, key);
            
            input->key_held_state[key] = 0;
        } break;
        
        case GLFW_REPEAT: {
            I_ResetPressedBit(input, key);
            I_ResetReleasedBit(input, key);
            I_SetHeldBit(input, key);
        } break;
    }
}

void I_CursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
    I_InputState* input = glfwGetWindowUserPointer(window);
    input->mouse_x = (f32)xpos;
    input->mouse_y = (f32)ypos;
}

void I_ScrollCallback(GLFWwindow* window, double xscroll, double yscroll) {
    I_InputState* input = glfwGetWindowUserPointer(window);
    input->mouse_scrollx = (f32)xscroll;
    input->mouse_scrolly = (f32)yscroll;
    input->mouse_absscrollx += (f32)xscroll;
    input->mouse_absscrolly += (f32)yscroll;
}

b32 I_Init(I_InputState* _input_state, GLFWwindow* window) {
    _input_state->window = window;
    glfwSetKeyCallback(_input_state->window, I_KeyCallback);
    glfwSetCursorPosCallback(_input_state->window, I_CursorPosCallback);
    glfwSetScrollCallback(_input_state->window, I_ScrollCallback);
    return true;
}

void I_Reset(I_InputState* _input_state) {
    memset(_input_state->key_states, 0, 350 * sizeof(u8));
    _input_state->mouse_scrollx = 0;
    _input_state->mouse_scrolly = 0;
}

b32 I_Key(I_InputState* input, i32 key) { return input->key_held_state[key]; }
b32 I_KeyPressed(I_InputState* input, i32 key) { return ((input->key_states[key] & 0b00000001) >> 0) != 0; }
b32 I_KeyReleased(I_InputState* input, i32 key) { return ((input->key_states[key] & 0b00000010) >> 1) != 0; }
b32 I_KeyHeld(I_InputState* input, i32 key) { return ((input->key_states[key] & 0b00000100) >> 2) != 0; }
