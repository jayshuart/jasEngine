#pragma once
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <FreeImage.h>
#include <glm/gtx/transform.hpp>

#include <sstream>
#include <fstream>
#include <iostream>
#include <string>

using namespace std;
using namespace glm;

struct Vertex 
{
	vec3 location;
	vec2 uv;
	vec3 normal;
};

struct VertInd
{
	unsigned int locInd;
	unsigned int uvInd;
	unsigned int normInd;
};

class Model
{
public:
	Model();
	~Model();

	bool buffer(string objFile);
	void render();

	Vertex vertex;
	VertInd vertInd;

private:
	GLuint vertArr;
	unsigned int vertCount;
};

