#ifndef LIGHT_H
#define LIGHT_H
#include <glm/glm.hpp>

class Light	//默认点光源
{
public:
	Light(glm::vec3 _pos, glm::vec3 _albedo) 
		: pos(_pos), albedo(_albedo)
	{}
	~Light() = default;

private:
	glm::vec3 pos;
	glm::vec3 albedo;
};

#endif // !LIGHT_H

