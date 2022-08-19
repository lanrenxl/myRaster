#ifndef LIGHT_H
#define LIGHT_H
#include <glm/glm.hpp>

class Light	//默认点光源
{
public:
	Light(glm::vec3 _albedo, glm::vec3 _pos) 
		: albedo(_albedo), pos(_pos)
	{}
	~Light() = default;

private:
	glm::vec3 albedo;
	glm::vec3 pos;
};

#endif // !LIGHT_H

