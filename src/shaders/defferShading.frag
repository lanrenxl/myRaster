#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform samplerBuffer lights;
uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;
uniform sampler2D gMaterial;

uniform vec3 viewPos;



void main()
{
    // retrieve data from gbuffer
    vec3 fragPos = texture(gPosition, TexCoords).rgb;
    vec3 normal = texture(gNormal, TexCoords).rgb;
    vec3 albedo = texture(gAlbedo, TexCoords).rgb;
    vec3 material = texture(gMaterial, TexCoords).rgb;
    FragColor = vec4(albedo, 1.0);
}
