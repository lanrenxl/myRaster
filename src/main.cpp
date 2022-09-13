#include "Render.h"
using namespace std;

Render *render;

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    if (!ImGui::GetIO().WantCaptureMouse)
        render->scene->mouse_callback(window, xposIn, yposIn);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    if (!ImGui::GetIO().WantCaptureMouse)
        render->scene->scroll_callback(window, xoffset, yoffset);
}

int main()
{
    // 初始化渲染器
    render = Render::get();
    // 设置回调函数
    glfwSetFramebufferSizeCallback(render->window, framebuffer_size_callback);
    glfwSetCursorPosCallback(render->window, mouse_callback);
    glfwSetScrollCallback(render->window, scroll_callback);
    render->imguiInit();
    //render->materials[0].addAlbedoTex("");
    render->addShader("D:/document/CODE/vs2019/openGL/myRaster/src/shaders/pbr.vert", "D:/document/CODE/vs2019/openGL/myRaster/src/shaders/pbr.frag");
    render->addShader("D:/document/CODE/vs2019/openGL/myRaster/src/shaders/hdr.vert", "D:/document/CODE/vs2019/openGL/myRaster/src/shaders/hdr.frag");
    // 初始化场景
    Scene* scene = new Scene();
    scene->addCamera();
    scene->addHDRMap("D:/document/CODE/vs2019/openGL/asset/fouriesburg_mountain_cloudy_4k.hdr", render->shaderSources[1]);
    auto jr = 1.0;
    scene->addLight(glm::vec3(-10.0f, 10.0f, 10.0f), glm::vec3(jr, jr, jr));
    scene->addLight(glm::vec3(10.0f, 10.0f, 10.0f), glm::vec3(jr, jr, jr));
    scene->addLight(glm::vec3(-10.0f, -10.0f, 10.0f), glm::vec3(jr, jr, jr));
    scene->addLight(glm::vec3(10.0f, -10.0f, 10.0f), glm::vec3(jr, jr, jr));
    scene->addModel("D:/document/CODE/vs2019/openGL/asset/xg.fbx", render->shaderSources[0]);
    //scene->addModel(make_shared<uvSphere>(), render->shaderSources[0]);
    render->scene = scene;
    render->scene->initScene();

    // render loop
    while (!glfwWindowShouldClose(render->window))
    {
        render->draw();
    }

    delete scene;
    delete render;
	return 0;
}