#include<Render.h>

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
    //this->imguiInit();
    RT_INFO("Render init success");
}

Render::~Render()
{
    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    // glfw: terminate, clearing all previously allocated GLFW resources.
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
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    if (scene)
    {
        // pass1 绘制天空球
        this->scene->drawHDRMap();
        // 绘制模型
        this->scene->drawModel();
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
    }
    ImGui::End();

    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    //RT_INFO("Render.cpp: Imgui Render");
}

