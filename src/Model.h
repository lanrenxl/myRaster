#ifndef MODEL_H
#define MODEL_H

#include <glad/glad.h> 

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>

#include <string>
#include <vector>
#include <memory>

#include "globel.h"
#include "Shader.h"
#include "Mesh.h"
using namespace std;

class Model
{
public:
    // model data
    vector<shared_ptr<Mesh> > meshes;   // 网格数据随vector和shared_ptr自动管理
    // shader 源码数据
    ShaderSource& shaderSource;

    // constructor, expects a filepath to a 3D model.
    Model(string const& path, ShaderSource& shader);
    ~Model() = default;

private:
    // loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
    void loadModel(string const& path);
    
    // processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
    void processNode(aiNode* node, const aiScene* scene);

    shared_ptr<Mesh> processMesh(aiMesh* mesh, const aiScene* scene);
};
#endif