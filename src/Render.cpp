#include <glad/glad.h>

#include "Render.h"
#include "DefferedScene.h"
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include "Log.h"

Render* Render::get()
{
    // 单例模式
    static Render* render;
    if (render == nullptr)
    {
        render = new Render();
    }
    return render;
}

Render::Render()
{
    Log::Init();
    this->glfwWindowInit();
    this->gladContexInit();
    this->renderType = RenderType::ForwardRender;
    if (this->renderType == RenderType::ForwardRender)
    {
        this->scene = new ForwardScene();
        this->scene->setHDRShader(make_shared<ShaderSource>("D:/document/CODE/vs2019/openGL/myRaster/src/shaders/hdr.vert", "D:/document/CODE/vs2019/openGL/myRaster/src/shaders/hdr.frag"));
        this->scene->setModelShader(make_shared<ShaderSource>("D:/document/CODE/vs2019/openGL/myRaster/src/shaders/pbr.vert", "D:/document/CODE/vs2019/openGL/myRaster/src/shaders/pbr.frag"));
    }
    else if (this->renderType == RenderType::DefferedRender)
    {
        this->scene = new DefferedScene();
        this->scene->setHDRShader(make_shared<ShaderSource>("D:/document/CODE/vs2019/openGL/myRaster/src/shaders/defferHDR.vert", "D:/document/CODE/vs2019/openGL/myRaster/src/shaders/defferHDR.frag"));
        this->scene->setModelShader(make_shared<ShaderSource>("D:/document/CODE/vs2019/openGL/myRaster/src/shaders/defferGbuffer.vert", "D:/document/CODE/vs2019/openGL/myRaster/src/shaders/defferGbuffer.frag"));
    }
    RT_INFO("Render init success");
}

Render::~Render()
{
    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    // glfw: terminate, clearing all previously allocated GLFW resources.
    delete scene;
    glfwTerminate();
}

void Render::addShader(std::string vertPath, std::string fragPath)
{
    this->shaderSources.push_back(ShaderSource(vertPath.c_str(), fragPath.c_str()));
    RT_INFO("Add shader success");
}

void Render::draw()
{
    glfwPollEvents();
    // per-frame time logic
        // --------------------
    float currentFrame = static_cast<float>(glfwGetTime());
    this->scene->deltaTime = currentFrame - this->scene->lastFrame;
    this->scene->lastFrame = currentFrame;

    // input
    processInput(window);

    // render
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    if (scene)
    {
        this->scene->update();
        this->scene->draw();
    }
    else
        RT_ERROR("Scene NULL");
    // render light source (simply re-render sphere at light positions)
    // this looks a bit off as we use the same shader, but it'll make their positions obvious and 
    // keeps the codeprint small.

    imguiRender();
    // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
    glfwSwapBuffers(window);
}

void Render::processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        this->scene->camera->ProcessKeyboard(FORWARD, scene->deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        this->scene->camera->ProcessKeyboard(BACKWARD, scene->deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        this->scene->camera->ProcessKeyboard(LEFT, scene->deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        this->scene->camera->ProcessKeyboard(RIGHT, scene->deltaTime);
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        this->scene->camera->ProcessKeyboard(UP, scene->deltaTime);
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        this->scene->camera->ProcessKeyboard(DOWN, scene->deltaTime);
}

bool Render::glfwWindowInit()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    this->window = glfwCreateWindow(WIDTH, HEIGHT, "Raster", NULL, NULL);
    
    if (this->window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }
    glfwMakeContextCurrent(this->window);
    glfwSwapInterval(1); // Enable vsync
    // tell GLFW to capture our mouse
    glfwSetInputMode(this->window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    return true;
}

bool Render::gladContexInit()
{
    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return false;
    }
    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);
    return true;
}

void Render::imguiInit()
{
    // 设置ImGui设置上下文.
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& m_io = ImGui::GetIO();
    m_io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    ImFont* font = m_io.Fonts->AddFontFromFileTTF("C:/Windows/Fonts/AdobeFangsongStd-Regular.otf", 20.0f, NULL, m_io.Fonts->GetGlyphRangesChineseSimplifiedCommon());
    //设置颜色风格
    
    ImGui::StyleColorsDark();
    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(this->window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
}

void Render::imguiRender()
{
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    //ImGui::ShowDemoWindow();

    // Create a window called "My First Tool", with a menu bar.
    ImGui::Begin("transform");
    string fps = "FPS: " + to_string(1.0 / scene->deltaTime);
    ImGui::Text(fps.c_str());
    /*ImGui::Text("变换矩阵: ");
    if (ImGui::DragFloat3("roate", scene->hoverModel->transform.rotate, 1.0, -90.0, 90.0))
    {
        scene->hoverModel->dirty = true;
    }
    if (ImGui::DragFloat3("scale", &scene->hoverModel->transform.scaleV[0], 1.0, 0, 1000))
    {
        scene->hoverModel->dirty = true;
    }
    if (ImGui::DragFloat3("trans", &scene->hoverModel->transform.transV[0], 1.0, -90.0, 90.0))
    {
        scene->hoverModel->dirty = true;
    }*/
    ImGui::Text("material: ");
    if (ImGui::SliderFloat("roughness", &scene->hoverModel->meshes[0]->materials.roughness, 0.f, 1.0f))
    {
        for (auto &item : scene->hoverModel->meshes)
        {
            item->materials.roughness = scene->hoverModel->meshes[0]->materials.roughness;
        }
        scene->hoverModel->meshes[0]->dirty = true;
    }
    if (ImGui::SliderFloat("metallic", &scene->hoverModel->meshes[0]->materials.metallic, 0.f, 1.0f))
    {
        for (auto& item : scene->hoverModel->meshes)
        {
            item->materials.metallic = scene->hoverModel->meshes[0]->materials.metallic;
        }
        scene->hoverModel->meshes[0]->dirty = true;
    }
    if (ImGui::SliderFloat("ao", &scene->hoverModel->meshes[0]->materials.ao, 0.f, 1.0f))
    {
        for (auto& item : scene->hoverModel->meshes)
        {
            item->materials.ao = scene->hoverModel->meshes[0]->materials.ao;
        }
        scene->hoverModel->meshes[0]->dirty = true;
    }
    ImGui::End();

    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    //RT_INFO("Render.cpp: Imgui Render");
}

