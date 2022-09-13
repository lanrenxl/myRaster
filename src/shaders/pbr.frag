#version 330 core
out vec4 FragColor;
in vec2 TexCoords;
in vec3 WorldPos;
in vec3 Normal;

// lights
uniform int lightNum;
uniform samplerBuffer lights;

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


uniform vec3 camPos;

const float PI = 3.14159265359;
const int lightSize = 2;

struct Light
{
    vec3 position;
    vec3 color;
};

Light getLight(int i)
{
    Light res;
    res.position = texelFetch(lights, i*lightSize).xyz;
    res.color = texelFetch(lights, i*lightSize+1).xyz;
    return res;
}
// ----------------------------------------------------------------------------
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}
// ----------------------------------------------------------------------------
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}
// ----------------------------------------------------------------------------
void main()
{		
    vec3 N = normalize(Normal);
    vec3 V = normalize(camPos - WorldPos);

    // calculate reflectance at normal incidence; if dia-electric (like plastic) use F0 
    // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)    
    vec3 F0 = vec3(0.04); 

#ifndef ALBEDO_TEX
    vec3 _albedo = albedo;
#else
    vec3 _albedo = texture(albedo, TexCoords).rgb/300.0;
#endif

    F0 = mix(F0, _albedo, metallic);

    // reflectance equation
    vec3 Lo = vec3(0.0);
    for(int i = 0; i < lightNum; ++i) 
    {
        // calculate per-light radiance
        Light tempL = getLight(i);
        vec3 L = normalize(tempL.position - WorldPos);
        vec3 H = normalize(V + L);
        float dis = length(tempL.position - WorldPos);
        float attenuation = 1.0 / (dis * dis);
        vec3 radiance = tempL.color * attenuation;

        // Cook-Torrance BRDF
        float NDF = DistributionGGX(N, H, roughness);
        float G   = GeometrySmith(N, V, L, roughness);
        vec3 F    = fresnelSchlick(clamp(dot(H, V), 0.0, 1.0), F0);
        
        vec3 numerator    = NDF * G * F;
        // + 0.0001 to prevent divide by zero
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
        vec3 specular = numerator / denominator;
        
        // kS is equal to Fresnel
        vec3 kS = F;
        // for energy conservation, the diffuse and specular light can't
        // be above 1.0 (unless the surface emits light); to preserve this
        // relationship the diffuse component (kD) should equal 1.0 - kS.
        vec3 kD = vec3(1.0) - kS;
        // multiply kD by the inverse metalness such that only non-metals 
        // have diffuse lighting, or a linear blend if partly metal (pure metals
        // have no diffuse light).
        kD *= 1.0 - metallic;

        // scale light by NdotL
        float NdotL = max(dot(N, L), 0.0);

        // add to outgoing radiance Lo
        Lo += (kD * _albedo / PI + specular) * radiance * NdotL;  // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
    }

    // ambient lighting (note that the next IBL tutorial will replace 
    // this ambient lighting with environment lighting).
    vec3 ambient = vec3(0.03) * _albedo * ao;
    vec3 color = ambient+Lo;
    // HDR tonemapping
    // color = _albedo/255;
    color = color / (color + vec3(2.0));
    // gamma correct
    color = pow(color, vec3(1.0/2.2)); 
    //color = getLight(0).color;
    FragColor = vec4(color*100, 1.0);
}
