#ifndef MESH_H
#define MESH_H

#include <glad/glad.h> // holds all OpenGL type declarations

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <string>
#include <vector>
#include "globel.h"
#include "Shader.h"
#include "Transform.h"
#include "Material.h"
#include "Texture.h"

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
    
    // 材质数据
    Material materials;
    // 纹理数据
    vector<Texture> textures;
    // 自动管理shader
    shared_ptr<Shader> shader;
    // 网格的图元绘制类型
    GLenum drawType;
    // 是否更改modelMat矩阵
    bool dirty;
    // model的固有transfrom矩阵
    Transform transform;
public:
    
    // constructor
    Mesh(vector<Vertex>& _vertices, vector<unsigned int>& _indices, vector<Texture>& textures, Material material, ShaderSource& shader);

    // initializes all the buffer objects/arrays
    void init();
    void update();
    void draw();
protected:
    // render data 
    unsigned int VBO, EBO, VAO;
    // mesh Data
    vector<Vertex>       vertices;
    vector<unsigned int> indices;
};


#endif