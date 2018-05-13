#include "stdafx.h"
#include "ShaderManager.h"

//CON DCON==============================================================================================================
ShaderManager::ShaderManager()
{
	//intilaize program
	program = 0;
}

ShaderManager::~ShaderManager()
{
}

//GET SET===============================================================================================================
GLuint ShaderManager::getProgram() const
{
	return program;
}


//METHODS ==============================================================================================================
bool ShaderManager::loadShaders(const char * vertexFile, const char * fragmentFile)
{
	//load shaders - save results
	GLuint vShader = loadShader(vertexFile, GL_VERTEX_SHADER);
	GLuint fShader = loadShader(fragmentFile, GL_FRAGMENT_SHADER);

	//check for errors laoding shades
	if (vShader == 0 || fShader == 0)
	{
		//errors - cant do anythign more
		return 0;
	}

	//create shader program
	program = glCreateProgram();

	//attach shaders to it
	glAttachShader(program, vShader);
	glAttachShader(program, fShader);

	//link program
	glLinkProgram(program);

	//check if linked properly
	GLint result;
	glGetProgramiv(program, GL_LINK_STATUS, &result);
	if (result == GL_TRUE)
	{
		//linked properly! - return index of linked program
		return program;
	}
	else
	{
		//error  - get log and print
		GLint size = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &size);

		GLchar* log = "";
		glGetProgramInfoLog(program, size, 0, log);

		//print log
		cout << "ERROR IN LINKING PROGRAM: " << log << endl;

		//clean up
		glDeleteProgram(program);
		delete[] log;
		return 0;

	}
}

GLuint ShaderManager::loadShader(const char * file, GLenum shaderType)
{
	//create input stream
	ifstream inFile;
	cout << file << endl;
	inFile.open(file, ios::binary); //file to be read in (as binary)

	//check if stream is open
	char* fileContents;
	int length;
	if (inFile.is_open())
	{
		//figure out file size
		inFile.seekg(0, ios::end); //seek to end
		length = (int)inFile.tellg(); //save length
		inFile.seekg(0, ios::beg); //go back to begining

		//create array to hold data
		fileContents = new char[length + 1];
		inFile.read(fileContents, length); //fill with data from stream
		fileContents[length] = 0;

		//close stream
		inFile.close();
	}
	else
	{
		//error opening file
		cout << "ERROR OPENING FILE: " << file << endl;
		return 0;
	}

	//create vertex shader
	GLuint index;
	index = glCreateShader(shaderType);

	//set source code
	glShaderSource(index, 1, &fileContents, 0);

	//compile shader
	glCompileShader(index);

	//delete array of code we read in, its no longer needed
	delete[] fileContents;

	//determine if the shader compiled
	GLint results = 0;
	glGetShaderiv(index, GL_COMPILE_STATUS, &results);
	
	if (results == GL_TRUE)
	{
		//sahder compiled sucessfully 
		return index;
	}
	else
	{
		//error in compile - get info log
		GLint logSize = 0;
		glGetShaderiv(index, GL_INFO_LOG_LENGTH, &logSize);
		GLchar* log = new GLchar[logSize];
		glGetShaderInfoLog(index, logSize, 0, log);

		//print log
		cout << "ERROR IN COMPILING SHADER: " << log << endl;

		//clean up
		glDeleteShader(program);
		delete[] log;
		return 0;
	}
}
