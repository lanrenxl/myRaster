#ifndef MESH_H
#define MESH_H

#include <glad/glad.h> // holds all OpenGL type declarations

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <string>
#include <vector>
#include "globel.h"
#include "Log.h"
#include "Material.h"
#include "Shader.h"
#include "Camera.h"

using namespace std;

#define MAX_BONE_INFLUENCE 4

struct Vertex {
    // position
    glm::vec3 Position;
    // normal
    glm::vec3 Normal;
    // texCoords
    glm::vec2 TexCoords;
};

class Mesh {
public:
    // mesh Data
    vector<Vertex>       vertices;
    vector<unsigned int> indices;
    // 每一个网格指向model导入的一系列材质, 每个材质的纹理来源tex可能不一样
    int materialIndex;
    // 自动管理shader
    shared_ptr<Shader> shader;
    unsigned int VAO;
    // 网格的图元绘制类型
    GLenum drawType;

    Mesh() : VAO(0), VBO(0), EBO(0), drawType(GL_TRIANGLES), materialIndex(0) {}
    
    // constructor
    Mesh(vector<Vertex> _vertices, vector<unsigned int> _indices, int _materialIndex);

    // initializes all the buffer objects/arrays
    void setupMesh();
    void updateShader(Camera& camera);
    void draw();
protected:
    // render data 
    unsigned int VBO, EBO;
};

class uvSphere : public Mesh
{
public:
    uvSphere();
    void setupMesh();
};

#endif