#include "Model.h"
#include "Log.h"
#include <assimp/postprocess.h>

Model::Model(string const& path, ShaderSource& shader)
	: shaderSource(shader)
{
	loadModel(path);
}

void Model::loadModel(string const& path)
{
	// read file via ASSIMP
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
	// check for errors
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
	{
		cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
		return;
	}
	// process ASSIMP's root node recursively
	processNode(scene->mRootNode, scene);
}

void setTransform()
{

}

void Model::processNode(aiNode* node, const aiScene* scene)
{
	// process each mesh located at the current node
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		// the node object only contains indices to index the actual objects in the scene. 
		// the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(mesh, scene));
	}
	// after we've processed all of the meshes (if any) we then recursively process each of the children nodes
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene);
	}
}

shared_ptr<Mesh> Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
	// data to fill
	vector<Vertex> vertices;
	vector<unsigned int> indices;

	// walk through each of the mesh's vertices
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;
		glm::vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
		// positions
		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertex.Position = vector;
		// normals
		if (mesh->HasNormals())
		{
			vector.x = mesh->mNormals[i].x;
			vector.y = mesh->mNormals[i].y;
			vector.z = mesh->mNormals[i].z;
			vertex.Normal = vector;
		}
		// texture coordinates
		if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
		{
			glm::vec2 vec;
			// a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
			// use models where a vertex can have multiple texture coordinates so we always take the first set (0).
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.TexCoords = vec;
		}
		else
			vertex.TexCoords = glm::vec2(0.0f, 0.0f);
		vertices.push_back(vertex);
	}
	// now walk through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		// retrieve all indices of the face and store them in the indices vector
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}
	// process materials
	// 处理材质和纹理信息
	// 创建材质 材质添加值model的materials数组中统一管理, mesh中使用索引进行绑定
	Material tempMat;
	vector<Texture> tempTex;
	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
	if (material->GetTextureCount(aiTextureType_DIFFUSE) < 1)
	{
		tempMat.addAlbedoTex(-1);
	}
	else
	{
		for (unsigned int i = 0; i < material->GetTextureCount(aiTextureType_DIFFUSE); i++)
		{
			// 创建纹理 纹理添加至model的texture数组中统一管理, material中使用索引进行绑定
			aiString str;
			material->GetTexture(aiTextureType_DIFFUSE, i, &str);
			
			tempTex.emplace_back(Texture(str.C_Str()));
			RT_INFO("load texture: {0}", str.C_Str());
			tempMat.addAlbedoTex(0);
		}
	}
	// return a mesh object created from the extracted mesh data
	auto tempMesh = make_shared<Mesh>(vertices, indices, tempTex, tempMat, this->shaderSource);
	RT_INFO("model.cpp: shader generate success");
	return tempMesh;
}
