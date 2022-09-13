#pragma once
//#include <GLFW/glfw3.h>
#include "Shader.h"
#include <memory>

class Quad
{
public:
    Quad();
    void Draw(std::shared_ptr<Shader>);

private:
    GLuint vao;
    GLuint vbo;
};