#ifndef SCENE_H
#define SCENE_H
#include <string>
#include "texture.h"
#include "Model.h"
#include "Light.h"
#include "Camera.h"
#include "Material.h"

class Scene	// 场景描述类
{
public:
	vector<Light> Lights;
	vector<Model> Models;
	vector<Texture> Textures;
	vector<Material> Materials;
	// 场景里唯一的元素 若要更换就直接重新定义
	HDRMap* HDRMaps;
	Camera* camera;
	bool dirty;

	void addLight(const Light& light);
	void addModel(const string& fileName);
	void addTexture(const string& fileName);
	void addMaterial(const Material& material);
};

#endif // !SCENE_H
