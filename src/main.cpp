#include "Render.h"
#include "Scene.h"
#include "DefferedScene.h"
#include "Shader.h"
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

using namespace std;


// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
    WIDTH = width;
    HEIGHT = height;
}

// glfw: whenever the mouse moves, this callback is called
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    if (!ImGui::GetIO().WantCaptureMouse)
        Render::get()->scene->mouse_callback(window, xposIn, yposIn);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    if (!ImGui::GetIO().WantCaptureMouse)
        Render::get()->scene->scroll_callback(window, xoffset, yoffset);
}

int main()
{
    // 初始化渲染器
    Render* render = Render::get();
    // 设置回调函数
    glfwSetFramebufferSizeCallback(render->window, framebuffer_size_callback);
    glfwSetCursorPosCallback(render->window, mouse_callback);
    glfwSetScrollCallback(render->window, scroll_callback);
    render->imguiInit();
    // 初始化场景
    Scene* scene = render->scene;
    scene->addCamera();
    //scene->addHDRMap("D:/document/CODE/vs2019/openGL/asset/fouriesburg_mountain_cloudy_4k.hdr");
    scene->addHDRMap("D:/document/CODE/vs2019/openGL/asset/circus_arena_4k.hdr");
    auto jr = 1000;
    scene->addLight(glm::vec3(-10.0f, 10.0f, 10.0f), glm::vec3(jr, jr, jr));
    scene->addLight(glm::vec3(10.0f, 10.0f, 10.0f), glm::vec3(jr, jr, jr));
    scene->addLight(glm::vec3(-10.0f, -10.0f, 10.0f), glm::vec3(jr, jr, jr));
    scene->addLight(glm::vec3(10.0f, -10.0f, 10.0f), glm::vec3(jr, jr, jr));
    scene->addModel("D:/document/models/yin/yin.fbx");
    //scene->addModel("D:/temp/Cerberus_by_Andrew_Maximov/Cerberus_LP.FBX");
    // auto sphere = shared_ptr<Mesh>(new uvSphere);
    // scene->addModel(sphere, render->shaderSources[0]);
    render->scene->init();

    // render loop
    while (!glfwWindowShouldClose(render->window))
    {
        render->draw();
    }
    delete render;
	return 0;
}