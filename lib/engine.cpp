#define GL_GLEXT_PROTOTYPES //Needs to be defined for some GL funcs to work.
#include <iostream>
#include <map>
#include <string>
#include <GL/gl.h>
#include <GL/glu.h>
#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "engine.h"
#include "utils.h"

Engine::Engine(void)
{
	WINDOW_TITLE = "Imagine Engine";
	WINDOW_WIDTH = 800;
	WINDOW_HEIGHT = 600;
	ASPECT_RATIO = float(WINDOW_WIDTH) / float(WINDOW_HEIGHT);
	FIELD_OF_VIEW = radians(60.0);
	Z_NEAR = 0.1;
	Z_FAR = 30.0;
	REQUIRED_SDL_MODULES = SDL_INIT_EVERYTHING;
	SDL_MODE = SDL_WINDOW_SHOWN|SDL_WINDOW_RESIZABLE|SDL_WINDOW_OPENGL;
	ACTIVEBUFFERS = GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT;
	WIREFRAME = false;
	DEPTHTEST = true;
	CULLFACE = true;
	vertexShaderFile = "lib/glsl/vshader.glsl";
	fragmentShaderFile = "lib/glsl/fshader.glsl";
	DEFAULT_CLEAR_COLOR = glm::vec4(0.0, 0.0, 0.0, 1.0);
	LIGHT0POS = glm::vec3(0.0f, 3.0f, 0.0f);
	AMBIENTLIGHT = glm::vec3(0.1f, 0.1f, 0.1f);
	eye.movespeed = 1.0; //Meters per Second
	eye.lookspeed = radians(0.05); //Degrees per rel movment
	init();
	run();
	cleanup();
}

bool Engine::cleanup(void)
{
	rm.releaseMem();
	vram.releaseMem();
	compiler.cleanUpShaders();
	SDL_Quit();
	return true;
}

bool Engine::init(void)
{
	SDL_Init(REQUIRED_SDL_MODULES);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	p_window = SDL_CreateWindow(WINDOW_TITLE.c_str(),
							  SDL_WINDOWPOS_CENTERED,
							  SDL_WINDOWPOS_CENTERED,
							  WINDOW_WIDTH, WINDOW_HEIGHT,
							  SDL_MODE);
	glContext = SDL_GL_CreateContext(p_window);
	SDL_GL_SetSwapInterval(1);
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	if (WIREFRAME) {glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);}
	if (DEPTHTEST) {glEnable(GL_DEPTH_TEST);}
	if (CULLFACE) {glEnable(GL_CULL_FACE);}
	glClearColor(DEFAULT_CLEAR_COLOR.r,
				 DEFAULT_CLEAR_COLOR.g,
				 DEFAULT_CLEAR_COLOR.b,
				 DEFAULT_CLEAR_COLOR.a);
	glClear(ACTIVEBUFFERS);
	eye.projectionMatrix = glm::perspective(FIELD_OF_VIEW, ASPECT_RATIO, Z_NEAR, Z_FAR);
	SDL_SetWindowGrab(p_window, SDL_TRUE);
	SDL_SetRelativeMouseMode(SDL_TRUE);
	init_shaders();
	glUniform3fv(ambientLightShaderUniLoc, 1, &AMBIENTLIGHT[0]);	
	glUniform3fv(light0posShaderUniLoc, 1, &LIGHT0POS[0]);
	loadResources();
	frame_start_time = SDL_GetPerformanceCounter();
	return true;
}

bool Engine::init_shaders(void)
{
	compiler.compileVertexShader(vertexShaderFile);
	compiler.compileFragmentShader(fragmentShaderFile);
	compiler.linkShaderProgram();
	programID = compiler.getProgramID();
	glUseProgram(programID);
	transformationMatShaderUniLoc = glGetUniformLocation(programID, "transformationMatrix");
	mtwMatShaderUniLoc = glGetUniformLocation(programID, "mtwMatrix");
	light0posShaderUniLoc = glGetUniformLocation(programID, "light0pos");
	ambientLightShaderUniLoc = glGetUniformLocation(programID, "ambientLight");
	return true;
}

bool Engine::loadResources(void)
{
	loadOBJ("data/Suzanne.obj");	
	loadOBJ("data/Cube.obj");
	loadOBJ("data/Windmillpole.obj");
	loadOBJ("data/Windmillblades.obj");
	loadOBJ("data/Ant.obj");
	vram.genVBO(rm.resArray, rm.resourceAmount);
	return true;
}

bool Engine::run(void)
{
	engine_on = true;
	while (engine_on) {
		handle_time();
		handle_events();
		handle_logic();
		render();
	}	
	return true;
}

void Engine::handle_logic(void)
{
	modelResource* res = rm.resArray;
	std::map<std::string, int> resMap = rm.resMap;
	int Suzanne = resMap["Suzanne"];
	int Cube = resMap["Cube"];
	int Windmillpole = resMap["Windmillpole"];
	int Windmillblades = resMap["Windmillblades"];
	int Ant = resMap["Ant"];
	res[Ant].translationMatrix = glm::translate(glm::mat4(), glm::vec3(8.0f, 0.0f, -3.0f));
	res[Ant].rotationMatrix *= glm::rotate(glm::mat4(), radians(0.5), glm::vec3(0.0f, 1.0f, 0.0f));
	res[Windmillpole].rotationMatrix = glm::rotate(glm::mat4(), radians(90), glm::vec3(0.0f, 1.0f, 0.0f));
	res[Windmillpole].translationMatrix = glm::translate(glm::mat4(), glm::vec3(-4.0f, 0.0f, -3.0));
	res[Windmillblades].translationMatrix = glm::translate(glm::mat4(), glm::vec3(-4.0f, 3.6f, -1.8f));
	res[Windmillblades].rotationMatrix *= glm::rotate(glm::mat4(), radians(3), glm::vec3(0.0f, 0.0f, 1.0f));
	res[Cube].translationMatrix = glm::translate(glm::mat4(), glm::vec3(4.0f, 0.0f, -3.0f));
	res[Cube].rotationMatrix *= glm::rotate(glm::mat4(), radians(0.5), glm::vec3(-1.0f, -1.0f, 0.0f));
	res[Suzanne].translationMatrix = glm::translate(glm::mat4(), glm::vec3(0.0f, 0.0f, -3.0f));
	res[Suzanne].rotationMatrix *= glm::rotate(glm::mat4(), radians(0.5), glm::vec3(1.0f, 1.0f, 0.0f));
}

void Engine::render(void)
{
	modelResource* resource = rm.resArray;
	int resourceAmount = rm.resourceAmount;
	if (SDL_GetWindowGrab(p_window) == SDL_TRUE) {eye.update(frame_delta);}
	glClear(ACTIVEBUFFERS);
	glBindBuffer(GL_ARRAY_BUFFER, vram.vboID);
	for (int n = 0; n < resourceAmount; n++)
	{
		if (resource[n].vboload == true)
		{
			glm::mat4 mtwMatrix = resource[n].translationMatrix *
								  resource[n].rotationMatrix;
			glm::mat4 transformationMatrix = eye.projectionMatrix *
											 eye.getViewMatrix() *
											 mtwMatrix;
			glUniformMatrix4fv(mtwMatShaderUniLoc, 1, GL_FALSE, &mtwMatrix[0][0]);
			glUniformMatrix4fv(transformationMatShaderUniLoc, 1, GL_FALSE, &transformationMatrix[0][0]); 
			glDrawArrays(GL_TRIANGLES, resource[n].vboOffsetIndex, resource[n].vertexAmount);
		}
	}
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	SDL_GL_SwapWindow(p_window);
}

void Engine::handle_time(void)
{
	frame_end_time = SDL_GetPerformanceCounter();
	frame_delta = ((frame_end_time - frame_start_time) * 1000.0 ) / SDL_GetPerformanceFrequency();
	frame_start_time = SDL_GetPerformanceCounter();
	fps = 1000.0 / frame_delta;
}

void Engine::handle_events(void)
{
	SDL_Event evnt;
	while (SDL_PollEvent(&evnt))
	{
		switch (evnt.type)
		{
			case SDL_QUIT:
				engine_on = false;
				break;
			case SDL_WINDOWEVENT:
				switch (evnt.window.event)
				{
				case SDL_WINDOWEVENT_RESIZED:
					handle_resize(evnt.window.data1, evnt.window.data2);
					break;
				}
				break;
			case SDL_MOUSEMOTION:
				if (SDL_GetRelativeMouseMode() == SDL_TRUE)
				{
					eye.REventVec.x += evnt.motion.xrel;	
					eye.REventVec.y += evnt.motion.yrel;	
				}
				break;
			case SDL_KEYDOWN:
				switch (evnt.key.keysym.sym)
				{
					case SDLK_ESCAPE:
						engine_on = false;
						break;
					case SDLK_e:
						if (SDL_GetWindowGrab(p_window) == SDL_FALSE) 
						{
							std::cout << "GrabMode On" << std::endl;
							SDL_SetWindowGrab(p_window, SDL_TRUE);
							SDL_SetRelativeMouseMode(SDL_TRUE);
						}
						else
						{
							std::cout << "GrabMode Off" << std::endl;
							SDL_SetWindowGrab(p_window, SDL_FALSE);
							SDL_SetRelativeMouseMode(SDL_FALSE);
						}	
						break;
					case SDLK_w:
						if (!evnt.key.repeat)
						{	
							eye.TEventVec.z += 1.0;
						}
						break;
					case SDLK_s:
						if (!evnt.key.repeat)
						{
							eye.TEventVec.z -= 1.0;
						}
						break;
					case SDLK_d:
						if (!evnt.key.repeat)
						{
							eye.TEventVec.x += 1.0;
						}
						break;
					case SDLK_a:
						if (!evnt.key.repeat)
						{
							eye.TEventVec.x -= 1.0;
						}
						break;
					case SDLK_SPACE:
						if (!evnt.key.repeat)
						{
							eye.TEventVec.y += 1.0;
						}
						break;
					case SDLK_LSHIFT:
						if (!evnt.key.repeat)
						{
							eye.TEventVec.y -= 1.0;
						}
						break;
				}
				break;				
			case SDL_KEYUP:
				switch(evnt.key.keysym.sym)
				{
					case SDLK_w:
							eye.TEventVec.z -= 1.0;
						break;
					case SDLK_s:
							eye.TEventVec.z += 1.0;
						break;
					case SDLK_d:
							eye.TEventVec.x -= 1.0;
						break;
					case SDLK_a:
							eye.TEventVec.x += 1.0;
						break;
					case SDLK_SPACE:
							eye.TEventVec.y -= 1.0;
						break;
					case SDLK_LSHIFT:
							eye.TEventVec.y += 1.0;
						break;
				}
				break;
		}
	}
}

void Engine::handle_resize(int width, int height)
{
	float aspect_ratio = float(width) / float(height);
	SDL_SetWindowSize(p_window, width, height);
	glViewport(0, 0, width, height);
	eye.projectionMatrix = glm::perspective(FIELD_OF_VIEW, aspect_ratio, Z_NEAR, Z_FAR);	
}

bool Engine::loadOBJ(std::string file)
{
	OBJReader obj(file);
	rm.pullResource(obj.pushResource());
}
