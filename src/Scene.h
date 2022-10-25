#ifndef SCENE_H
#define SCENE_H
#include <string>
#include "IBL_HDR.h"
#include "Light.h"
#include "Camera.h"
#include "Model.h"
#include "Quad.h"

#include <GLFW/glfw3.h>

class Scene	// 场景描述类
{
public:
	vector<Light> Lights;
	vector<Model> Models;
	// 着色器
	shared_ptr<Shader> hdrShader;
	shared_ptr<ShaderSource> shader;

	// HDR环境光照绘制
	IBL_HDR* ibl_hdr;
	Quad hdrQuad;
	// 相机相关
	Camera* camera;
	float lastX;
	float lastY;
	float deltaTime;
	float lastFrame;
	bool firstMouse;
	Model* hoverModel;
	// 场景是否改变
	bool dirty;
	GLuint LightTexBuffer;
	GLuint LightTex;
public:
	Scene();
	~Scene();

	void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
	void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

	void addLight(glm::vec3 pos, glm::vec3 color);
	void addModel(const string& fileName);
	void addHDRMap(const string& fileName);
	void addCamera();
	void setModelShader(shared_ptr<ShaderSource> _shader);
	void setHDRShader(shared_ptr<ShaderSource> hdrShader);

	virtual void init() = 0;

	virtual void update() = 0;
	virtual void draw() = 0;
};

class ForwardScene : public Scene
{
public:
	ForwardScene() {}
	void init() override;
	void update() override;
	void draw() override;
public:
private:
	void drawHDRMap();
	void drawModel();
};



#endif // !SCENE_H
