#version 430
layout (location = 5) uniform vec3 lightLoc;
layout (location = 6) uniform vec3 cameraLoc;
layout (location = 7) uniform float ambient;

in vec2 uvFrag;
in vec3 worldLoc;
in vec3 worldNorm;

uniform sampler2D img;

void main()
{
	vec3 L = normalize(lightLoc - worldLoc);
	vec3 V = normalize(cameraLoc - worldLoc);
	vec3 H = normalize(L + V);
	vec3 N = normalize(worldNorm);

	//float ambient = .9;
	float diffuse = .5 * max(dot(L, N), 0);
	float specular = .5 * pow(max(dot(H, N), 0), 16);
	float bright = ambient + diffuse + specular;

	vec4 textureColour = texture(img, uvFrag);

	gl_FragColor = vec4(bright * textureColour.rgb, textureColour.a);
}