#include <glad/glad.h>
#include "defines.h"
#include <GLFW/glfw3.h>
#include <stdio.h>

#include "os/os.h"
#include "core/core.h"
#include "base/ds.h"

C_ClientState _client_state;
D_Drawer drawer;
D_CommandBuffer command_buffer;

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
    
    A_AssetLoader loader = {0};
    A_LoaderInit(&loader);
    
    I_InputState input = {0};
    I_Init(&input, window);
    glfwSetWindowUserPointer(window, &input);
    glfwSetWindowSizeCallback(window, resize_callback);
    I_RegisterKeyCallback(&input, C_KeyCallback);
    
    drawer = (D_Drawer) {0};
    D_Init(&drawer, &loader);
    command_buffer = (D_CommandBuffer) {0};
    D_CB_Init(&drawer, &command_buffer);
    
    _client_state = (C_ClientState) {0};
    C_Init(&_client_state, &loader);
    
    A_LoadAllFontsAndTextures(&loader);
    
    f32 start = 0.f; f32 end = 0.016f;
    f32 delta = 0.016f;
    
    while (!glfwWindowShouldClose(window)) {
        delta = end - start;
        start = glfwGetTime();
        
        I_Reset(&input);
        glfwPollEvents();
        
        C_Update(&input, &loader, delta);
        
        command_buffer.idx = 0;
        C_Render(&command_buffer);
        D_BeginDraw(&drawer);
        D_SubmitCommandBuffer(&drawer, &command_buffer);
        D_EndDraw(&drawer);
        
        glfwSwapBuffers(window);
        
        end = glfwGetTime();
    }
    
    C_Shutdown();
    D_CB_Free(&command_buffer);
    D_Shutdown(&drawer);
    I_Free(&input);
    A_LoaderFree(&loader);
    
    glfwDestroyWindow(window);
    glfwTerminate();
    
    flush;
    
    M_ScratchFree();
}