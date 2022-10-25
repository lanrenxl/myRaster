#version 330
#define PI 3.1415926
out vec4 outColor;
in vec2 TexCoords;
uniform sampler2D hdrTexture;

struct Camera
{
	vec3 up;
	vec3 position;
	vec3 lookat;
	float ratio;
	float fov;
};

uniform Camera camera;

vec3 get_Ray()
{
	vec2 uv = TexCoords * 2 - vec2(1.0);
	vec3 forward = camera.lookat - camera.position;
	float len_forward = length(forward);
	float width = 2 * tan(radians(camera.fov)) * len_forward;
	float height = width * camera.ratio;
	vec3 right = cross(camera.up, forward);
	vec3 v_up = cross(forward, right);
	vec3 last_lookat = camera.lookat - ((uv.x * width+0.5) / 2 * normalize(right)) + ((uv.y * height+0.5) / 2 * normalize(v_up));
	vec3 dir = normalize(last_lookat - camera.position);
	return dir;
}

vec2 SampleSphericalMap(vec3 v)
{
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv /= vec2(2.0 * PI, PI);
    uv += 0.5;
    uv.y = 1.0 - uv.y;
    return uv;
}

vec3 sampleHdr(vec3 v)
{
    vec2 uv = SampleSphericalMap(normalize(v));
    vec3 color = texture(hdrTexture, uv).xyz;
    return color;
}

void main()
{
	vec3 dir = get_Ray();
	outColor = vec4(sampleHdr(dir), 1.0);
}
