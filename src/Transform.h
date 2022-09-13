#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Transform
{
public:
	float rotate[3];
	glm::vec3 scaleV, transV;
	glm::mat4 modelMat;
	Transform()
		: modelMat(glm::mat4(1.0f)), rotate{ -90.0f, 0.0f, 0.0f },
		scaleV(glm::vec3(1.0f, 1.0f, 1.0f)), transV(glm::vec3(0.0, 0.0, 0.0))
	{
		update();
	}
	void update()
	{
		modelMat = glm::mat4(1.0);
		modelMat = glm::scale(modelMat, scaleV);
		modelMat = glm::rotate(modelMat, glm::radians(rotate[0]), glm::vec3(1.0, 0.0, 0.0));
		modelMat = glm::rotate(modelMat, glm::radians(rotate[1]), glm::vec3(0.0, 1.0, 0.0));
		modelMat = glm::rotate(modelMat, glm::radians(rotate[2]), glm::vec3(0.0, 0.0, 1.0));
		modelMat = glm::translate(modelMat, transV);
	}
};

#endif // !TRANSFORM_H

