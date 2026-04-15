#version 430 core

struct Triangle {
	vec3 v0;
	float pad0;
	vec3 v1;
	float pad1;
	vec3 v2;
	float pad2;
};

layout(std430, binding = 0) buffer TrianglesBuffer {
	Triangle triangles[];
};

layout(location=0) out vec4 FragColor;

void main()
{
	FragColor = vec4(triangles[0].v0.x, 0., 0., 1.0);
}
