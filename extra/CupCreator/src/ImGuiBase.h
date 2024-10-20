#ifndef UIEDITOR_IMGUIBASE_H
#define UIEDITOR_IMGUIBASE_H

#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <vector>

class ImGuiBase {
public:
    virtual void Init(GLFWwindow * window, const char * glsl_version);
    virtual void Update();
    virtual void Render();
    virtual void Shutdown();
    virtual void NewFrame();
};

#endif //UIEDITOR_IMGUIBASE_H
