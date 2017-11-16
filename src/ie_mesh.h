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
#include "ie_enum.h"

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

    unsigned int removeMeshDuplicates(float);
    void removeRenderUnitDuplicates(unsigned int, float);

    virtual void reset(void);

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
    void setVertexFormat(IEenum);
    void detectVertexFormat(void);
    Mesh* getParentMesh(void);
    Material* getMaterial(void);
    unsigned int getElementArrayLocation(void);
    unsigned int getElementArrayAmount(void);
    unsigned int getVertexArrayLocation(void);
    unsigned int getVertexArrayAmount(void);
    unsigned int getAttribAmount(IEenum);
    IEenum getVertexFormat(void);
    void addPositionAttrib(glm::vec3);
    void addUnpackedNormalAttrib(glm::vec3);
    void addPackedNormalAttrib(unsigned int);
    void addMapAttrib(glm::vec2);
    void addUnpackedColorAttrib(glm::vec3);
    void addPackedColorAttrib(unsigned int);
    void addUnpackedDataAttrib(short, glm::u8vec4);
    void addPackedDataAttrib(short, unsigned int);
    void addIndexAttrib(unsigned int);
    glm::vec3 getPositionAttrib(unsigned int);
    unsigned int getPackedNormalAttrib(unsigned int);
    glm::vec3 getUnpackedNormalAttrib(unsigned int);
    glm::vec2 getMapAttrib(unsigned int);
    unsigned int getPackedColorAttrib(unsigned int);
    glm::vec3 getUnpackedColorAttrib(unsigned it);
    glm::u8vec4 getUnpackedDataAttrib(short, unsigned int);
    unsigned int getPackedDataAttrib(short, unsigned int);
    glm::uvec4 getDataAttrib(unsigned int);
    unsigned int getIndexAttrib(unsigned int);

    void clearAttrib(IEenum);
    void clearDataChannel(short);
    void clearAllAttribs(void);

    unsigned int removeDuplicates(float);

    private:
    Mesh* parentMesh;
    Material* material;
    unsigned int elementArrayLocation;
    unsigned int vertexArrayLocation;
    IEenum vertexFormat;

    std::vector<glm::vec3> positions;
    std::vector<unsigned int> normals;
    std::vector<glm::vec2> maps;
    std::vector<unsigned int> colors;
    glm::uvec4 dataChannelAmount;
    std::vector<glm::uvec4> data;
    std::vector<unsigned int> indices;

    void dynamicDataIncrease(unsigned int);
    void deleteVertex(unsigned int);
  };

}

#endif
