#include <iostream>
#include "ImGuiBase.h"
#include "editor/EditorBase.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

int main() {
    if (!glfwInit())
        return 1;

    const char *glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    GLFWwindow *window = glfwCreateWindow(1280, 720, "Cosmos Cup Creator", nullptr, nullptr);
    if (window == nullptr)
        return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        throw("Unable to context to OpenGL");

    int screen_width, screen_height;
    glfwGetFramebufferSize(window, &screen_width, &screen_height);
    glViewport(0, 0, screen_width, screen_height);

    EditorBase myimgui;
    myimgui.Init(window, glsl_version);
    while(!glfwWindowShouldClose(window)){
        glfwPollEvents();
        glClearColor(0.1f,0.1f,0.1f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        myimgui.NewFrame();
        myimgui.Update();
        myimgui.Render();
        glfwSwapBuffers(window);
    }
    myimgui.Shutdown();

    return EXIT_SUCCESS;
}
