#pragma once
#include <string>
#include <HDR/HDRLoader.h>
#include <glm/glm.hpp>
#include "Shader.h"

class HDRMap
{
public:
	HDRMap(const std::string& fileName);

	bool LoadHDRMap(const std::string& fileName);

	HDRLoaderResult hdrRes;
};

class IBL_HDR
{
	using string = std::string;
public:
	HDRMap *hdrMap;
	// IBL部分预处理
	Shader* hdrToCubeShader;			// hdr图像转换为六面体
	Shader* diffuseIrradiansShader;	// diffuse项半球均匀采样预计算
	Shader* specularPrefilterShader;	// specular项按roughness进行mipmap计算
	Shader* specularLUTShader;

	GLuint HDRTex;
	unsigned int precomputeFBO, precomputeRBO;
	unsigned int hdrCubemap;
	unsigned int irradianceCubemap;
	unsigned int prefilterCubemap, brdfLutTex;
public:
	IBL_HDR(const string& path);	// 构造函数
	~IBL_HDR();
	void precompute();
private:
	glm::mat4 captureProjection;
	glm::mat4 captureViews[6];
	void hdrToCubemap();
	void hdrDiffusePrecompute();
	void hdrSpecularPrefilterPrecompute();
	void hdrSpecularLutPrecompute();
private:
	unsigned int cubeVAO = 0;
	unsigned int cubeVBO = 0;
	void renderCube();
};

