#include <glad/glad.h>
#include "defines.h"
#include <GLFW/glfw3.h>
#include <stdio.h>

#include "os/os.h"
#include "core/core.h"
#include "base/ds.h"

C_ClientState _client_state;
D_Drawer drawer;

void resize_callback(GLFWwindow* window, int w, int h) {
    glViewport(0, 0, w, h);
    C_PanelResize(_client_state.root, (rect) { 0.f, 0.f, (f32) w, (f32) h });
    D_Resize(&drawer, (rect) { 0.f, 0.f, (f32) w, (f32) h });
}

int main(int argc, char** argv) {
    M_ScratchInit();
    OS_Init();
    
    glfwInit();
    glfwDefaultWindowHints();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    GLFWwindow* window = glfwCreateWindow(1080, 720, "A Newer Title", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    
    gladLoadGL();
    
    I_InputState input = {0};
    I_Init(&input, window);
    glfwSetWindowUserPointer(window, &input);
    glfwSetWindowSizeCallback(window, resize_callback);
    I_RegisterKeyCallback(&input, C_KeyCallback);
    
    drawer = (D_Drawer) {0};
    D_Init(&drawer);
    
    _client_state = (C_ClientState) {0};
    C_Init(&_client_state);
    
    while (!glfwWindowShouldClose(window)) {
        I_Reset(&input);
        glfwPollEvents();
        
        C_Update(&input);
        
        D_BeginDraw(&drawer);
        C_Render(&drawer);
        D_EndDraw(&drawer);
        
        glfwSwapBuffers(window);
    }
    
    C_Shutdown();
    D_Shutdown(&drawer);
    I_Free(&input);
    
    glfwDestroyWindow(window);
    glfwTerminate();
    
    flush;
    
    M_ScratchFree();
}