#pragma once
#define GL_GLEXT_PROTOTYPES // Needs to be defined for some GL funcs to work.
#include <map>
#include <string>
#include <GL/gl.h>
#include <GL/glu.h>
#include <glm/glm.hpp>

//Degrees to Radians
float radians(float);


//String Funcs
std::string split(std::string, char, size_t);


//Camrea
class Camera
{
	public:
	Camera();
	float movespeed;
	float lookspeed;
	glm::vec3 upVector;
	glm::vec3 posVector;
	glm::vec3 lookVector;
	glm::vec3 TEventVec;
	glm::vec2 REventVec;
	glm::mat4 projectionMatrix;
	glm::mat4 getViewMatrix(void);
	void update(float);
};


//Resources
class modelResource
{
	public:
	int id;
	std::string name;
	std::string filename;
	glm::vec3* vertexData;
	int vertexAmount;
	int vertexSizeBytes;
	bool vboload;
	int vboOffsetIndex;
	int vboOffsetBytes;
	bool hidden;
	glm::mat4 translationMatrix;
	glm::mat4 rotationMatrix;
};


//OBJReader
class OBJReader
{
	public:
	OBJReader(std::string);
	modelResource pushResource(void);
	private:
	bool preprocOBJ(void);
	bool allocMem(void);
	bool allocConvMem(void);
	bool parseOBJ(void);
	bool vboConvert(void);
	bool genResource(void);
	bool releaseMem(void);
	std::string name;
	std::string filename;
	glm::vec3* vertexData;
	glm::vec3* normalData;
	glm::ivec3* indexData;
	glm::vec3* vboData;
	int vertexAmount;
	int normalAmount;
	int indexAmount;
	modelResource obj;
};


//Managers
class ResourceManager 
{
	public:
	ResourceManager();
	bool pullResource(modelResource);
	bool releaseMem(void);
	int resourceAmount;
	modelResource* resArray;
	std::map<std::string, int> resMap;
};

class VRAMManager
{
	public:
	VRAMManager();
	bool genVBO(modelResource*, int);
	bool releaseMem(void);	
	GLuint vboID;
	int vboSizeBytes;
	void* positionStart;
	GLuint positionDim;
	GLuint normalDim;
	void* normalStart;
	GLuint vboStride;
};


//Shader
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
	bool cleanUpShaders(void);
	private:
	GLuint vertexShaderID;
	GLuint fragmentShaderID;
	GLuint programID;
};


//Time
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
