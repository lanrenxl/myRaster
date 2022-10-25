#version 330 core
out vec4 fragColor;
in vec3 worldPos;

uniform samplerCube environmentMap;

const float PI = 3.14159265359;

void main()
{
	vec3 N = normalize(worldPos);

	vec3 irradiance = vec3(0.0);

	// 切线空间
	vec3 up = vec3(0.0, 1.0, 0.0);
	vec3 right = normalize(cross(up, N));
	up = normalize(cross(N, right));

	float sampleDelta = 0.025;
	float nrSamples = 0.0f;
	for (float phi = 0.0; phi < 2.0 * PI; phi += sampleDelta)
	{
		for (float theta = 0.0; theta < 0.5 * PI; theta += sampleDelta)
		{
			// 球面坐标转为向量
			vec3 tangentSample = vec3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
			// 切线空间转世界空间, 三个值代表三个轴上的分量, 所以分别乘上三个坐标轴就可以转换到世界坐标
			vec3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * N;

			irradiance += texture(environmentMap, sampleVec).rgb * cos(theta) * sin(theta);
			nrSamples++;
		}
	}

	irradiance = PI * irradiance * (1.0 / float(nrSamples));
	fragColor = vec4(irradiance, 1.0);
}