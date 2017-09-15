#define GL_GLEXT_PROTOTYPES //Needs to be defined for some GL funcs to work.
#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <cmath>
#include <exception>
#include <sstream>
#include <GL/gl.h>
#include <GL/glu.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <SDL2/SDL_image.h>
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
Camera::Camera(void)
{
	movespeed = 1.0; //Meters per Second
	lookspeed = radians(0.05); //Degrees per rel movment
	upVector = glm::vec3(0.0f, 1.0f, 0.0f);
	lookVector = glm::vec3(0.0f, 0.0f, -1.0f);
	posVector = glm::vec3(0.0f, 0.0f, 0.0f);
	TEventVec = glm::vec3(0.0f, 0.0f, 0.0f);
	REventVec = glm::vec2(0.0f, 0.0f);
	projectionMatrix = glm::perspective(radians(60.0), float(1.0), float(0.1), float(100.0));
}

void Camera::update(float frame_delta)
{
	float delta = frame_delta * float(movespeed / 1000.0); 
	posVector += TEventVec.z * delta * lookVector;
	posVector += TEventVec.x * delta * glm::cross(lookVector, upVector);
	posVector += TEventVec.y * delta * upVector;
	lookVector = glm::mat3(glm::rotate(glm::mat4(), -float(REventVec.x * lookspeed),
									   upVector)) * lookVector;
	glm::vec3 newXAxisVector = glm::cross(lookVector, upVector);
	lookVector = glm::mat3(glm::rotate(glm::mat4(), -float(REventVec.y * lookspeed),
									  newXAxisVector)) * lookVector; 
	REventVec = glm::vec2(0.0f, 0.0f);
}

glm::mat4 Camera::getViewMatrix(void)
{
	return glm::lookAt(posVector, (lookVector+posVector), upVector);
}


//OBJReader
OBJReader::OBJReader(std::string name)
{
	objFilename = name;	
	mtlFilename = "void";
	vertexAmount = 0;
	textureAmount = 0;
	normalAmount = 0;
	indexAmount = 0;
	faceGroupAmount = 0;
	materialAmount = 0;
	preprocOBJ();
	preprocMTL();
	allocMem();
	parseOBJ();
	parseMTL();
	allocConvMem();
	vboConvert();
	genResource();
}

bool OBJReader::preprocOBJ(void)
{
	std::ifstream OBJFile;
	OBJFile.open(objFilename.c_str());
	if(OBJFile.is_open())
	{
		std::string line;
		while (getline(OBJFile, line))
		{
			std::string command = split(line, ' ', 0);
			if (command == "v") {vertexAmount++;}
			else if (command == "vt") {textureAmount++;}
			else if (command == "vn") {normalAmount++;}
			else if (command == "f") {indexAmount += 3;}
			else if (command == "usemtl") {faceGroupAmount++;}
			else if (command == "mtllib") {mtlFilename = split(line, ' ', 1);}
		}
	}
	else
	{
		std::cout << "File " << objFilename << " failed to open for preproc..." << std::endl;
		return false;
	}
	OBJFile.close();
	return true;
}

bool OBJReader::preprocMTL(void)
{
	std::ifstream MTLFile;
	MTLFile.open(mtlFilename.c_str());
	if(MTLFile.is_open())
	{
		std::string line;
		while (getline(MTLFile, line))
		{
			std::string command = split(line, ' ', 0);
			if (command == "newmtl") {materialAmount++;}
		}
	}
	else
	{
		std::cout << "File " << mtlFilename << " failed to open for preproc..." << std::endl;
		return false;
	}
	MTLFile.close();
	return true;
}

bool OBJReader::allocMem(void)
{
	try
	{
		vertexData = new glm::vec3[vertexAmount];
		textureData = new glm::vec3[textureAmount];
		normalData = new glm::vec3[normalAmount];
		indexData = new glm::ivec3[indexAmount];	
		groupArr = new faceGroup[faceGroupAmount];
		materialArr = new material[materialAmount];
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
	delete [] textureData;
	delete [] normalData;
	delete [] indexData;
	return true;
}

bool OBJReader::parseOBJ(void)
{
	std::ifstream OBJFile;
	OBJFile.open(objFilename.c_str());
	if (OBJFile.is_open())
	{
		size_t vOffset = 0;
		size_t tOffset = 0;
		size_t nOffset = 0;
		size_t iOffset = 0;
		int gOffset = -1;
		std::string line;
		while (getline(OBJFile, line))
		{
			std::string command = split(line, ' ', 0);
			if (command == "#")	{continue;} //Comment
			else if (command == "") {continue;} //Blank Line
			else if (command == "mtllib") {continue;} //Handled in preproc
			else if (command == "usemtl")
			{
				if (gOffset > -1)
				{
					int vertexAmount = iOffset - groupArr[gOffset].offsetIndex;
					groupArr[gOffset].vertexAmount = vertexAmount;
					groupArr[gOffset].vertexSizeBytes = vertexAmount * 6 * sizeof(float);
				}
				gOffset++;
				groupArr[gOffset].mtlName = split(line, ' ', 1);
				groupArr[gOffset].offsetIndex = iOffset;
				groupArr[gOffset].offsetBytes = iOffset * 6 * sizeof(float);
			}
			else if (command == "s")
			{
				if (gOffset > -1)
				{
					std::string smooth = split(line, ' ', 1);
					if (smooth == "off") {groupArr[gOffset].smooth = 0;}
					else {groupArr[gOffset].smooth = stoi(smooth);}
				}
				else
				{
					std::cout << "Warning: Cannot define smooth param before material..." << std::endl;
				} 
			}
			else if (command == "o")
			{
				name = split(line, ' ', 1);
			}
			else if (command == "v")
			{
				float x = stof(split(line, ' ', 1));
				float y = stof(split(line, ' ', 2));
				float z = stof(split(line, ' ', 3));	
				vertexData[vOffset] = glm::vec3(x,y,z);
				vOffset++;
			}
			else if (command == "vt")
			{
				float u = stof(split(line, ' ', 1));
				float v = stof(split(line, ' ', 2));
				textureData[tOffset] = glm::vec3(u, v, 0.0);
				tOffset++;
			}
			else if (command == "vn")
			{
				float x = stof(split(line, ' ', 1));
				float y = stof(split(line, ' ', 2));
				float z = stof(split(line, ' ', 3));
				normalData[nOffset] = glm::vec3(x,y,z);
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
					indexData[iOffset] = glm::ivec3(v,t,n);
					iOffset++;
				}	
			}
			else
			{
				std::cout << "Warning: Unrecognized command '" << command <<
							 "' in OBJ file '" << objFilename << "'." << std::endl;
			}
			
		}
		int vertexAmount = iOffset - groupArr[gOffset].offsetIndex;
		groupArr[gOffset].vertexAmount = vertexAmount;
		groupArr[gOffset].vertexSizeBytes = vertexAmount * 6 * sizeof(float);
	}
	else
	{
		std::cout << "File " << objFilename << " failed to open for parsing..." << std::endl;
		return false;
	}	
	OBJFile.close();
	return true;
}

bool OBJReader::parseMTL(void)
{
	std::ifstream MTLFile;
	MTLFile.open(mtlFilename.c_str());
	if(MTLFile.is_open())
	{
		int mOffset = -1;
		std::string line;
		while (getline(MTLFile, line))
		{
			std::string command = split(line, ' ', 0);
			if (command == "#")	{continue;} //Comment
			else if (command == "") {continue;} //Blank
			else if (command == "newmtl")
			{
				mOffset++;
				materialArr[mOffset].id = mOffset;
				materialArr[mOffset].name = split(line, ' ', 1);
				materialMap[materialArr[mOffset].name] = mOffset;
				materialArr[mOffset].mtlFilename = mtlFilename;
				materialArr[mOffset].map_Kd = "";
				materialArr[mOffset].textureID = 0;
			}
			else if (command == "Ns")
			{
				materialArr[mOffset].Ns = stof(split(line, ' ', 1));
			}
			else if (command == "Ka")
			{
				float r = stof(split(line, ' ', 1));
				float g = stof(split(line, ' ', 2));
				float b = stof(split(line, ' ', 3));
				materialArr[mOffset].Ka = glm::vec3(r,g,b);
			}
			else if (command == "Kd")
			{
				float r = stof(split(line, ' ', 1));
				float g = stof(split(line, ' ', 2));
				float b = stof(split(line, ' ', 3));
				materialArr[mOffset].Kd = glm::vec3(r,g,b);
			}
			else if (command == "Ks")
			{
				float r = stof(split(line, ' ', 1));
				float g = stof(split(line, ' ', 2));
				float b = stof(split(line, ' ', 3));
				materialArr[mOffset].Ks = glm::vec3(r,g,b);
			}
			else if (command == "Ke")
			{
				float r = stof(split(line, ' ', 1));
				float g = stof(split(line, ' ', 2));
				float b = stof(split(line, ' ', 3));
				materialArr[mOffset].Ke == glm::vec3(r,g,b);
			}			
			else if (command == "Ni")
			{
				materialArr[mOffset].Ni == stof(split(line, ' ', 1));
			}
			else if (command == "d")
			{
				materialArr[mOffset].d = stof(split(line, ' ', 1));
			}
			else if (command == "illum")
			{
				materialArr[mOffset].illum = stoi(split(line, ' ', 1));
			}
			else if (command == "map_Kd")
			{
				std::string textureFilename = split(line, ' ', 1);
				materialArr[mOffset].map_Kd = textureFilename;
				materialArr[mOffset].textureID = loadTexture(textureFilename); 
			}
			else
			{
				std::cout << "Warning: Unrecognized command '" << command <<
							 "' in MTL file '" << mtlFilename << "'." << std::endl;
			}
		}
	}
	else
	{
		std::cout << "File " << mtlFilename << " failed to open for preproc..." << std::endl;
		return false;
	}
	MTLFile.close();
	return true;
}

bool OBJReader::allocConvMem(void)
{
	try
	{
		vboData = new glm::vec3[indexAmount * 3];
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
	size_t tOffset = 0;
	size_t nOffset = 0;
	for (int offset = 0; offset < indexAmount; offset++)
	{
		glm::ivec3 index = indexData[offset];
		vOffset = index[0] - 1;
		glm::vec3 textureVector;
		if (index[1] == 0) {textureVector = glm::vec3(0.0, 0.0, 0.0);}
		else {textureVector = textureData[index[1] - 1];}
		nOffset = index[2] - 1;
		vboData[vboOffset] = vertexData[vOffset];
		vboOffset++;
		vboData[vboOffset] = textureVector;
		vboOffset++;
		vboData[vboOffset] = normalData[nOffset];	
		vboOffset++;
	}
	return true;
}

bool OBJReader::genResource(void)
{
	obj.name = name;
	obj.objFilename = objFilename;
	obj.mtlFilename = objFilename;
	obj.vertexData = vboData;
	obj.vertexAmount = indexAmount;
	obj.materialAmount = materialAmount;
	obj.materialArr = materialArr;
	obj.faceGroupAmount = faceGroupAmount;
	for (int n = 0; n < faceGroupAmount; n++)
	{
		groupArr[n].mtlID = materialMap[groupArr[n].mtlName];
	}
	obj.groupArr = groupArr;
	obj.vertexSizeBytes = indexAmount * 3 * sizeof(glm::vec3);
	obj.tobevboloaded = true;
	obj.vboloaded = false;
	obj.hidden = false;
	obj.translationMatrix = glm::mat4();
	obj.rotationMatrix = glm::mat4();
	releaseMem();
	return true;
}

modelResource OBJReader::pushResource(void) {return obj;}

GLuint loadTexture(std::string filename)
{
	SDL_Surface* texture;
	GLuint textureID;
	int mode;
	texture = IMG_Load(filename.c_str());
	if (!texture) {std::cout << "Warning: Texture '" << filename << "' failed to load..." << std::endl;}
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	if (texture->format->BytesPerPixel == 4) {mode = GL_RGBA;}
	else if (texture->format->BytesPerPixel = 3) {mode = GL_RGB;}
	glTexImage2D(GL_TEXTURE_2D, 0, mode, texture->w, texture->h, 0, mode, GL_UNSIGNED_BYTE, texture->pixels);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	SDL_FreeSurface(texture);
	return textureID;
}

//Resource Manager
ResourceManager::ResourceManager(void)
{
	modelAmount = 0;
	modelArr = new modelResource[modelAmount];
}

bool ResourceManager::pullOBJResources(modelResource model)
{
	if (modelMap.find(model.name) == modelMap.end())
	{
		model.id = modelAmount;
		modelMap[model.name] = model.id;
		modelAmount++;
		modelResource* newArr = new modelResource[modelAmount - 1];
		for (int n = 0; n < modelAmount - 1; n++) {newArr[n] = modelArr[n];}
		delete [] modelArr;
		modelArr = new modelResource[modelAmount];
		for (int n = 0; n < modelAmount - 1; n++) {modelArr[n] = newArr[n];}
		delete [] newArr;
		modelArr[model.id] = model;
	}
	else
	{
		std::cout << "Warning: Model resource with name '" << model.name << "' already exists." << std::endl;
	}
}

bool ResourceManager::releaseMem(void)
{
	for (int n = 0; n < modelAmount; n++)
	{
		delete [] modelArr[n].vertexData;
		delete [] modelArr[n].groupArr;
		delete [] modelArr[n].materialArr;
	}
	delete [] modelArr;
}


//Video Memory Manger
VRAMManager::VRAMManager(void)
{
	vboSizeBytes = 0;
	positionStart = (void*)(0);
	positionDim = 3;
	textureStart = (void*)(sizeof(float) * 3);
	textureDim = 3;
	normalStart = (void*)(sizeof(float) * 6);
	normalDim = 3;
	vboStride = sizeof(float) * 9;
	glGenBuffers(1, &vboID);
	glBindBuffer(GL_ARRAY_BUFFER, vboID);	
}

bool VRAMManager::genVBO(modelResource* model, int modelAmount)
{
	int newVBOSizeBytes = 0;
	for (int n = 0; n < modelAmount; n++)
	{
		if (model[n].vboloaded == true || model[n].tobevboloaded == true)
		{
			newVBOSizeBytes += model[n].vertexSizeBytes;
		}
	}
	glDeleteBuffers(1, &vboID);
	glGenBuffers(1, &vboID);
	glBindBuffer(GL_ARRAY_BUFFER, vboID);
	glBufferData(GL_ARRAY_BUFFER, newVBOSizeBytes, NULL, GL_STATIC_DRAW);
	vboSizeBytes = newVBOSizeBytes;
	size_t vOffset = 0;
	size_t iOffset = 0;
	for (int n = 0; n < modelAmount; n++)
	{
		if (model[n].vboloaded == true || model[n].tobevboloaded == true)
		{
			model[n].vboOffsetBytes = vOffset;
			model[n].vboOffsetIndex = iOffset;
			glBufferSubData(GL_ARRAY_BUFFER,
							vOffset,
							model[n].vertexSizeBytes,
							model[n].vertexData);
			vOffset += (model[n].vertexSizeBytes);
			iOffset += (model[n].vertexAmount);
			model[n].vboloaded = true;
			model[n].tobevboloaded = false;
		}	
	}	
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(0, positionDim,
						  GL_FLOAT, GL_FALSE,
						  vboStride, positionStart);
	glVertexAttribPointer(1, textureDim,
						  GL_FLOAT, GL_FALSE,
						  vboStride, textureStart);
	glVertexAttribPointer(2, normalDim,
						  GL_FLOAT, GL_FALSE,
						  vboStride, normalStart);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

bool VRAMManager::releaseMem(void)
{ 
	glDeleteBuffers(1, &vboID);
}


//Shader
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
