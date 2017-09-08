#define GL_GLEXT_PROTOTYPES
#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <cmath>
#include <cstring>
#include <exception>
#include <sstream>
#include <GL/gl.h>
#include <GL/glu.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <SDL2/SDL.h>
#include "utils.h"


//Math
float radians(float degrees) {return degrees * (M_PI / 180.0);}

//String Funcs
std::string split(std::string str, char delim, size_t tokenNum)
{
	std::string substr;	
	size_t foundtoken = -1;
	size_t findlen = 0;
	size_t findstart = 0;	
	do
	{
		findlen = str.find(delim, findstart) - findstart;
		substr = str.substr(findstart, findlen);
		findstart = str.find(delim, findstart) + 1;	
		foundtoken++;
	}
	while (foundtoken < tokenNum);
	return substr;
}

//Camera
glm::mat4 Camera::getViewMatrix(void)
{
	return glm::lookAt(posVector, (lookVector+posVector), upVector);
}

//OBJReader
OBJReader::OBJReader(std::string name)
{
	filename = name;	
	vertexAmount = 0;
	normalAmount = 0;
	indexAmount = 0;
	preprocOBJ();
	allocMem();
	parseOBJ();
	allocConvMem();
	vboConvert();
	genResource();
}

bool OBJReader::preprocOBJ(void)
{
	std::ifstream OBJFile;
	OBJFile.open(filename.c_str());
	if(OBJFile.is_open())
	{
		std::cout << "PreProc " << filename << std::endl;	
		std::string line;
		while (getline(OBJFile, line))
		{
			std::string command = split(line, ' ', 0);
			if (command == "v") {vertexAmount++;}
			else if (command == "vn") {normalAmount++;}
			else if (command == "f") {indexAmount += 3;}
		}
	}
	else
	{
		std::cout << "File " << filename << " failed to open for preproc..." << std::endl;
		return false;
	}
	OBJFile.close();
	return true;
}

bool OBJReader::allocMem(void)
{
	try
	{
		vertexData = new glm::vec3[vertexAmount];
		normalData = new glm::vec3[normalAmount];
		indexData = new glm::ivec3[indexAmount];	
	}
	catch (std::exception& exc)
	{
		std::cout << exc.what() << std::endl;
		return false;
	}
	return true;
}

bool OBJReader::releaseMem(void)
{
	delete [] vertexData;
	delete [] normalData;
	delete [] indexData;
	return true;
}

bool OBJReader::parseOBJ(void)
{
	std::ifstream OBJFile;
	OBJFile.open(filename.c_str());
	if (OBJFile.is_open())
	{
		std::cout << "Parsing " << filename << std::endl;
		size_t vOffset = 0;
		size_t nOffset = 0;
		size_t iOffset = 0;
		std::string line;
		while (getline(OBJFile, line))
		{
			std::string command = split(line, ' ', 0);
			if (command == "o")
			{
				name = split(line, ' ', 1);
			}
			else if (command == "v")
			{
				float x = stof(split(line, ' ', 1));
				float y = stof(split(line, ' ', 2));
				float z = stof(split(line, ' ', 3));	
				*(vertexData + vOffset) = glm::vec3(x,y,z);
				vOffset++;
			}
			else if (command == "vn")
			{
				float x = stof(split(line, ' ', 1));
				float y = stof(split(line, ' ', 2));
				float z = stof(split(line, ' ', 3));
				*(normalData + nOffset) = glm::vec3(x,y,z);
				nOffset++;	
			}
			else if (command == "f")
			{
				for (int i = 1; i < 4; i++)
				{
					std::string subline = split(line, ' ', i);
					int v = stoi(split(subline, '/', 0));
					std::string str_t = split(subline, '/', 1);
					if (str_t == "") {str_t = "0";}
					int t = stoi(str_t);	
					int n = stoi(split(subline, '/', 2));
					*(indexData + iOffset) = glm::ivec3(v,t,n);
					iOffset++;
				}	
			}
		}
	}
	else
	{
		std::cout << "File " << filename << " failed to open for parsing..." << std::endl;
		return false;
	}	
	OBJFile.close();
	return true;
}

bool OBJReader::allocConvMem(void)
{
	try
	{
		vboData = new glm::vec3[indexAmount * 2];
	}
	catch (std::exception& exc)
	{
		std::cout << exc.what() << std::endl;
		return false;
	}
	return true;
}

bool OBJReader::vboConvert(void)
{
	size_t vboOffset = 0;	
	size_t vOffset = 0;
	size_t nOffset = 0;
	for (int offset = 0; offset < indexAmount; offset++)
	{
		glm::ivec3 index = indexData[offset];
		vOffset = index[0] - 1;
		nOffset = index[2] - 1;
		vboData[vboOffset] = vertexData[vOffset];
		vboOffset++;
		vboData[vboOffset] = normalData[nOffset];	
		vboOffset++;
	}
	return true;
}

bool OBJReader::genResource(void)
{
	obj.name = name;
	obj.filename = filename;
	obj.vertexData = vboData;
	obj.vertexAmount = indexAmount;
	obj.vertexSizeBytes = indexAmount * 2 * sizeof(glm::vec3);
	obj.tobeVBOLoaded = true;
	obj.vboloaded = false;
	obj.hidden = false;
	obj.translationMatrix = glm::mat4();
	obj.rotationMatrix = glm::mat4(); 
	releaseMem();
	return true;
}

modelResource OBJReader::pushResource(void) {return obj;}

//Resource Manager
ResourceManager::ResourceManager(void)
{
	resourceCount = 0;
	resArray = new modelResource[resourceCount];
}

bool ResourceManager::pullResource(modelResource res)
{
	res.id = resourceCount;
	resMap[res.name] = res.id;
	resourceCount++;
	modelResource* oldArray = resArray;
	resArray = new modelResource[resourceCount];
	std::memcpy(resArray, oldArray, (resourceCount) * sizeof(resourceCount));
	resArray[res.id] = res;
}

bool ResourceManager::releaseMem(void)
{
	for (int i = 0; i < resourceCount; i++)
	{
		delete [] resArray[i].vertexData;
	}
	delete [] resArray;
}

//Shader Stuff
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
		GLint compileStatus;
	glGetShaderiv(vertexShaderID, GL_COMPILE_STATUS, &compileStatus);
	if (compileStatus != GL_TRUE)
	{
		GLint infoLogLength;
		glGetShaderiv(vertexShaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
		try	
		{
			GLchar* buffer = new GLchar[infoLogLength];
			GLsizei bufferSize; //Not used: required as func param
			glGetShaderInfoLog(vertexShaderID, infoLogLength, &bufferSize, buffer);
			std::cout << "Vertex Shader Compile Error: " << buffer << std::endl;
			delete [] buffer;
		}
		catch (std::exception& exc) {std::cout << exc.what() << std::endl;}
	}
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
	GLint compileStatus;
	glGetShaderiv(fragmentShaderID, GL_COMPILE_STATUS, &compileStatus);
	if (compileStatus != GL_TRUE)
	{
		GLint infoLogLength;
		glGetShaderiv(fragmentShaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
		try	
		{
			GLchar* buffer = new GLchar[infoLogLength];
			GLsizei bufferSize; //Not used: required as func param
			glGetShaderInfoLog(fragmentShaderID, infoLogLength, &bufferSize, buffer);
			std::cout << "Fragment Shader Compile Error: " << buffer << std::endl;
			delete [] buffer;
		}
		catch (std::exception& exc) {std::cout << exc.what() << std::endl;}
	}
}

bool GLSL_Compiler::linkShaderProgram(void)
{
	programID = glCreateProgram();
	glAttachShader(programID, vertexShaderID);
	glAttachShader(programID, fragmentShaderID);
	glLinkProgram(programID);
	GLint linkStatus;
	glGetProgramiv(programID, GL_LINK_STATUS, &linkStatus);
	if (linkStatus != GL_TRUE)
	{
		GLint infoLogLength;
		glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &infoLogLength);
		try
		{
			GLchar* buffer = new GLchar[infoLogLength];
			GLsizei bufferSize; //Not used: required as func param
			glGetProgramInfoLog(programID, infoLogLength, &bufferSize, buffer);
			std::cout << "Shader Program Link Error: " << buffer << std::endl;
			delete [] buffer;
		}
		catch (std::exception& exc) {std::cout << exc.what() << std::endl;}
	}

}

GLuint GLSL_Compiler::getProgramID(void) {return programID;}

bool GLSL_Compiler::cleanUpShaders(void)
{
	glDetachShader(programID, vertexShaderID);
	glDetachShader(programID, fragmentShaderID);
	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);
	glUseProgram(0);
	glDeleteProgram(programID);
}


//Time
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


//Geometry
Model::Model(void)
{
	Vertex verticesStack[] =
	{
	glm::vec3(+0.5, +0.5, +0.5), //0 Top
	glm::vec3(+1.0, +0.0, +0.0), 
	glm::vec3(+0.0, +1.0, +0.0),
	glm::vec3(+0.5, +0.5, -0.5), //1
	glm::vec3(+0.0, +1.0, +0.0), 
	glm::vec3(+0.0, +1.0, +0.0),
	glm::vec3(-0.5, +0.5, -0.5), //2
	glm::vec3(+0.0, +0.0, +1.0),
	glm::vec3(+0.0, +1.0, +0.0),
	glm::vec3(-0.5, +0.5, +0.5), //3
	glm::vec3(+1.0, +1.0, +1.0),
	glm::vec3(+0.0, +1.0, +0.0),	

	glm::vec3(+0.5, -0.5, +0.5), //4 Bottom
	glm::vec3(+1.0, +1.0, +1.0),
	glm::vec3(+0.0, -1.0, +0.0),
	glm::vec3(+0.5, -0.5, -0.5), //5
	glm::vec3(+0.0, +0.0, +1.0),
	glm::vec3(+0.0, -1.0, +0.0),
	glm::vec3(-0.5, -0.5, -0.5), //6
	glm::vec3(+0.0, +1.0, +0.0),
	glm::vec3(+0.0, -1.0, +0.0),
	glm::vec3(-0.5, -0.5, +0.5), //7
	glm::vec3(+1.0, +0.0, +0.0),
	glm::vec3(+0.0, -1.0, +0.0),

	glm::vec3(+0.5, -0.5, -0.5), //8 Back
	glm::vec3(+1.0, +0.0, +1.0),
	glm::vec3(+0.0, +0.0, -1.0),
	glm::vec3(+0.5, +0.5, -0.5), //9
	glm::vec3(+1.0, +1.0, +0.0),
	glm::vec3(+0.0, +0.0, -1.0),
	glm::vec3(-0.5, +0.5, -0.5), //10
	glm::vec3(+0.0, +1.0, +1.0),
	glm::vec3(+0.0, +0.0, -1.0),
	glm::vec3(-0.5, -0.5, -0.5), //11
	glm::vec3(+0.5, +0.0, +0.0),
	glm::vec3(+0.0, +0.0, -1.0),

	glm::vec3(+0.5, -0.5, +0.5), //12 Front
	glm::vec3(+0.0, +0.0, +0.5),
	glm::vec3(+0.0, +0.0, +1.0),
	glm::vec3(+0.5, +0.5, +0.5), //13
	glm::vec3(+0.0, +1.0, +1.0),
	glm::vec3(+0.0, +0.0, +1.0),
	glm::vec3(-0.5, +0.5, +0.5), //14
	glm::vec3(+1.0, +1.0, +0.0),
	glm::vec3(+0.0, +0.0, +1.0),
	glm::vec3(-0.5, -0.5, +0.5), //15
	glm::vec3(+1.0, +0.0, +1.0),
	glm::vec3(+0.0, +0.0, +1.0),

	glm::vec3(-0.5, -0.5, +0.5), //16 Left
	glm::vec3(+0.5, +0.0, +0.5),
	glm::vec3(-1.0, +0.0, +0.0),
	glm::vec3(-0.5, -0.5, -0.5), //17
	glm::vec3(+0.5, +0.5, +0.0),
	glm::vec3(-1.0, +0.0, +0.0),
	glm::vec3(-0.5, +0.5, -0.5), //18
	glm::vec3(+0.0, +0.5, +0.5),
	glm::vec3(-1.0, +0.0, +0.0),
	glm::vec3(-0.5, +0.5, +0.5), //19
	glm::vec3(+0.0, +0.5, +0.5),
	glm::vec3(-1.0, +0.0, +0.0),

	glm::vec3(+0.5, -0.5, +0.5), //20 Right
	glm::vec3(+0.0, +0.5, +0.0),
	glm::vec3(+1.0, +0.0, +0.0),
	glm::vec3(+0.5, -0.5, -0.5), //21
	glm::vec3(+0.0, +0.5, +0.5),
	glm::vec3(+1.0, +0.0, +0.0),
	glm::vec3(+0.5, +0.5, -0.5), //22
	glm::vec3(+0.5, +0.5, +0.0),
	glm::vec3(+1.0, +0.0, +0.0),
	glm::vec3(+0.5, +0.5, +0.5), //23
	glm::vec3(+0.5, +0.0, +0.5),
	glm::vec3(+1.0, +0.0, +0.0)
	};

	GLushort indicesStack[] =
	{
	0,1,2, 0,2,3, //Top
	4,6,5, 4,7,6, //Bottom
	9,11,10, 8,11,9, //Back
	13,14,15, 12,13,15, //Front
	16,18,17, 16,19,18, //Left
	20,21,22, 20,22,23 //Right
	};
	positionDim = 3;
	colorDim = 3;
	normalDim = 3;
	vectorArrayStride = sizeof(GLfloat) * 9;
	positionStart = (void*)(0);
	colorStart = (void*)(sizeof(GLfloat) * 3);
	normalStart = (void*)(sizeof(GLfloat) * 6);
	indexStart = (void*)(0);
	vertexNum = 24;
	verticesBytes = 864;
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
