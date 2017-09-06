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
	bool uploadData(void);
	bool run(void);
	bool cleanup(void);
	void draw(void);
	void handle_events(void);
	void handle_resize(int,int);
	void handle_time(void);
	void handle_view_translation(void);
	void handle_view_rotation(void);
	void handle_model_rotation(void);
	bool init_shaders(void);
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
	GLuint vertexBufferID;
	GLuint indexBufferID;
	GLuint programID;
	glm::mat4 translationMatrix;
	glm::vec3 DEFAULT_TRANSLATION;
	glm::mat4 rotationMatrix;
	glm::mat4 viewMatrix;
	glm::mat4 modeltoworldMatrix;
	glm::mat4 modelviewMatrix;
	glm::mat4 projectionMatrix;
	glm::mat4 transformationMatrix;
	GLint transformationMatShaderUniLoc;
	GLint modeltoworldMatShaderUniLoc;
	GLint light0posShaderUniLoc;
	GLint ambientLightShaderUniLoc;
	unsigned long frame_start_time;
	unsigned long frame_end_time;
	float frame_delta;
	float fps;
	Model currentModel;
	GLSL_Compiler compiler;
	glm::vec3 LIGHT0POS;
	glm::vec3 AMBIENTLIGHT;
	Camera eye;
	glm::vec3 transEventVec;
	glm::vec2 rotateEventVec;
	float MOVESPEED;
	float LOOKSPEED;
	float METERS_PER_MSEC;
	Time_Gauge measureTime;
};
