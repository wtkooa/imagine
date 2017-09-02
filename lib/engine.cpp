#define GL_GLEXT_PROTOTYPES //Needs to be defined for some GL funcs to work.
#include <iostream>
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
	Z_FAR = 10;
	REQUIRED_SDL_MODULES = SDL_INIT_EVERYTHING;
	SDL_MODE = SDL_WINDOW_SHOWN|SDL_WINDOW_RESIZABLE|SDL_WINDOW_OPENGL;
	ACTIVEBUFFERS = GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT;
	WIREFRAME = false;
	DEPTHTEST = true;
	vertexShaderFile = "lib/glsl/basicvertexShader.glsl";
	fragmentShaderFile = "lib/glsl/basicfragmentShader.glsl";
	DEFAULT_TRANSLATION = glm::vec3(0.0, 0.0, -3.0);
	DEFAULT_CLEAR_COLOR = glm::vec4(0.0, 0.0, 0.0, 1.0);
	init();
}

Engine::~Engine(void)
{
	glDeleteBuffers(1, &indexBufferID);
	glDeleteBuffers(1, &vertexBufferID);
	SDL_Quit();
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
	glClearColor(DEFAULT_CLEAR_COLOR.r,
				 DEFAULT_CLEAR_COLOR.g,
				 DEFAULT_CLEAR_COLOR.b,
				 DEFAULT_CLEAR_COLOR.a);
	glClear(ACTIVEBUFFERS);
	translationMatrix = glm::translate(glm::mat4(), DEFAULT_TRANSLATION);
	projectionMatrix = glm::perspective(FIELD_OF_VIEW, ASPECT_RATIO, Z_NEAR, Z_FAR);
	init_shaders();
	uploadData();
	frame_start_time = SDL_GetPerformanceCounter();
	run();
	return true;
}

bool Engine::init_shaders(void)
{
	GLSL_Compiler compiler;
	compiler.compileVertexShader(vertexShaderFile);
	compiler.compileFragmentShader(fragmentShaderFile);
	compiler.linkShaderProgram();
	programID = compiler.getProgramID();
	glUseProgram(programID);
	transformationMatShaderUniLoc = glGetUniformLocation(programID, "transformationMatrix");
}

bool Engine::uploadData(void)
{
	glGenBuffers(1, &vertexBufferID);
	glGenBuffers(1, &indexBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
	glBufferData(GL_ARRAY_BUFFER,
				 currentModel.verticesBytes,
				 currentModel.p_verticesHeap,
				 GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
				 currentModel.indicesBytes,
				 currentModel.p_indicesHeap,
				 GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, currentModel.positionDim,
						  GL_FLOAT, GL_FALSE,
						  currentModel.vectorArrayStride,
						  currentModel.positionStart);
	glVertexAttribPointer(1, currentModel.colorDim,
						  GL_FLOAT, GL_FALSE,
						  currentModel.vectorArrayStride,
						  currentModel.colorStart);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	return true;
}

bool Engine::run(void)
{
	engine_on = true;
	while (engine_on) {
		handle_time();
		handle_events();
		draw();
	}
	return true;
}

bool Engine::draw(void)
{
	transformationMatrix = glm::mat4();
	handle_rotation();
	transformationMatrix = projectionMatrix * translationMatrix * rotationMatrix;
	glUniformMatrix4fv(transformationMatShaderUniLoc, 1, GL_FALSE,
					   &transformationMatrix[0][0]);
	glClear(ACTIVEBUFFERS);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
	glDrawElements(GL_TRIANGLES, currentModel.indicesNum, GL_UNSIGNED_SHORT, currentModel.indexStart);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	measureTime.start();
	SDL_GL_SwapWindow(p_window);
	measureTime.end();
	return true;
}

bool Engine::handle_time(void)
{
	frame_end_time = SDL_GetPerformanceCounter();
	frame_delta = ((frame_end_time - frame_start_time) * 1000.0 ) / SDL_GetPerformanceFrequency();
	frame_start_time = SDL_GetPerformanceCounter();
	fps = 1000.0 / frame_delta;
	measureTime.gauge();
	return true;
}

bool Engine::handle_rotation(void)
{
	glm::mat4 rotation = glm::rotate(glm::mat4(), radians(0.5f), glm::vec3(1.0, 1.0, 0.0));
	rotationMatrix = rotationMatrix * rotation;
}

bool Engine::handle_events(void)
{
	SDL_Event evnt;
	while (SDL_PollEvent(&evnt)) {
		switch (evnt.type) {
			case SDL_QUIT:
				engine_on = false;
			break;
			case SDL_WINDOWEVENT:
				switch (evnt.window.event) {
				case SDL_WINDOWEVENT_RESIZED:
					handle_resize(evnt.window.data1, evnt.window.data2);
				break;
				}
			break;
			case SDL_KEYDOWN:
				switch (evnt.key.keysym.sym) {
					case SDLK_ESCAPE:
					engine_on = false;
					break;
				}
		}
	}
	return true;
}

bool Engine::handle_resize(int width, int height)
{
	float aspect_ratio = float(width) / float(height);
	SDL_SetWindowSize(p_window, width, height);
	glViewport(0, 0, width, height);
	projectionMatrix = glm::perspective(FIELD_OF_VIEW, aspect_ratio, Z_NEAR, Z_FAR);
	return true;
}
