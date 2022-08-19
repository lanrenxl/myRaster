#ifndef MATERIAL_H
#define MATERIAL_H
#include <glm/glm.hpp>
class Material
{
public:
	Material()
	{
		albedo = glm::vec3(0.5, 0.5, 0.5);
		ao = 0.5f;
		roughness = 0.5f;
		metallic = 0.5f;
	}

	glm::vec3 albedo;
	float ao;
	float roughness;
	float metallic;
};
#endif // !MATERIAL
