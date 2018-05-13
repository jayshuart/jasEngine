// jasEngine.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Engine.h"



int main()
{
	//declare engine obj
	Engine engine;

	//intialize window
	if (!engine.init())
	{
		//initialization didnt work - return -1 for main
		return -1;
	}

	//buffer mdoels
	if (!engine.bufferModels())
	{
		//buffer didnt work 
		return -1;
	}

	//load shaders
	if (engine.useShaders() == 0)
	{
		//bad load
		return 0;
	}

	//run loop
	engine.gameLoop();


    return 0;
}

