#pragma once
#include <glad/glad.h>
#include <memory>
#include "Scene.h"

class DefferedScene : public Scene
{
public:
	GLuint gBuffer, gPosition, gNormal, gAlbedo, gMaterial;
	shared_ptr<Shader> lightingPass, lightBox;
	Quad lightingQuad;
public:
	DefferedScene();
	void init() override;
	void update() override;
	void draw() override;
private:
	void drawHDRMap();
};
