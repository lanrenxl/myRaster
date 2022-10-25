#include "Scene.h"
#include "Log.h"
#include "Render.h"

Scene::Scene()
	: camera(nullptr), dirty(false), 
	lastX(WIDTH/2.0f), lastY(HEIGHT/2.0f), deltaTime(0.0f), lastFrame(0.0f),
	LightTex(-1), LightTexBuffer(-1), firstMouse(true), hoverModel(nullptr), hdrQuad(Quad())
{
}

Scene::~Scene()
{
	if (this->camera != nullptr)
		delete this->camera;
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

void Scene::addModel(const string& fileName)
{
	Models.push_back(Model(fileName, *this->shader));
	hoverModel = &Models[Models.size()-1];
	RT_INFO("Add model success");
}

void Scene::addHDRMap(const string& fileName)
{
	if (ibl_hdr != nullptr)
	{
		delete ibl_hdr;
	}
	ibl_hdr = new IBL_HDR(fileName);
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

void Scene::setModelShader(shared_ptr<ShaderSource> _shader)
{
	this->shader = _shader;
}

void Scene::setHDRShader(shared_ptr<ShaderSource> _hdrShader)
{
	this->hdrShader = make_shared<Shader>(*_hdrShader);
	hdrShader->use();
	hdrShader->setInt("hdrTexture", 0);
}

void ForwardScene::init()	// Light编码, 创建texbuffer传入gpu
{
	if (!Lights.empty())	// 有光源
	{
		glGenBuffers(1, &LightTexBuffer);
		glBindBuffer(GL_TEXTURE_BUFFER, LightTexBuffer);
		glBufferData(GL_TEXTURE_BUFFER, sizeof(Light) * Lights.size(), &Lights[0], GL_STATIC_DRAW);
		glGenTextures(1, &LightTex);
		glBindTexture(GL_TEXTURE_BUFFER, LightTex);
		glTexBuffer(GL_TEXTURE_BUFFER, GL_RGB32F, LightTexBuffer);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_BUFFER, LightTex);
		RT_INFO("Scene.cpp: Light Tex generate success");
	}
	if (ibl_hdr)
	{
		this->ibl_hdr->precompute();
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_CUBE_MAP, this->ibl_hdr->irradianceCubemap);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_CUBE_MAP, this->ibl_hdr->prefilterCubemap);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, this->ibl_hdr->brdfLutTex);
		RT_INFO("Scene.cpp: HDR precompute generate success");
	}
	for (int i = 0; i < this->Models.size(); i++)
	{
		auto model = Models[i];
		for (int j = 0; j < model.meshes.size(); j++)
		{
			auto mesh = model.meshes[j];
			mesh->shader->use();
			mesh->init();
			mesh->shader->setInt("irradianceMap", 1);
			mesh->shader->setInt("prefilterMap", 2);
			mesh->shader->setInt("lutMap", 3);
		}
	}
	RT_INFO("scene init success");
}

void ForwardScene::update()
{
	for (int i = 0; i < this->Models.size(); i++)
	{
		auto model = Models[i];
		for (int j = 0; j < model.meshes.size(); j++)
		{
			auto mesh = model.meshes[j];
			mesh->update();
		}
	}
}

void ForwardScene::drawHDRMap()
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, this->ibl_hdr->HDRTex);
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

void ForwardScene::drawModel()
{
	// pass2 绘制模型
	if (Models.empty())
	{
		RT_ERROR("Scene models is NULL");
		return;
	}
	for (int i = 0; i < this->Models.size(); i++)
	{
		auto model = Models[i];
		for (int j = 0; j < model.meshes.size(); j++)
		{
			auto mesh = model.meshes[j];
			mesh->shader->use();
			Material& mat = mesh->materials;
			// 设置材质信息
			// 反射率颜色albedo
			if (mat.albedoTex == -1)
				mesh->shader->setVec3("albedo", mat.albedo);
			else
			{
				// 初始化纹理单元
				mesh->textures[mat.albedoTex].bindGPU(4);
				mesh->shader->setInt("albedo", 4);
			}
			// 遮蔽AO
			if (mat.aoTex == -1)
				mesh->shader->setFloat("ao", mat.ao);
			else
			{
				mesh->textures[mat.aoTex].bindGPU(5);
				mesh->shader->setInt("ao", 5);
			}
			// 粗糙度roughtness
			if (mat.roughnessTex == -1)
				mesh->shader->setFloat("roughness", mat.roughness);
			else
			{
				mesh->textures[mat.roughnessTex].bindGPU(6);
				mesh->shader->setInt("roughness", 6);
			}
			// 金属度metallic
			if (mat.metallicTex == -1)
				mesh->shader->setFloat("metallic", mat.metallic);
			else
			{
				mesh->textures[mat.metallicTex].bindGPU(7);
				mesh->shader->setInt("metallic", 7);
			}
			mesh->draw();
		}
	}
}

void ForwardScene::draw()
{
	this->drawModel();
	this->drawHDRMap();
}
