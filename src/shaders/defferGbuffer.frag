#version 330 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec3 gAlbedo;
layout (location = 3) out vec3 gMaterial;   // metellic roughness specular

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

#define uniformTex

// material parameters
#ifndef ALBEDO_TEX
    uniform vec3 albedo;
#else
    uniform sampler2D albedo;
#endif
#ifndef AO_TEX
    uniform float ao;
#else
    uniform sampler2D ao;
#endif
#ifndef METALLIC_TEX
    uniform float metallic;
#else
    uniform sampler2D metallic;
#endif
#ifndef ROUGHNESS_TEX
    uniform float roughness;
#else
    uniform sampler2D roughness;
#endif

void main()
{
    // 和漫反射对每个逐片段颜色
#ifndef ALBEDO_TEX
    vec3 _albedo = albedo;
#else
    vec3 _albedo = texture(albedo, TexCoords).rgb;
#endif
    vec3 material = vec3(metallic, roughness, ao);
    // 存储第一个G缓冲纹理中的片段位置向量
    gPosition = FragPos;
    // 同样存储对每个逐片段法线到G缓冲中
    gNormal = normalize(Normal);
    gAlbedo = _albedo;
    gMaterial = material;
}