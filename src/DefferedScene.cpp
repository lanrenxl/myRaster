#include "DefferedScene.h"
#include "globel.h"
#include "Log.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

DefferedScene::DefferedScene()
    : gBuffer(-1), gPosition(-1), gNormal(-1), gAlbedo(-1), gMaterial(-1)
{}

void DefferedScene::init()
{
    if (!Lights.empty())	// 有光源
    {
        glGenBuffers(1, &LightTexBuffer);
        glBindBuffer(GL_TEXTURE_BUFFER, LightTexBuffer);
        glBufferData(GL_TEXTURE_BUFFER, sizeof(Light) * Lights.size(), &Lights[0], GL_STATIC_DRAW);
        glGenTextures(1, &LightTex);
        glBindTexture(GL_TEXTURE_BUFFER, LightTex);
        glTexBuffer(GL_TEXTURE_BUFFER, GL_RGB16F, LightTexBuffer);
        RT_INFO("Scene.cpp: Light Tex generate success");
    }
    if (this->ibl_hdr != nullptr)	// 有HDR天空球
    {
        ibl_hdr->precompute();
        RT_INFO("Scene.cpp: HDR Tex generate success");
    }
    {
        glGenFramebuffers(1, &gBuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
        // position color buffer
        glGenTextures(1, &gPosition);
        glBindTexture(GL_TEXTURE_2D, gPosition);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, WIDTH, HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);
        // normal color buffer
        glGenTextures(1, &gNormal);
        glBindTexture(GL_TEXTURE_2D, gNormal);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, WIDTH, HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);
        // color + specular color buffer
        glGenTextures(1, &gAlbedo);
        glBindTexture(GL_TEXTURE_2D, gAlbedo);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, WIDTH, HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedo, 0);
        // material roughness metallic ao
        glGenTextures(1, &gMaterial);
        glBindTexture(GL_TEXTURE_2D, gMaterial);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, WIDTH, HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, gMaterial, 0);
        // tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
        unsigned int attachments[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
        glDrawBuffers(4, attachments);
        // create and attach depth buffer (renderbuffer)
        unsigned int rboDepth;
        glGenRenderbuffers(1, &rboDepth);
        glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, WIDTH, HEIGHT);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
        // finally check if framebuffer is complete
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            RT_ERROR("Framebuffer not complete!");
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    
    for (int i = 0; i < this->Models.size(); i++)
    {
        auto model = Models[i];
        for (int j = 0; j < model.meshes.size(); j++)
        {
            auto mesh = model.meshes[j];
            mesh->init();
        }
    }
    ShaderSource temp = ShaderSource("D:/document/CODE/vs2019/openGL/myRaster/src/shaders/defferShading.vert", "D:/document/CODE/vs2019/openGL/myRaster/src/shaders/defferShading.frag");
    this->lightingPass = make_shared<Shader>(temp);
    this->lightingPass->use();
    this->lightingPass->setInt("lights",    0);
    this->lightingPass->setInt("gPosition", 1);
    this->lightingPass->setInt("gNormal",   2);
    this->lightingPass->setInt("gAlbedo",   3);
    this->lightingPass->setInt("gMaterial", 4);
    RT_INFO("scene init success");
}

void DefferedScene::update()
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

void DefferedScene::draw()
{
    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
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
                mesh->textures[mat.albedoTex].bindGPU(1);
                mesh->shader->setInt("albedo", 1);
            }
            // 遮蔽AO
            if (mat.aoTex == -1)
                mesh->shader->setFloat("ao", mat.ao);
            else
            {
                mesh->textures[mat.aoTex].bindGPU(2);
                mesh->shader->setInt("ao", 2);
            }
            // 粗糙度roughtness
            if (mat.roughnessTex == -1)
                mesh->shader->setFloat("roughness", mat.roughness);
            else
            {
                mesh->textures[mat.roughnessTex].bindGPU(3);
                mesh->shader->setInt("roughness", 3);
            }
            // 金属度metallic
            if (mat.metallicTex == -1)
                mesh->shader->setFloat("metallic", mat.metallic);
            else
            {
                mesh->textures[mat.metallicTex].bindGPU(4);
                mesh->shader->setInt("metallic", 4);
            }
            mesh->draw();
        }
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    this->lightingPass->use();
    this->lightingPass->setVec3("viewPos", camera->Position);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_BUFFER, LightTex);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, gPosition);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, gNormal);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, gAlbedo);
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, gMaterial);
    lightingQuad.Draw(this->lightingPass);

    glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // 写入到默认帧缓冲
    glBlitFramebuffer(
        0, 0, WIDTH, HEIGHT, 0, 0, WIDTH, HEIGHT, GL_DEPTH_BUFFER_BIT, GL_NEAREST
    );
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    drawHDRMap();
}

void DefferedScene::drawHDRMap()
{    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    this->hdrShader->use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, this->ibl_hdr->HDRTex);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, gPosition);
    hdrShader->setInt("hdrTexture", 0);
    hdrShader->setInt("gPosition", 1);
    hdrShader->setVec3("camera.up", camera->Up);
    hdrShader->setVec3("camera.position", camera->Position);
    hdrShader->setVec3("camera.lookat", camera->Position + camera->Front);
    hdrShader->setFloat("camera.ratio", float(HEIGHT) / WIDTH);
    hdrShader->setFloat("camera.fov", camera->Zoom);
    hdrQuad.Draw(this->hdrShader);
}
