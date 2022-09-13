#version 330
#define PI 3.1415926
out vec4 outColor;		// 输出颜色
in vec2 TexCoords;		// 当前像素的uv
uniform sampler2D hdrTexture;	// hdr纹理, 天空球

struct Camera
{
	vec3 up;		// 摄像机向上向量
	vec3 position;	// 摄像机位置坐标
	vec3 lookat;	// 观察点
	float ratio;	// height:width 长宽比
	float fov;		// 视角大小
};

uniform Camera camera;	// 同步外部的相机参数

vec3 get_Ray()	//根据camera属性及uv获取对应的光线
{
	//texcoord从0~1映射到-1~1
	vec2 uv = TexCoords * 2 - vec2(1.0);
	vec3 forward = camera.lookat - camera.position;
	float len_forward = length(forward);
	float width = 2 * tan(radians(camera.fov)) * len_forward;
	float height = width * camera.ratio;
	// glsl里是左手系进行叉乘
	vec3 right = cross(camera.up, forward);
	vec3 v_up = cross(forward, right);
	vec3 last_lookat = camera.lookat - ((uv.x * width+0.5) / 2 * normalize(right)) + ((uv.y * height+0.5) / 2 * normalize(v_up));
	vec3 dir = normalize(last_lookat - camera.position);
	return dir;
}

vec2 SampleSphericalMap(vec3 v)	// 将三维向量 v 转为 HDR map 的纹理坐标 uv
{
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv /= vec2(2.0 * PI, PI);
    uv += 0.5;
    uv.y = 1.0 - uv.y;
    return uv;
}

vec3 sampleHdr(vec3 v)	// 获取 HDR 环境颜色
{
    vec2 uv = SampleSphericalMap(normalize(v));
    vec3 color = texture(hdrTexture, uv).xyz;
    //color = min(color, vec3(10));
    return color;
}

void main()
{
	vec3 dir = get_Ray();
	outColor = vec4(sampleHdr(dir), 1.0);
}
