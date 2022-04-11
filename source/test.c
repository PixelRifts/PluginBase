#include <glad/glad.h>
#include "defines.h"
#include <GLFW/glfw3.h>
#include <stdio.h>

#include "os/os.h"
#include "core/core.h"
#include "base/ds.h"

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
    
    D_Drawer drawer = {0};
    D_Init(&drawer);
    
    C_ClientState client_state = {0};
    C_Init(&client_state);
    
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
    
    glfwDestroyWindow(window);
    glfwTerminate();
    
    flush;
    
    M_ScratchFree();
}