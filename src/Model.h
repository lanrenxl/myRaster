#ifndef MODEL_H
#define MODEL_H

#include <glad/glad.h> 

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <memory>

#include "globel.h"
#include "Camera.h"
#include "Material.h"
#include "Shader.h"
#include "Mesh.h"
#include "Transform.h"
using namespace std;

class Model
{
public:
    // model data
    vector<shared_ptr<Mesh> > meshes;   // 网格数据随vector和shared_ptr自动管理
    // 若是文件导入则有文件路径
    string directory;
    // shader 源码数据
    ShaderSource& shaderSource;
    // 材质数据 统一管理
    vector<Material> materials;
    // 纹理数据 统一管理
    vector<Texture> textures;
    // 是否进行gamma校准
    bool gammaCorrection;

    // 是否更改modelMat矩阵
    bool dirty;
    // model的固有transfrom矩阵
    Transform transform;

    // constructor, expects a filepath to a 3D model.
    Model() = default;
    Model(string const& path, ShaderSource& shader, bool gamma = false);
    Model(shared_ptr<Mesh> mesh, ShaderSource& shader, bool gamma = false);
    ~Model() = default;

    void initMesh(GLuint lightNum, GLuint _HDRTex, GLuint _LightTex);
    void initMaterial(shared_ptr<Mesh>& mesh);
    void draw(Camera& camera);

private:
    // loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
    void loadModel(string const& path);
    
    // processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
    void processNode(aiNode* node, const aiScene* scene);

    shared_ptr<Mesh> processMesh(aiMesh* mesh, const aiScene* scene);
};
#endif