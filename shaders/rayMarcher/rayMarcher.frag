#version 330 core

layout(location=0) out vec4 FragColor;

vec3 s = vec3(0., 1., -1.);

float dot2(vec3 p) {
	return dot(p, p);
}

mat2 rot(float t) {
    float c = cos(t), s = sin(t);
    return mat2(c, -s, s, c);
}

float getSquareAngularMomentum(vec3 position, vec3 velocity) {
	vec3 c = cross(position, velocity);
	return dot(c, c);
}

void movePhoton(inout vec3 position, inout vec3 velocity)
// Reference: https://rantonels.github.io/starless/
{
	// float d = length(position) - .1;
    float timeStep = .1; //* d;
    position += velocity * timeStep;
    vec3 acceleration = -1.5 * getSquareAngularMomentum(position, velocity)
        / pow(dot2(position), 2.5) * position;
    //velocity += acceleration * timeStep;
}

void accumulate(vec3 position, inout vec4 color) {
	//color += .0001 / (.01 + pow(length(position.y), 2.)) / (.01 +.05 * pow(length(position.xz), 2.));
    vec3 q = position;
    q.xz *= .05;
    float r = length(position.xz);
    float th = atan(position.z / position.x);
    //color += .05 / (.001 + pow(5. * length(q), 4.)); // * texture(iChannel0, vec2(.1 * r, .03 * th)).r;
    color.rgb += .003 / (.03 + pow(abs(position.y), 4.) + .0001 * pow(length(position.xz), 5.));
        //* pow(texture(iChannel0, vec2(.05 * r, .01 * th)).rgb, vec3(2.)) * 4.;
}

float sdBox(vec3 p, vec3 b)
// Reference: https://iquilezles.org/articles/distfunctions/
{
    vec3 q = abs(p) - b;
    return length(max(q,0.0)) + min(max(q.x,max(q.y,q.z)),0.0);
}

void main()
{
	vec2 resolution = vec2(1920., 1080.);
	vec2 uv = (2. * gl_FragCoord.xy - resolution.xy) / resolution.y;
	vec3 ro = .7  * vec3(0., 5., 5.);
	vec3 rd = normalize(vec3(uv, -1.));
    // ro.xz *= rot(2. * iMouse.x / iResolution.x);
    // ro.yz *= rot(2. * iMouse.y / iResolution.y);
    rd.yz *= rot(-.7);
    // rd.xz *= rot(2. * iMouse.x / iResolution.x);
    // rd.yz *= rot(2. * iMouse.y / iResolution.y);
	vec3 position = ro;
	vec3 velocity = rd;
	vec4 color = s.xxxy;
	for (float i = 0.; i < 80.; ++i) {
        //float d = sdBox(position, vec3(.5));
        //position += d * rd;
		movePhoton(position, velocity);
		accumulate(position, color);
		float d = length(position) - 1.;
		if (d < 0.)
            break;
	}
	FragColor = 1. - exp(-color);
}
