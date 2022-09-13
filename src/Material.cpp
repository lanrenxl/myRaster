#include "Material.h"
#include <memory>
#include "Log.h"

Material::Material()
{
	albedo = glm::vec3(1.0, 1.0, 1.0);
	ao = 0.5f;
	roughness = 1.0f;
	metallic = 0.5f;
	albedoTex = -1;
	aoTex = -1;
	roughnessTex = -1;
	metallicTex = -1;
	shaderDefine = "";
}

void Material::addAlbedoTex(size_t texIndex)
{
	if (albedoTex == -1)
	{
		this->albedoTex = texIndex;
		this->shaderDefine += "#define ALBEDO_TEX\n";
	}
	else
	{
		RT_WARN("redefine ALBEDO_TEX");
	}
}

void Material::addAoTex(size_t texIndex)
{
	if (aoTex == -1)
	{
		this->aoTex = texIndex;
		this->shaderDefine += "#define AO_TEX";
	}
	else
	{
		RT_WARN("redefine AO_TEX");
	}
}

void Material::addRoughnessTex(size_t texIndex)
{
	if (roughnessTex == -1)
	{
		this->roughnessTex = texIndex;
		this->shaderDefine += "#define ROUGHNESS_TEX";
	}
	else
	{
		RT_WARN("redefine ROUGHNESS_TEX");
	}
}

void Material::addMetallicTex(size_t texIndex)
{
	if (metallicTex == -1)
	{
		this->metallicTex = texIndex;
		this->shaderDefine += "#define METALLIC_TEX";
	}
	else
	{
		RT_WARN("redefine METALLIC_TEX");
	}
}

void Material::setAlbedo(glm::vec3 _albedo)
{
	this->albedo = _albedo;
}

void Material::setAo(float _ao)
{
	this->ao = _ao;
}

void Material::setRoughness(float _roughness)
{
	this->roughness = _roughness;
}

void Material::setMetallic(float _metallic)
{
	this->metallic = _metallic;
}
