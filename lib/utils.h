#pragma once
#define GL_GLEXT_PROTOTYPES
#include <map>
#include <string>
#include <GL/gl.h>
#include <GL/glu.h>
#include <glm/glm.hpp>

//Degrees to Radians
float radians(float);

std::string split(std::string, char, size_t);

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

class modelResource
{
	public:
	int id;
	std::string name;
	std::string filename;
	glm::vec3* vertexData;
	int vertexAmount;
	int vertexSizeBytes;
	bool tobeVBOLoaded;
	bool vboloaded;
	int vboAddress;
	bool hidden;
	glm::mat4 translationMatrix;
	glm::mat4 rotationMatrix;
};

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

class ResourceManager 
{
	public:
	ResourceManager();
	bool pullResource(modelResource);
	bool releaseMem(void);
	private:
	int resourceCount;
	modelResource* resArray;
	std::map<std::string, int> resMap;
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
	glm::vec3 normal;
};


class Model
{
	public:
	Model();
	bool releaseMem(void);
	GLuint positionDim;
	GLuint colorDim;
	GLuint normalDim;
	GLuint vectorArrayStride;
	void* positionStart;
	void* colorStart;
	void* normalStart;
	void* indexStart;
	Vertex* p_verticesHeap;
	GLuint vertexNum;
	GLuint verticesBytes;
	GLushort* p_indicesHeap;
	GLuint indicesNum;
	GLuint indicesBytes;
};

class Camera
{
	public:
	glm::vec3 upVector;
	glm::vec3 posVector;
	glm::vec3 lookVector;
	glm::mat4 getViewMatrix(void);
};
