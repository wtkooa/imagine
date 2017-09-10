#pragma once
#include <string>
#include <GL/gl.h>
#include <GL/glu.h>
#include <glm/glm.hpp>
#include <SDL2/SDL.h>
#include "utils.h"

class Engine
{
	public:
	Engine();
	private:
	bool init(void);
	bool loadResources(void);
	bool run(void);
	bool cleanup(void);
	void render(void);
	void handle_events(void);
	void handle_logic(void);
	void handle_resize(int,int);
	void handle_time(void);
	bool init_shaders(void);
	bool loadOBJ(std::string);
	std::string vertexShaderFile;
	std::string fragmentShaderFile;
	SDL_Window * p_window;
	SDL_GLContext glContext;
	std::string WINDOW_TITLE;
	bool engine_on;
	unsigned short WINDOW_WIDTH;
	unsigned short WINDOW_HEIGHT;
	float FIELD_OF_VIEW;
	float ASPECT_RATIO;
	float Z_NEAR;
	float Z_FAR;
	unsigned int REQUIRED_SDL_MODULES;
	unsigned int SDL_MODE;
	unsigned int ACTIVEBUFFERS;
	bool WIREFRAME;
	bool DEPTHTEST;
	bool CULLFACE;
	glm::vec4 DEFAULT_CLEAR_COLOR;
	GLuint programID;
	GLint transformationMatShaderUniLoc;
	GLint mtwMatShaderUniLoc;
	GLint light0posShaderUniLoc;
	GLint ambientLightShaderUniLoc;
	unsigned long frame_start_time;
	unsigned long frame_end_time;
	float frame_delta;
	float fps;
	GLSL_Compiler compiler;
	glm::vec3 LIGHT0POS;
	glm::vec3 AMBIENTLIGHT;
	Camera eye;
	Time_Gauge measureTime;
	ResourceManager rm;
	VRAMManager vram;
};
