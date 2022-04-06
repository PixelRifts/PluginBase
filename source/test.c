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
    
    M_Scratch scratch = scratch_get();
    C_Panel* root = C_PanelAlloc(&scratch.arena, (rect) { 0.f, 0.f, 1080.f, 720.f });
    C_PanelChop(&scratch.arena, root, PanelChop_Horizontal);
    C_PanelChop(&scratch.arena, root->b, PanelChop_Vertical);
    
    while (!glfwWindowShouldClose(window)) {
        I_Reset(window);
        glfwPollEvents();
        
        C_Update();
        
        D_BeginDraw();
        C_Render();
        C_PanelRender(root);
        D_EndDraw();
        
        glfwSwapBuffers(window);
    }
    
    scratch_return(&scratch);
    
    C_Shutdown();
    
    D_Shutdown();
    R_ShutdownOpenGL();
    
    glfwDestroyWindow(window);
    glfwTerminate();
    
    flush;
    
    M_ScratchFree();
}