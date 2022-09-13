#include "Mesh.h"

Mesh::Mesh(vector<Vertex> _vertices, vector<unsigned int> _indices, int _materialIndex)
    : drawType(GL_TRIANGLES)
{
    this->vertices = _vertices;
    this->indices = _indices;
    this->materialIndex = _materialIndex;
    // now that we have all the required data, set the vertex buffers and its attribute pointers.
}

void Mesh::setupMesh()
{
    // create buffers/arrays
    glGenVertexArrays(1, &this->VAO);
    glGenBuffers(1, &this->VBO);
    glGenBuffers(1, &this->EBO);

    glBindVertexArray(this->VAO);
    // load data into vertex buffers
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
    // A great thing about structs is that their memory layout is sequential for all its items.
    // The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
    // again translates to 3/2 floats which translates to a byte array.
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    // set the vertex attribute pointers
    // vertex Positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    // vertex normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
    // vertex texture coords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
    RT_INFO("mesh.cpp: Mesh VAO Generate success");
}

void Mesh::updateShader(Camera& camera)
{
    // 设置mvp矩阵
    this->shader->setVec3("camPos", camera.Position);
    this->shader->setMat4("view", camera.GetViewMatrix());
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
    this->shader->setMat4("projection", projection);
    //RT_INFO("Mesh.cpp: Update Camera success");
}

void Mesh::draw()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindVertexArray(this->VAO);	// 绑定VAO
    glDrawElements(this->drawType, this->indices.size(), GL_UNSIGNED_INT, 0);	// 不同mesh使用不同的图元进行绘制
}

uvSphere::uvSphere()
{
    drawType = GL_TRIANGLE_STRIP;
    this->materialIndex = -1;
    this->setupMesh();
}

void uvSphere::setupMesh()
{
    this->vertices.clear();
    this->indices.clear();

    const unsigned int X_SEGMENTS = 64;
    const unsigned int Y_SEGMENTS = 64;
    const float PI = 3.14159265359f;
    for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
    {
        for (unsigned int y = 0; y <= Y_SEGMENTS; ++y)
        {
            float xSegment = (float)x / (float)X_SEGMENTS;
            float ySegment = (float)y / (float)Y_SEGMENTS;
            float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
            float yPos = std::cos(ySegment * PI);
            float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
            Vertex tempV;
            tempV.Position = glm::vec3(xPos, yPos, zPos);
            tempV.TexCoords = glm::vec2(xSegment, ySegment);
            tempV.Normal = glm::vec3(xPos, yPos, zPos);
            this->vertices.push_back(tempV);
        }
    }

    bool oddRow = false;
    for (unsigned int y = 0; y < Y_SEGMENTS; ++y)
    {
        if (!oddRow) // even rows: y == 0, y == 2; and so on
        {
            for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
            {
                this->indices.push_back(y * (X_SEGMENTS + 1) + x);
                this->indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
            }
        }
        else
        {
            for (int x = X_SEGMENTS; x >= 0; --x)
            {
                this->indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
                this->indices.push_back(y * (X_SEGMENTS + 1) + x);
            }
        }
        oddRow = !oddRow;
    }
    glGenVertexArrays(1, &this->VAO);

    glGenBuffers(1, &this->VBO);
    glGenBuffers(1, &this->EBO);
    glBindVertexArray(this->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
    glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(Vertex), &this->vertices[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(unsigned int), &this->indices[0], GL_STATIC_DRAW);
    unsigned int stride = (3 + 2 + 3) * sizeof(float);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
}
