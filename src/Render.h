#ifndef RENDER_H
#define RENDER_H

#include <vector>
#include <string>
#include "Shader.h"
#include "Scene.h"
#include <GLFW/glfw3.h>

enum class RenderType
{
    None = 0,
    ForwardRender,
    DefferedRender
};

class Render
{
public:
    GLFWwindow* window;
    vector<ShaderSource> shaderSources;
    Scene* scene;
    int nrRows = 7;
    int nrColumns = 7;
    float spacing = 2.5;
    RenderType renderType;
public:
    // 全局获取render
    static Render* get();
    ~Render();
    
    void addShader(std::string vertPath, std::string fragPath);
    void draw();
    void imguiInit();
private:
    Render();
    Render(const Render& render) = delete;
    Render& operator=(const Render& render) { return *this; }
private:

    void processInput(GLFWwindow* window);
    bool glfwWindowInit();
    bool gladContexInit();
    
    void imguiRender();
    
};

//class forwardRender : public Render // 前向渲染
//{
//
//};
//
//class defferedRender : public Render    // 延迟渲染
//{
//
//};

#endif // !RENDER_H

