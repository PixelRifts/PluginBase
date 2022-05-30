
Array_Impl(I_KeyCallbackProcArray, I_KeyCallbackProc*);

static void I_KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key < 0 || key >= 350) return;
    
    I_InputState* input = glfwGetWindowUserPointer(window);
    switch (action) {
        case GLFW_PRESS: {
            input->key_states[key] |= 0b00000001;
            
            input->key_held_state[key] = 1;
        } break;
        
        case GLFW_RELEASE: {
            input->key_states[key] |= 0b00000010;
            
            input->key_held_state[key] = 0;
        } break;
        
        case GLFW_REPEAT: {
            input->key_states[key] |= 0b00000100;
        } break;
    }
    
    Iterate(input->key_callbacks, i) {
        input->key_callbacks.elems[i](key, scancode, action, mods);
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

void I_Init(I_InputState* _input_state, GLFWwindow* window) {
    _input_state->window = window;
    glfwSetKeyCallback(_input_state->window, I_KeyCallback);
    glfwSetCursorPosCallback(_input_state->window, I_CursorPosCallback);
    glfwSetScrollCallback(_input_state->window, I_ScrollCallback);
}

void I_Reset(I_InputState* _input_state) {
    memset(_input_state->key_states, 0, 350 * sizeof(u8));
    _input_state->mouse_scrollx = 0;
    _input_state->mouse_scrolly = 0;
}

void I_Free(I_InputState* _input_state) {
    I_KeyCallbackProcArray_free(&_input_state->key_callbacks);
}

void I_RegisterKeyCallback(I_InputState* input, I_KeyCallbackProc* proc) {
    I_KeyCallbackProcArray_add(&input->key_callbacks, proc);
}

b32 I_Key(I_InputState* input, i32 key) {
    return input->key_held_state[key];
}

b32 I_KeyPressed(I_InputState* input, i32 key) {
    return ((input->key_states[key] & 0b00000001) >> 0) != 0;
}

b32 I_KeyReleased(I_InputState* input, i32 key) {
    return ((input->key_states[key] & 0b00000010) >> 1) != 0;
}

b32 I_KeyHeld(I_InputState* input, i32 key) {
    return ((input->key_states[key] & 0b00000100) >> 2) != 0;
}
