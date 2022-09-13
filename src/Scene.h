#ifndef SCENE_H
#define SCENE_H
#include <string>

#include "Log.h"
#include "Light.h"
#include "Camera.h"
#include "Texture.h"
#include "Model.h"
#include "Quad.h"
#include <GLFW/glfw3.h>

class Scene	// 场景描述类
{
public:
	vector<Light> Lights;
	vector<Model> Models;
	// 场景里唯一的元素 若要更换就直接重新定义
	shared_ptr<Shader> hdrShader;
	HDRMap* hdrMap;
	Quad hdrQuad;
	// 相机相关
	Camera* camera;
	float lastX;
	float lastY;
	// timing
	float deltaTime;
	float lastFrame;
	bool firstMouse;
	// 场景是否改变
	Model* hoverModel;
	bool dirty;
	GLuint LightTexBuffer;
	GLuint HDRTex;
	GLuint LightTex;
	Scene();
	~Scene();

	void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
	void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

	void addLight(glm::vec3 pos, glm::vec3 color);
	void addModel(const string& fileName, ShaderSource& shader);
	void addModel(shared_ptr<Mesh>& mesh, ShaderSource& shader);
	void addHDRMap(const string& fileName, ShaderSource& hdrShader);
	void addCamera();
	void initScene();
	void drawHDRMap();
	void drawModel();
};

#endif // !SCENE_H
