#pragma once
#include <glad/glad.h>
#include <memory>
#include "Shader.h"

class Quad
{
public:
    Quad();
    void Draw(std::shared_ptr<Shader>);
private:
    GLuint vao;
    GLuint vbo;
};