#version 430 core

uniform vec3 cameraPosition;
uniform vec3 camera;

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

vec3 triIntersect( in vec3 ro, in vec3 rd, in vec3 v0, in vec3 v1, in vec3 v2 )
{
    vec3 v1v0 = v1 - v0;
    vec3 v2v0 = v2 - v0;
    vec3 rov0 = ro - v0;
    vec3  n = cross( v1v0, v2v0 );
    vec3  q = cross( rov0, rd );
    float d = 1.0/dot( rd, n );
    float u = d*dot( -q, v2v0 );
    float v = d*dot(  q, v1v0 );
    float t = d*dot( -n, rov0 );
    if( u<0.0 || v<0.0 || (u+v)>1.0 ) t = -1.0;
    return vec3( t, u, v );
}

layout(location=0) out vec4 FragColor;

void main()
{
	vec2 resolution = vec2(1920., 1080.);
	vec2 uv = (2. * gl_FragCoord.xy - resolution.xy) / resolution.y;
	vec3 ro = vec3(0., 0., 3.);
	vec3 rd = normalize(vec3(uv, -1.));
	vec3 color = vec3(0., 0., 0.);
	for (int i = 0; i < 100; ++i) {
		vec3 intersection = triIntersect(ro, rd, triangles[i].v0,
			triangles[i].v1, triangles[i].v2);
		if (intersection.x > 0.)
			color = vec3(1., 1., 1.);
	}

	FragColor = vec4(color, 1.0);
}
