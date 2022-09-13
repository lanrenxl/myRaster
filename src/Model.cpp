#include "Model.h"
#include "Log.h"
#include "Render.h"

Model::Model(string const& path, ShaderSource& shader, bool gamma)
	: gammaCorrection(gamma), shaderSource(shader), dirty(false)
{
	textures.push_back(Texture("./asset/default.png"));
	loadModel(path);
}

Model::Model(shared_ptr<Mesh> mesh, ShaderSource& shader, bool gamma)
	: gammaCorrection(gamma), shaderSource(shader), dirty(false)
{
	textures.emplace_back(Texture("./asset/default.png"));
	materials.emplace_back(Material());
	this->meshes.emplace_back(mesh);
	meshes[meshes.size() - 1]->materialIndex = 0;
}

// 每一个mesh带有一个shader
void Model::initMesh(GLuint lightNum, GLuint _HDRTex, GLuint _LightTex)
{
	for (int i = 0; i < meshes.size(); i++)
	{
		meshes[i]->shader->use();
		// 初始化mesh的VAO
		meshes[i]->setupMesh();
		// 初始化shader的uniform
		// 设置模型矩阵
		meshes[i]->shader->setMat4("model", this->transform.modelMat);
		// 设置光源信息
		meshes[i]->shader->setInt("lightNum", lightNum);
		// 设置纹理单元
		if (_LightTex != -1)
		{
			meshes[i]->shader->setInt("lights", 0);
		}
	}
}
// 每一个mesh带有一个材质 根据材质初始化shader的uniform
void Model::initMaterial(shared_ptr<Mesh>& mesh)
{
	mesh->shader->use();
	mesh->shader->setMat4("model", this->transform.modelMat);
	if (mesh->materialIndex == -1)
	{
		RT_ERROR("model.cpp: Mesh no material");
		return;
	}
	Material& mat = this->materials[mesh->materialIndex];
	// 设置材质信息
	// 反射率颜色albedo
	if (mat.albedoTex == -1)
		mesh->shader->setVec3("albedo", mat.albedo);
	else
	{
		// 初始化纹理单元
		this->textures[mat.albedoTex].bindGPU(1);
		mesh->shader->setInt("albedo", 1);
	}
	// 遮蔽AO
	if (mat.aoTex == -1)
		mesh->shader->setFloat("ao", mat.ao);
	else
	{
		this->textures[mat.aoTex].bindGPU(2);
		mesh->shader->setInt("ao", 2);
	}
	// 粗糙度roughtness
	if (mat.roughnessTex == -1)
		mesh->shader->setFloat("roughness", mat.roughness);
	else
	{
		this->textures[mat.roughnessTex].bindGPU(3);
		mesh->shader->setInt("roughness", 3);
	}
	// 金属度metallic
	if (mat.metallicTex == -1)
		mesh->shader->setFloat("metallic", mat.metallic);
	else
	{
		this->textures[mat.metallicTex].bindGPU(4);
		mesh->shader->setInt("metallic", 4);
	}

	//RT_INFO("mdoel.cpp: Mesh init shader success");
}

// 以mesh为单位进行绘制
void Model::draw(Camera& camera)
{
	if (meshes.empty())	// 若mesh为空则停止绘制并报错
	{
		RT_ERROR("Model Meshs is NULL");
		return;
	}
	for (int i = 0; i < meshes.size(); i++)	// 遍历所有的mesh网格, 使用model自己的shader进行绘制
	{
		// 更新相机对应的参数, 如相机的位置, 相机的view矩阵
		// 只有当数据更改时才调用 主要更新相机相关数据
		if (dirty)
		{
			transform.update();
			dirty = false;
		}
		initMaterial(meshes[i]);
		meshes[i]->updateShader(camera);
		meshes[i]->draw();
	}
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
	// retrieve the directory path of the filepath
	directory = path.substr(0, path.find_last_of('/'));

	// process ASSIMP's root node recursively
	processNode(scene->mRootNode, scene);
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
	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
	if (material->GetTextureCount(aiTextureType_DIFFUSE) < 1)
	{
		tempMat.addAlbedoTex(0);
	}
	else
	{
		for (unsigned int i = 0; i < material->GetTextureCount(aiTextureType_DIFFUSE); i++)
		{
			// 创建纹理 纹理添加至model的texture数组中统一管理, material中使用索引进行绑定
			aiString str;
			material->GetTexture(aiTextureType_DIFFUSE, i, &str);
			this->textures.emplace_back(Texture(str.C_Str()));
			RT_INFO("load texture: {0}", str.C_Str());
			tempMat.addAlbedoTex(this->textures.size() - 1);
		}
	}
	this->materials.emplace_back(tempMat);
	// return a mesh object created from the extracted mesh data
	auto tempMesh = make_shared<Mesh>(vertices, indices, this->materials.size() - 1);
	auto vertCode = this->shaderSource.vertexCode;
	auto fragCode = this->shaderSource.fragmentCode;
	auto geoCode = this->shaderSource.geometryCode;
	auto index = this->shaderSource.fragmentCode.find("uniform");
	if (tempMat.shaderDefine != "")
	{
		fragCode.insert(index, tempMat.shaderDefine);
	}
	tempMesh->shader = make_shared<Shader>(vertCode, fragCode, geoCode);
	RT_INFO("model.cpp: shader generate success");
	return tempMesh;
}
