#ifndef IE_MESH_H
#define IE_MESH_H

//___|"ie_mesh.h"|______________________________________________________________
//
// Project: Imagine: 3D Environment Engine
// Version: 0.1.0
// Author: David Lipps
// License: MIT License
//
// Copyright (c) 2017 David E Lipps
//______________________________________________________________________________

#include <list>
#include <string>
#include <vector>

#define GL_GLEXT_PROTOTYPES //Needs to be defined for some GL funcs to work.
#include <GL/gl.h>
#include <GL/glu.h>
#include <glm/gtc/type_precision.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "ie_asset.h"
#include "ie_material.h"
#include "ie_vram.h"

namespace ie
{

  //FORWARD DECLARATIONS
  class RenderUnit;

  class Mesh : public Asset 
  {
    public:
    Mesh();
    ~Mesh();

    //GETTERS AND SETTERS
    void addRenderUnit(RenderUnit*);
    unsigned int getRenderUnitAmount(void);
    RenderUnit* getRenderUnit(unsigned int);
    std::vector<RenderUnit*>* getRenderUnits(void);

    void deleteRenderUnit(unsigned int);
    void deleteAllRenderUnits(void);

    void removeMeshDuplicates(float);
    void removeRenderUnitDuplicates(unsigned int, float);

    virtual void factoryReset(void);

    private:
    std::vector<RenderUnit*> renderUnits;
  };


  class RenderUnit
  {
    public:
    RenderUnit();
    void factoryReset(void);

    //GETTERS AND SETTERS
    void setParentMesh(Mesh*);
    void setMaterial(Material*);
    void setElementArrayLocation(unsigned int);
    void setVertexArrayLocation(unsigned int);
    void setVertexFormat(VboDataFormat);
    void detectVertexFormat(void);
    Mesh* getParentMesh(void);
    Material* getMaterial(void);
    unsigned int getElementArrayLocation(void);
    unsigned int getElementArrayAmount(void);
    unsigned int getVertexArrayLocation(void);
    unsigned int getVertexArrayAmount(void);
    VboDataFormat getVertexFormat(void);
    void addPositionAttrib(glm::vec3);
    void addNormalAttrib(glm::vec3);
    void addMapAttrib(glm::vec2);
    void addColorAttrib(glm::vec3);
    void addData1Attrib(glm::u8vec4);
    void addData2Attrib(glm::u8vec4);
    void addIndexAttrib(unsigned int);
    glm::vec3 getPositionAttrib(unsigned int);
    unsigned int getPackedNormalAttrib(unsigned int);
    glm::vec3 getUnpackedNormalAttrib(unsigned int);
    glm::vec2 getMapAttrib(unsigned int);
    glm::u8vec4 getPackedColorAttrib(unsigned int);
    glm::vec3 getUnpackedColorAttrib(unsigned it);
    glm::u8vec4 getData1Attrib(unsigned int);
    glm::u8vec4 getData2Attrib(unsigned int);
    unsigned int getIndexAttrib(unsigned int);

    void clearAttrib(VertexAttribute);
    void clearAllAttribs(void);

    void removeDuplicates(float);

    private:
    Mesh* parentMesh;
    Material* material;
    unsigned int elementArrayLocation;
    unsigned int vertexArrayLocation;
    VboDataFormat vertexFormat;

    std::vector<glm::vec3> positions;
    std::vector<unsigned int> normals;
    std::vector<glm::vec2> maps;
    std::vector<glm::u8vec4> colors;
    std::vector<glm::u8vec4> data1;
    std::vector<glm::u8vec4> data2;
    std::vector<unsigned int> indices;


    void deleteVertex(unsigned int);
  };

}

#endif
