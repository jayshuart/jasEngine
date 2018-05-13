#version 430

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uv;
layout (location = 2) uniform mat4 world;
layout (location = 3) uniform mat4 camera;
layout (location = 4) in vec3 normal;

out vec2 uvFrag;
out vec3 worldLoc;
out vec3 worldNorm;

void main()
{
	gl_Position = camera * world * vec4(position, 1);
	uvFrag = uv;
	worldNorm = normal;
	worldLoc = gl_Position.xyz;
}