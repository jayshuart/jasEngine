#include "stdafx.h"
#include "Engine.h"

//INPUT MNGR NAMESPACE ====================================================================================================
namespace 
{
	map<int, bool> keyIsDown;
	map<int, bool> keyWasDown;

	void mouseClick(GLFWwindow * windowPtr, int button, int action, int mods)
	{
		keyIsDown[button] = action;
	}

	void keyCallBack(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		keyIsDown[key] = action;
	}

	static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
	{
	}
}

//CON DCON=================================================================================================================
Engine::Engine()
{
	//build skull object
	Transform skullTrans =
	{
		vec3(0, 0, 0), //location
		vec3(0, 0, 0), //rotation
		vec3(.1, .1, .1) //size
		//world matrix - set in objs constructor
	};

	RigidBody rigid = 
	{
		vec3(0, 0, 0), //velocity
		vec3(0, 0, 0), //force
		1 //mass
	};

	skull = Object(skullTrans, "textures/skull.png", rigid, box);

	//build camera
	Transform camTrans =
	{
		vec3(0, 0, 2), //location
		vec3(0, 0, 0), //rotation
		vec3(1, 1, 1) //size
	};

	//build logo object
	Transform logoTrans =
	{
		vec3(0, 0, 0), //location
		vec3(0, 0, 0), //rotation
		vec3(.7, .7, .7) //size
						 //world matrix - set in objs constructor
	};

	logo = Object(logoTrans, "textures/gjg_logo_v1.png", rigid, box);

	//build cam
	camera = Camera(camTrans, rigid);

	//build model class
	model = Model();

	//set inital game state
	state = mainMenu;
}
Engine::~Engine()
{
}

//METHODS ================================================================================================================
bool Engine::init()
{
	//intialize the window library
	if (glfwInit() == GL_FALSE)
	{
		return false;
	}

	//create a windowed mode window
	GLFWwindowPtr = glfwCreateWindow(800, 600, "Joel Shuart DSA1 Engine", NULL, NULL);

	//make it the currently active window, or quit
	if (GLFWwindowPtr != nullptr)
	{
		glfwMakeContextCurrent(GLFWwindowPtr);
	}
	else
	{
		glfwTerminate();
		return false;
	}

	//initialize GLEW or quit
	if (glewInit() != GLEW_OK)
	{
		glfwTerminate();
		return false;
	}

	//engage wire frame mode
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	//everything ran ok
	return true;
}

bool Engine::bufferModels()
{
	if (model.buffer("models/box.obj")) //try buffering mdoel
	{
		return true;
	}
	else
	{
		return false; //buffer failed
	}
}

bool Engine::gameLoop()
{
	//get textures - save ids
	skullTex = buildTextures(skull.textureFilename);
	boneTex = buildTextures("textures/bone.png");
	logoTex = buildTextures("textures/gjg_logo_v1.png");

	//make textures have alpha
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//set callbacks
	glfwSetKeyCallback(GLFWwindowPtr, keyCallBack);
	glfwSetMouseButtonCallback(GLFWwindowPtr, mouseClick);

	//set world matrix for player
	skull.transform.worldMatrix = translate(skull.transform.location) * yawPitchRoll(skull.transform.rotation.y, skull.transform.rotation.x, skull.transform.rotation.z) * scale(skull.transform.size);

	//setup time variables
	currentTime = 0;
	previousTime = 0;
	deltaTime = 0;

	float timer = 750;

	//set score variables
	score = 0;

	//set lighting values
	ambient = .9f;
	float lightingShift = 0;

	//loop until the user closes the window
	while (!glfwWindowShouldClose(GLFWwindowPtr))
	{
		//update physical sim --------------------------------------------------------------------------------------------------

		//update time
		previousTime = currentTime; //save the last runs time
		currentTime = glfwGetTime(); //get new time
		deltaTime += currentTime + previousTime; //save totalt ime taken

		//load bones
		loadLevel();

		keyMove(); //move based on wasd keys
		//fpsControls();

		//check collisions between skull and bones
		for (int i = 0; i < 1; i++)
		{
			if (skull.collidesWith(bones[i]))
			{
				//randomize position
				float x = rand() % 800 - 400;
				float y = rand() % 600 - 300;

				//convert positions to percent
				x = x / 800;
				y = y / 600;

				//move bone
				bones[i].transform.location = vec3(x, y, bones[i].transform.location.z);

				//up score
				score++;

				//rotate cam bc i can
				float rot = rand() % 10;
				rot = rot / 10; //turn into percent
				camera.transform.rotation.z += rot;
			}
		}

		//updaet cam
		camera.update();
		
		//reset velocity
		skull.rigidBody.velocity = { 0, 0, 0 };

		//update world matrix
		skull.transform.worldMatrix = translate(skull.transform.location) * yawPitchRoll(skull.transform.rotation.y, skull.transform.rotation.x, skull.transform.rotation.z) * scale(skull.transform.size);
		bones[0].transform.worldMatrix = translate(bones[0].transform.location) * yawPitchRoll(bones[0].transform.rotation.y, bones[0].transform.rotation.x, bones[0].transform.rotation.z) * scale(bones[0].transform.size);
		logo.transform.worldMatrix = translate(logo.transform.location) * yawPitchRoll(logo.transform.rotation.y, logo.transform.rotation.x, logo.transform.rotation.z) * scale(logo.transform.size);

		//clear canvas
		glClearColor(.392f, .584f, .929f, 1.0f); //cornflower blue
		//glClearColor(0, 0, 0, 1.0f); //black
		glEnable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //clear canvas

		//upload camera matrix to the vshader
		glUniformMatrix4fv(3, 1, GL_FALSE, &camera.camMatrix[0][0]);

		vec3 lightLoc = vec3(0, 2, 0);
		glUniform3f(5, lightLoc.x, lightLoc.y, lightLoc.z); //lightLoc to fshader

		glUniform3f(6, camera.transform.location.x, camera.transform.location.y, camera.transform.location.z); //cameraLoc to fshader

		glUniform1f(7, ambient); //push ambient lighting to fshader
		//draw buffered models --------------------------------------------------------------------------------------------------
		if (state == mainMenu)
		{
			//draw logo
			showLogo(lightingShift);

			//check if we need to chaneg states
			if (deltaTime > timer)
			{
				state = game;
			}
			else
			{
				//chekc light level
				if (ambient < 1)
				{
					//we need to up lighting
					lightingShift += .005;
				}
			}
		}
		else if (state == game)
		{
			//draw game pieces
			play();
		}
		
		
		//swap the screen display and gl draws to buffers
		glfwSwapBuffers(GLFWwindowPtr);

		//process input/window events -------------------------------------------------------------------------------------------

		//process queued windoe, mouse & keyboard callback events ---------------------------------------------------------------
		keyWasDown = keyIsDown; //update key input
		glfwPollEvents();

		//use escape to close window
		if (keyIsDown[GLFW_KEY_ESCAPE] && keyWasDown[GLFW_KEY_ESCAPE] == false)
		{
			glfwSetWindowShouldClose(GLFWwindowPtr, GL_TRUE);
		}

		////////////////////////////////////////////////////////////////////////DEBUG LINES
		//std::cout << "(" << skull.transform.location.x << ", " << skull.transform.location.y << ", " << skull.transform.location.x << ")" << std::endl;
		//std::cout << "(" << skull.rigidBody.velocity.x << ", " << skull.rigidBody.velocity.y << ", " << skull.rigidBody.velocity.z << ")" << std::endl;
	}

	//delet textures
	glDeleteTextures(1, &skullTex);
	glDeleteTextures(1, &boneTex);

	//exit game loop
	glfwTerminate();
	return true;
}

bool Engine::useShaders()
{
	//load shaders
	bool result = shaderMngr.loadShaders("shaders/vShader.glsl", "shaders/fShader.glsl");

	//check if loaded
	if (result != 0)//not zero, loaded properly
	{
		//use the program
		glUseProgram(shaderMngr.getProgram());

		//return 1 for sucessful run
		return 1;

	}
	else
	{
		//bad load
		return 0;
	}
}

unsigned int Engine::buildTextures(char* filename)
{
	//texture id to be returned at end
	unsigned int id;
	//load textures
	FIBITMAP* imagePtr = FreeImage_Load(FreeImage_GetFileType(filename, 0), filename);
	
	//check if image was loaded properly
	if (imagePtr == nullptr)
	{
		//print error
		cout << "ERROR: Image(" << filename << ") failed to load." << endl;
	}

	//convert to 32bit
	FIBITMAP* image32 = FreeImage_ConvertTo32Bits(imagePtr);

	//check if image was converted properly
	if (image32 == nullptr)
	{
		//print error
		cout << "ERROR: Image(" << filename << ") failed to convert to 32bit." << endl;
	}

	//clear texture from ram
	FreeImage_Unload(imagePtr);
	
	//generate and bind new texture
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);
	

	//upload texture bytes
	glTexImage2D(GL_TEXTURE_2D, 
		0, 
		GL_SRGB_ALPHA, 
		FreeImage_GetWidth(image32), 
		FreeImage_GetHeight(image32), 
		0, 
		GL_BGRA, 
		GL_UNSIGNED_BYTE, 
		(void*)FreeImage_GetBits(image32));

	//set min filter to linear instead of mipap linear
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//unbind texture
	glBindTexture(GL_TEXTURE_2D, 0);

	//return texture id
	return id;
}

//INPUT METHODS =========================================================================================================
vec3 Engine::clickMove()
{
	//preset vec3 to be be applied
	vec3 steering = { 0, 0, 0 }; //no force currently

	//mouse1 click input - move towards mouse
	if (keyIsDown[GLFW_MOUSE_BUTTON_1] && keyWasDown[GLFW_MOUSE_BUTTON_1] == false)
	{
		//get desired velocity
		vec3 desired =
		{
			getMousePos().x - skull.transform.location.x,
			getMousePos().y - skull.transform.location.y,
			getMousePos().z - skull.transform.location.z
		};

		//get steering force
		steering =
		{
			desired.x - skull.rigidBody.velocity.x,
			-(desired.y - skull.rigidBody.velocity.y),
			desired.z - skull.rigidBody.velocity.z
		};
	}

	//return force
	return steering;
}
void Engine::keyMove()
{
	//left
	if (keyIsDown[GLFW_KEY_A] && keyWasDown[GLFW_KEY_A])
	{
		skull.applyForce(vec3(-.005, 0, 0));
	}

	//right
	if (keyIsDown[GLFW_KEY_D] && keyWasDown[GLFW_KEY_D])
	{
		skull.applyForce(vec3(.005, 0, 0));
	}

	//up
	if (keyIsDown[GLFW_KEY_W] && keyWasDown[GLFW_KEY_W])
	{
		skull.applyForce(vec3(0, .005, 0));
	}

	//down
	if (keyIsDown[GLFW_KEY_S] && keyWasDown[GLFW_KEY_S])
	{
		skull.applyForce(vec3(0, -.005, 0));
	}
	
}
void Engine::fpsControls()
{
	// Turn with mouse
	float sens = .005;
	int w = 800, h = 600;
	double x, y;
	glfwGetCursorPos(GLFWwindowPtr, &x, &y);
	camera.transform.rotation.y -= sens * (x - w * .5f); // Yaw
	camera.transform.rotation.x -= sens * (y - h * .5f); // Pitch
	camera.transform.rotation.x = clamp(camera.transform.rotation.x, (-.5f * 3.1459f), (.5f * 3.1459f));
	glfwSetCursorPos(GLFWwindowPtr, w * .5f, h * .5f);

	// Move with arrows
	float speed = .001f;
	vec3 camVel;
	mat3 R = (mat3)glm::yawPitchRoll(camera.transform.rotation.y, camera.transform.rotation.x, camera.transform.rotation.z);
	if (keyIsDown[GLFW_KEY_LEFT] && keyWasDown[GLFW_KEY_LEFT])
	{
		camVel = R * vec3(-1, 0, 0);

		if (camVel != vec3())
			camVel = glm::normalize(camVel) * speed;

		//apply force
		camera.applyForce(camVel);
	}
		
	if (keyIsDown[GLFW_KEY_RIGHT] && keyWasDown[GLFW_KEY_RIGHT])
	{
		camVel = R * vec3(1, 0, 0);

		if (camVel != vec3())
			camVel = glm::normalize(camVel) * speed;

		//apply force
		camera.applyForce(camVel);
	}
		
	if (keyIsDown[GLFW_KEY_UP] && keyWasDown[GLFW_KEY_UP])
	{
		camVel = R * vec3(0, 0, -1);

		if (camVel != vec3())
			camVel = glm::normalize(camVel) * speed;

		//apply force
		camera.applyForce(camVel);
	}
		
	if (keyIsDown[GLFW_KEY_DOWN] && keyWasDown[GLFW_KEY_DOWN])
	{
		camVel = R * vec3(0, 0, 1);

		if (camVel != vec3())
			camVel = glm::normalize(camVel) * speed;

		//apply force
		camera.applyForce(camVel);
	}
		
	
	
}

//HELPER METHODS =========================================================================================================
vec3 Engine::getMousePos()
{
	// Get mouse position
	double xpos, ypos;
	glfwGetCursorPos(GLFWwindowPtr, &xpos, &ypos);

	//adjust mouse pos to act like it does for the shaders
	xpos = xpos / 800; //make % of screen
	ypos = ypos / 600;

	xpos -= .5; //center
	ypos -= .5;

	//return as vec3
	return vec3(xpos, ypos, 0);
}
Object Engine::makeBone()
{
	//randomize position
	float x = rand() % 800 - 400;
	float y = rand() % 600 - 300;

	//convert positions to percent
	x = x / 800;
	y = y / 600;


	//build skull object
	Transform trans =
	{
		vec3(x, y, 0), //location
		vec3(0, 0, 0), //rotation
		vec3(.1, .1, .1) //size
						 //world matrix - built later
	};

	RigidBody rigid =
	{
		vec3(0, 0, 0), //velocity
		vec3(0, 0, 0), //force
		1 //mass
	};

	//make obj
	Object bone = Object(trans, "textures/bone.png", rigid, box);

	//return it
	return bone;
}

//STATE METHODS ==========================================================================================================
void Engine::showLogo(float lighting)
{
	//set initial lighting
	ambient = lighting;

	//render game objs
	glUniformMatrix4fv(2, 1, GL_FALSE, &logo.transform.worldMatrix[0][0]);
	glBindTexture(GL_TEXTURE_2D, logoTex); //logo
	model.render();

}
void Engine::play()
{
	//render game objs
	glUniformMatrix4fv(2, 1, GL_FALSE, &skull.transform.worldMatrix[0][0]);
	glBindTexture(GL_TEXTURE_2D, skullTex); //skull
	model.render();

	for (int i = 0; i < 1; i++)
	{
		glUniformMatrix4fv(2, 1, GL_FALSE, &bones[i].transform.worldMatrix[0][0]);
		glBindTexture(GL_TEXTURE_2D, boneTex); //skull
		model.render();
	}
}
void Engine::loadLevel()
{
	//make bones
	for (int i = 0; i < 1; i++)
	{
		//make a bone in random position - add it to the vector
		bones.push_back(makeBone());

		//set its world matrix
		bones[i].transform.worldMatrix = translate(bones[i].transform.location) * yawPitchRoll(bones[i].transform.rotation.y, bones[i].transform.rotation.x, bones[i].transform.rotation.z) * scale(bones[i].transform.size);
	}
}
void Engine::printScore()
{
	cout << "You collected " << score << " bones!" << endl;
}




