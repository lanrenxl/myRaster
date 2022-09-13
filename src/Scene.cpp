#include "Scene.h"

Scene::Scene()
	: hdrMap(nullptr), camera(nullptr), dirty(false), 
	lastX(WIDTH/2.0), lastY(HEIGHT/2.0f), deltaTime(0.0f), lastFrame(0.0f),
	HDRTex(-1), LightTex(-1), LightTexBuffer(-1), firstMouse(true), hoverModel(nullptr), hdrQuad(Quad())
{}

Scene::~Scene()
{
	if (this->camera != nullptr)
		delete this->camera;
	if (this->hdrMap != nullptr)
		delete this->hdrMap;
}

void Scene::mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
	{
		firstMouse = true;
		return;
	}
	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);

	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	this->camera->ProcessMouseMovement(xoffset, yoffset);
}

void Scene::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	this->camera->ProcessMouseScroll(static_cast<float>(yoffset));
}

void Scene::addLight(glm::vec3 pos, glm::vec3 color)
{
	Light l(pos, color);
	this->Lights.push_back(l);
	RT_INFO("Add light success");
}

void Scene::addModel(const string& fileName, ShaderSource& shader)
{
	Models.push_back(Model(fileName, shader));
	hoverModel = &Models[Models.size()-1];
	RT_INFO("Add model success");
}

void Scene::addModel(shared_ptr<Mesh>& mesh, ShaderSource& shader)
{
	Models.push_back(Model(mesh, shader));
	RT_INFO("Add model success");
}

void Scene::addHDRMap(const string& fileName, ShaderSource& hdrShader)
{
	if (hdrMap != nullptr)
	{
		delete hdrMap;
	}
	this->hdrShader = make_shared<Shader>(hdrShader.vertexCode, hdrShader.fragmentCode, hdrShader.geometryCode);
	hdrMap = new HDRMap(fileName);
	RT_INFO("Add HDRMap success");
}

void Scene::addCamera()
{
	if (camera != nullptr)
	{		
		delete camera;
	}
	camera = new Camera();
	RT_INFO("Add camera success");
}

void Scene::initScene()	// Light编码, 创建texbuffer传入gpu
{
	if (!Lights.empty())	// 有光源
	{
		glActiveTexture(GL_TEXTURE0);
		glGenBuffers(1, &LightTexBuffer);
		glBindBuffer(GL_TEXTURE_BUFFER, LightTexBuffer);
		glBufferData(GL_TEXTURE_BUFFER, sizeof(Light) * Lights.size(), &Lights[0], GL_STATIC_DRAW);
		glGenTextures(1, &LightTex);
		glBindTexture(GL_TEXTURE_BUFFER, LightTex);
		glTexBuffer(GL_TEXTURE_BUFFER, GL_RGB32F, LightTexBuffer);
		glBindTexture(GL_TEXTURE_BUFFER, LightTex);
		glActiveTexture(GL_TEXTURE0);
		RT_INFO("Scene.cpp: Light Tex generate success");
	}
	if (hdrMap != nullptr)	// 有HDR天空球
	{
		glGenTextures(1, &HDRTex);
		glBindTexture(GL_TEXTURE_2D, HDRTex);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, hdrMap->hdrRes.width, hdrMap->hdrRes.height, 0, GL_RGB, GL_FLOAT, hdrMap->hdrRes.cols);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, 0);
		RT_INFO("HDR Tex generate success");
	}
	for (int i = 0; i < Models.size(); i++)
	{
		Models[i].initMesh(this->Lights.size(), this->HDRTex, this->LightTex);
	}
	RT_INFO("scene init success");
}

void Scene::drawHDRMap()
{
	glBindTexture(GL_TEXTURE_2D, HDRTex);
	glActiveTexture(GL_TEXTURE0);
	this->hdrShader->use();
	hdrShader->setInt("hdrTexture", 0);
	hdrShader->setVec3("camera.up", camera->Up);
	hdrShader->setVec3("camera.position", camera->Position);
	hdrShader->setVec3("camera.lookat", camera->Position+camera->Front);
	hdrShader->setFloat("camera.ratio", float(HEIGHT) / WIDTH);
	hdrShader->setFloat("camera.fov", camera->Zoom);
	hdrQuad.Draw(hdrShader);
	glClear(GL_DEPTH_BUFFER_BIT);
}


void Scene::drawModel()
{
	// pass2 绘制模型
	if (Models.empty())
	{
		RT_ERROR("Scene models is NULL");
		return;
	}
	for (int i = 0; i < this->Models.size(); i++)
	{
		// 不同的model也要更新不同的
		Models[i].draw(*camera);
	}
}
