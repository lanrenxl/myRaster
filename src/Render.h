#ifndef RENDER_H
#define RENDER_H
#include<iostream>
#include<vector>

#include<glm/glm.hpp>
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<imgui-docking/imgui.h>
#include<imgui-docking/imgui_impl_glfw.h>
#include<imgui-docking/imgui_impl_opengl3.h>
#include<ImGuizmo/ImGuizmo.h>
#include"Log.h"
#include"Scene.h"
#include"Shader.h"

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

class Render
{
public:
    GLFWwindow* window;
    Shader* shader;
    glm::vec3 lightPositions[4] = {
        glm::vec3(-10.0f,  10.0f, 10.0f),
        glm::vec3(10.0f,  10.0f, 10.0f),
        glm::vec3(-10.0f, -10.0f, 10.0f),
        glm::vec3(10.0f, -10.0f, 10.0f),
    };
    glm::vec3 lightColors[4] = {
        glm::vec3(300.0f, 300.0f, 300.0f),
        glm::vec3(300.0f, 300.0f, 300.0f),
        glm::vec3(300.0f, 300.0f, 300.0f),
        glm::vec3(300.0f, 300.0f, 300.0f)
    };

    int nrRows = 7;
    int nrColumns = 7;
    float spacing = 2.5;
public:
    Render();
    ~Render();
    void draw();
private:
    bool glfwWindowInit();
    bool gladContexInit();
    void imguiInit();
    void shaderInit();
    void imguiRender();
};

class forwardRender : public Render // 前向渲染
{

};

class defferedRender : public Render    // 延迟渲染
{

};

#endif // !RENDER_H

