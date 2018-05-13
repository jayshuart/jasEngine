#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <FreeImage.h>

#include "ShaderManager.h"
#include "Object.h"
#include "Camera.h"
#include "Model.h"

#include <iostream>

enum gameState
{
	mainMenu,
	game,
	loading,
	gameOver
};

class Engine
{
public:
	Engine();
	~Engine();

	bool init();
	bool bufferModels();
	bool gameLoop();
	bool useShaders();

	gameState state;

	Object obj;
	Object skull;
	Object logo;
	Camera camera;
	Model model;

private:
	unsigned int buildTextures(char* filename);
	vec3 clickMove();
	void keyMove();
	void fpsControls();
	vec3 getMousePos();
	Object makeBone();

	void showLogo(float lighting);
	void play();
	void loadLevel();
	void printScore();

	unsigned int skullTex;
	unsigned int boneTex;
	unsigned int logoTex;

	int score;
	float timer;
	float ambient;
	vector<Object> bones;
	float currentTime, previousTime, deltaTime;

	GLFWwindow* GLFWwindowPtr;
	ShaderManager shaderMngr;
};

