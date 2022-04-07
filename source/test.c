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
    
    I_Init(window);
    
    R_InitOpenGL();
    D_Init();
    
    C_Init();
    
    while (!glfwWindowShouldClose(window)) {
        I_Reset(window);
        glfwPollEvents();
        
        C_Update();
        
        D_BeginDraw();
        C_Render();
        D_EndDraw();
        
        glfwSwapBuffers(window);
    }
    
    C_Shutdown();
    
    D_Shutdown();
    R_ShutdownOpenGL();
    
    glfwDestroyWindow(window);
    glfwTerminate();
    
    flush;
    
    M_ScratchFree();
}