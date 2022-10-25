#ifndef MATERIAL_H
#define MATERIAL_H
#include <glm/glm.hpp>
#include <string>

// 基础PBR材质类
class Material
{
public:
	Material();
	~Material() = default;
	void addAlbedoTex(size_t texIndex);	// 通过文件添加纹理至材质中
	void addAoTex(size_t texIndex);
	void addRoughnessTex(size_t texIndex);
	void addMetallicTex(size_t texIndex);
	void setAlbedo(glm::vec3 _albedo);	// 通过值设置材质数据
	void setAo(float _ao);
	void setRoughness(float _roughness);
	void setMetallic(float _metallic);
public:
	glm::vec3 albedo;
	float ao;
	float roughness;
	float metallic;
	size_t albedoTex;
	size_t aoTex;
	size_t roughnessTex;
	size_t metallicTex;
	// 网格的shader里的define
	std::string shaderDefine;
};
#endif // !MATERIAL
