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
class material
{
	public:
	int id;
	std::string name;
	std::string mtlFilename;
	float Ns;
	glm::vec3 Ka;
	glm::vec3 Kd;
	glm::vec3 Ks;
	glm::vec3 Ke;
	float Ni;
	float d;
	int illum;
};

class faceGroup
{
	public:
	std::string mtlName;
	int smooth;
	int vertexAmount;
	int vertexSizeBytes;
	int vboOffsetIndex;
	int vboOffsetBytes;	
};

class modelResource
{
	public:
	int id;
	std::string name;
	std::string objFilename;
	std::string mtlFilename;
	glm::vec3* vertexData;
	int vertexAmount;
	int vertexSizeBytes;
	faceGroup* groupArr;
	int faceGroupAmount;
	material* materialArr;
	int materialAmount;
	std::map<std::string, int> materialMap;
	bool tobevboloaded;
	bool vboloaded;
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
	bool preprocMTL(void);
	bool allocMem(void);
	bool allocConvMem(void);
	bool parseOBJ(void);
	bool parseMTL(void);
	bool vboConvert(void);
	bool genResource(void);
	bool releaseMem(void);
	std::string name;
	std::string objFilename;
	std::string mtlFilename;
	glm::vec3* vertexData;
	glm::vec3* normalData;
	glm::ivec3* indexData;
	glm::vec3* vboData;
	int vertexAmount;
	int normalAmount;
	int indexAmount;
	faceGroup* groupArr;
	int faceGroupAmount;
	material* materialArr;
	std::map<std::string, int> materialMap;
	int materialAmount;
	modelResource obj;
};


//Manager
class ResourceManager 
{
	public:
	ResourceManager();
	bool pullOBJResources(modelResource);
	bool releaseMem(void);
	int modelAmount;
	modelResource* modelArr;
	std::map<std::string, int> modelMap;
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
