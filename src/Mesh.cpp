#include "Mesh.h"
#include "Log.h"
#include "Render.h"
#include "Camera.h"

Mesh::Mesh(vector<Vertex>& _vertices, vector<unsigned int>& _indices, vector<Texture>& textures, Material material, ShaderSource& shaderSource)
{
    this->drawType = GL_TRIANGLES;
    this->vertices = _vertices;
    this->indices = _indices;
    this->materials = material;
    this->textures = textures;

    auto vertCode = shaderSource.vertexCode;
    auto fragCode = shaderSource.fragmentCode;
    auto geoCode = shaderSource.geometryCode;
    auto index = shaderSource.fragmentCode.find("#define uniformTex");
    if (this->materials.shaderDefine != "")
    {
        fragCode.insert(index, this->materials.shaderDefine);
    }
    this->shader = make_shared<Shader>(vertCode, fragCode, geoCode);
    // now that we have all the required data, set the vertex buffers and its attribute pointers.
}

void Mesh::init()
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
    
    this->shader->use();
    // 初始化shader的uniform 设置光源信息
    this->shader->setInt("lightNum", Render::get()->scene->Lights.size());
    // 设置纹理单元
    if (Render::get()->scene->LightTex != -1)
    {
        this->shader->setInt("lights", 0);
    }
    RT_INFO("mesh.cpp: Mesh VAO Generate success");
}

void Mesh::update()
{
    // 绑定shader
    this->shader->use();
    // 更新需要更新的内容
    auto camera = Render::get()->scene->camera;
    // 设置mvp矩阵
    this->shader->setVec3("camPos", camera->Position);
    this->shader->setMat4("model", this->transform.modelMat);
    this->shader->setMat4("view", camera->GetViewMatrix());
    glm::mat4 projection = glm::perspective(glm::radians(camera->Zoom), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
    this->shader->setMat4("projection", projection);
    //RT_INFO("Mesh.cpp: Update Camera success");
}

void Mesh::draw()
{
    //glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindVertexArray(this->VAO);	// 绑定VAO
    glDrawElements(this->drawType, this->indices.size(), GL_UNSIGNED_INT, 0);	// 不同mesh使用不同的图元进行绘制
}
