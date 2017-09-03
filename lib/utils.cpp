#define GL_GLEXT_PROTOTYPES
#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <exception>
#include <sstream>
#include <GL/gl.h>
#include <GL/glu.h>
#include <glm/glm.hpp>
#include <SDL2/SDL.h>
#include "utils.h"


//Degrees to Radian Convertion
float radians(float degrees) {return degrees * ( M_PI / 180.0);}


//GLSL_Reader reads glsl code from file.
GLSL_Reader::GLSL_Reader() {}
GLSL_Reader::GLSL_Reader(std::string name) {filename = name;}

void GLSL_Reader::setFilename(std::string name) {filename = name;}

std::string GLSL_Reader::getFilename(void) {return filename;}

bool GLSL_Reader::read(void)
{
	std::ifstream glslFile;
	glslFile.open(filename.c_str());
	if (glslFile.is_open()) {
		std::stringstream buffer;
		buffer << glslFile.rdbuf();
		code = buffer.str();
		glslFile.close();
		return true;
	} else {
		std::cout << "File " << filename << " failed to open..." << std::endl;
		return false;
	}
}

std::string GLSL_Reader::getCode(void) {return code;}


//GLSL_Reader compiles glsl code.
bool GLSL_Compiler::compileVertexShader(std::string filename)
{
	GLSL_Reader vertexShaderReader(filename);
	vertexShaderReader.read();
	std::string vertexShaderCode = vertexShaderReader.getCode();
	vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	const char* codeParamAdapter[1]; //glShaderSource needs char**
	codeParamAdapter[0] = vertexShaderCode.c_str();
	glShaderSource(vertexShaderID, 1, codeParamAdapter, 0);
	glCompileShader(vertexShaderID);
}

bool GLSL_Compiler::compileFragmentShader(std::string filename)
{
	GLSL_Reader fragmentShaderReader(filename);
	fragmentShaderReader.read();
	std::string fragmentShaderCode = fragmentShaderReader.getCode();
	fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
	const char* codeParamAdapter[1]; //glShaderSource needs char**
	codeParamAdapter[0] = fragmentShaderCode.c_str();
	glShaderSource(fragmentShaderID, 1, codeParamAdapter, 0);
	glCompileShader(fragmentShaderID);
}

bool GLSL_Compiler::linkShaderProgram(void)
{
	programID = glCreateProgram();
	glAttachShader(programID, vertexShaderID);
	glAttachShader(programID, fragmentShaderID);
	glLinkProgram(programID);
}

GLuint GLSL_Compiler::getProgramID(void) {return programID;}


//Time_Gauge measures code performance.
void Time_Gauge::start(void) {start_time = SDL_GetPerformanceCounter();}

void Time_Gauge::end(void)
{
	end_time = SDL_GetPerformanceCounter();
	time_delta = ((end_time - start_time) * 1000.0) / SDL_GetPerformanceFrequency();
	per_second = 1000.0 / time_delta;
}

void Time_Gauge::gauge(void)
{
	std::cout << time_delta << " msec | " << per_second << " exec per sec" << std::endl;
}


//Geometry Library
Model::Model(void)
{
	Vertex verticesStack[] =
	{
	glm::vec3(+0.5, +0.5, +0.5), //0 Top
	glm::vec3(+1.0, +0.0, +0.0), 
	glm::vec3(+0.5, +0.5, -0.5), //1
	glm::vec3(+0.0, +1.0, +0.0), 
	glm::vec3(-0.5, +0.5, -0.5), //2
	glm::vec3(+0.0, +0.0, +1.0),
	glm::vec3(-0.5, +0.5, +0.5), //3
	glm::vec3(+1.0, +1.0, +1.0),

	glm::vec3(+0.5, -0.5, +0.5), //4 Bottom
	glm::vec3(+1.0, +1.0, +1.0),
	glm::vec3(+0.5, -0.5, -0.5), //5
	glm::vec3(+0.0, +0.0, +1.0),
	glm::vec3(-0.5, -0.5, -0.5), //6
	glm::vec3(+0.0, +1.0, +0.0),
	glm::vec3(-0.5, -0.5, +0.5), //7
	glm::vec3(+1.0, +0.0, +0.0),

	glm::vec3(+0.5, -0.5, -0.5), //8 Back
	glm::vec3(+1.0, +0.0, +1.0),
	glm::vec3(+0.5, +0.5, -0.5), //9
	glm::vec3(+1.0, +1.0, +0.0),
	glm::vec3(-0.5, +0.5, -0.5), //10
	glm::vec3(+0.0, +1.0, +1.0),
	glm::vec3(-0.5, -0.5, -0.5), //11
	glm::vec3(+0.5, +0.0, +0.0),

	glm::vec3(+0.5, -0.5, +0.5), //12 Front
	glm::vec3(+0.0, +0.0, +0.5),
	glm::vec3(+0.5, +0.5, +0.5), //13
	glm::vec3(+0.0, +1.0, +1.0),
	glm::vec3(-0.5, +0.5, +0.5), //14
	glm::vec3(+1.0, +1.0, +0.0),
	glm::vec3(-0.5, -0.5, +0.5), //15
	glm::vec3(+1.0, +0.0, +1.0),

	glm::vec3(-0.5, -0.5, +0.5), //16 Left
	glm::vec3(+0.5, +0.0, +0.5),
	glm::vec3(-0.5, -0.5, -0.5), //17
	glm::vec3(+0.5, +0.5, +0.0),
	glm::vec3(-0.5, +0.5, -0.5), //18
	glm::vec3(+0.0, +0.5, +0.5),
	glm::vec3(-0.5, +0.5, +0.5), //19
	glm::vec3(+0.0, +0.5, +0.5),

	glm::vec3(+0.5, -0.5, +0.5), //20 Right
	glm::vec3(+0.0, +0.5, +0.0),
	glm::vec3(+0.5, -0.5, -0.5), //21
	glm::vec3(+0.0, +0.5, +0.5),
	glm::vec3(+0.5, +0.5, -0.5), //22
	glm::vec3(+0.5, +0.5, +0.0),
	glm::vec3(+0.5, +0.5, +0.5), //23
	glm::vec3(+0.5, +0.0, +0.5)
	};

	GLushort indicesStack[] =
	{
	0,1,2, 0,2,3, //Top
	4,5,6, 4,6,7, //Bottom
	9,10,11, 8,9,11, //Back
	13,14,15, 12,13,15, //Front
	16,18,17, 16,19,18, //Left
	20,21,22, 20,22,23 //Right
	};
	positionDim = 3;
	colorDim = 3;
	vectorArrayStride = sizeof(GLfloat) * 6;
	positionStart = (void*)(0);
	colorStart = (void*)(sizeof(GLfloat) * 3);
	indexStart = (void*)(0);
	vertexNum = 24;
	verticesBytes = 576;
	indicesNum = 36;
	indicesBytes = 72;
	try
	{
	p_verticesHeap = new Vertex[vertexNum];
	memcpy(p_verticesHeap, verticesStack, verticesBytes);
	p_indicesHeap = new GLushort[indicesNum];
	memcpy(p_indicesHeap, indicesStack, indicesBytes);
	}
	catch (std::exception& exc) {std::cout << exc.what() << std::endl;}
}

bool Model::releaseMem(void)
{
	delete [] p_verticesHeap;
	delete [] p_indicesHeap;
	p_verticesHeap = 0;
	p_indicesHeap = 0;
}
