#pragma once
#define GL_GLEXT_PROTOTYPES
#include <string>
#include <GL/gl.h>
#include <GL/glu.h>
#include <glm/glm.hpp>

//Degrees to Radians
float radians(float);


class GLSL_Reader
{
	public:
	GLSL_Reader();
	GLSL_Reader(std::string);
	void setFilename(std::string);
	std::string getFilename(void);
	bool read(void);
	std::string getCode(void);
	private:
	std::string filename;
	std::string code;
};


class GLSL_Compiler
{
	public:
	bool compileVertexShader(std::string);
	bool compileFragmentShader(std::string);
	bool linkShaderProgram(void);
	GLuint getProgramID(void);
	private:
	GLuint vertexShaderID;
	GLuint fragmentShaderID;
	GLuint programID;
};


class Time_Gauge
{
	public:
	void start(void);
	void end(void);
	void gauge(void);
	private:
	unsigned long start_time;
	unsigned long end_time;
	float time_delta;
	float per_second;
};


class Vertex
{
	public:
	glm::vec3 position;
	glm::vec3 color;
};


class Model
{
	public:
	Model();
	~Model();
	GLuint positionDim;
	GLuint colorDim;
	GLuint vectorArrayStride;
	void* positionStart;
	void* colorStart;
	void* indexStart;
	Vertex* p_verticesHeap;
	GLuint vertexNum;
	GLuint verticesBytes;
	GLushort* p_indicesHeap;
	GLuint indicesNum;
	GLuint indicesBytes;
};
