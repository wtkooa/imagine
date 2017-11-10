//___|"ie_mesh.h.cpp"|__________________________________________________________
//
// Project: Imagine: 3D Environment Engine
// Version: 0.1.0
// Author: David Lipps
// License: MIT License
//
// Copyright (c) 2017 David E Lipps
//______________________________________________________________________________

#include "ie_mesh.h"

#include <map>
#include <set>

#include <glm/gtc/type_precision.hpp>
#include <glm/gtx/hash.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "ie_vram.h"
#include "ie_utils.h"

//___|MESH|_____________________________________________________________________

ie::Mesh::Mesh()
{
  reset();
}

ie::Mesh::~Mesh()
{
  for(auto ru = renderUnits.begin(); ru != renderUnits.end(); ru++)
  {
    delete *ru;
  }
}


void ie::Mesh::reset(void)
{
  deleteAllRenderUnits();
}


void ie::Mesh::addRenderUnit(RenderUnit* ru)
{
  ru->setParentMesh(this);
  renderUnits.push_back(ru);
}


unsigned int ie::Mesh::getRenderUnitAmount(void) {return renderUnits.size();}
ie::RenderUnit* ie::Mesh::getRenderUnit(unsigned int n) {return renderUnits[n];}
std::vector<ie::RenderUnit*>* ie::Mesh::getRenderUnits(void) {return &renderUnits;}


void ie::Mesh::deleteRenderUnit(unsigned int n)
{
  delete renderUnits[n]; 
  renderUnits.erase(renderUnits.begin() + n);
}
void ie::Mesh::deleteAllRenderUnits(void)
{
  for (auto ru = renderUnits.begin(); ru != renderUnits.end(); ru++)
  {
    delete *ru;
  }
  renderUnits.clear();
}


void ie::Mesh::removeMeshDuplicates(float sensitivity)
{
  for (auto ru = renderUnits.begin(); ru != renderUnits.end(); ru++)
  {
    (*ru)->removeDuplicates(sensitivity);
  }
}
void ie::Mesh::removeRenderUnitDuplicates(unsigned int ru, float sensitivity)
{
  (renderUnits[ru])->removeDuplicates(sensitivity);
}

//______________________________________________________________________________

//___|RENDER UNIT|______________________________________________________________

ie::RenderUnit::RenderUnit()
{
  factoryReset();
}


void ie::RenderUnit::factoryReset(void)
{
  parentMesh = NULL;
  material = NULL;
  elementArrayLocation = 0;
  vertexArrayLocation = 0;
  vertexFormat = NULL_FORMAT;
  clearAllAttribs();
}


void ie::RenderUnit::setParentMesh(Mesh* mesh) {parentMesh = mesh;}
ie::Mesh* ie::RenderUnit::getParentMesh(void) {return parentMesh;}


void ie::RenderUnit::setMaterial(Material* mat) {material = mat;}
ie::Material* ie::RenderUnit::getMaterial(void) {return material;}


void ie::RenderUnit::setElementArrayLocation(unsigned int loc)
{
  elementArrayLocation = loc;
}
unsigned int ie::RenderUnit::getElementArrayLocation(void)
{
  return elementArrayLocation;
}


void ie::RenderUnit::setVertexArrayLocation(unsigned int loc)
{
  vertexArrayLocation = loc;
}
unsigned int ie::RenderUnit::getVertexArrayLocation(void)
{
  return vertexArrayLocation;
}


unsigned int ie::RenderUnit::getElementArrayAmount(void)
{
  return indices.size();
}
unsigned int ie::RenderUnit::getVertexArrayAmount(void)
{
  bool usesPosition = vertexFormat == P_FORMAT || vertexFormat == PN_FORMAT ||
                      vertexFormat == PMN_FORMAT || vertexFormat == PMNCD_FORMAT ||
                      vertexFormat == PMNCDD_FORMAT;
  if (usesPosition)
  {
    return positions.size();
  }
}


void ie::RenderUnit::setVertexFormat(VboDataFormat format)
{
  vertexFormat = format;
}
void ie::RenderUnit::detectVertexFormat(void)
{
  bool p, n, m, c, d1, d2;
  if (positions.size() == 0) {p = false;} else {p = true;}
  if (normals.size() == 0) {n = false;} else {n = true;}
  if (maps.size() == 0) {m = false;} else {m = true;}
  if (colors.size() == 0) {c = false;} else {c = true;}
  if (data1.size() == 0) {d1 = false;} else {d1 = true;}
  if (data2.size() == 0) {d2 = false;} else {d2 = true;}
  bool P = p && !(n || m || c || d1 || d2);
  bool PN = (p && n) && !(m || c || d1 || d2);
  bool PMN = (p && m && n) && !(c || d1 || d2);
  bool PMNCD = (p && m && n && c && d1) && !d2; 
  bool PMNCDD = p && m && n && c && d1 && d2;
  if (P) {vertexFormat = P_FORMAT;}
  else if (PN) {vertexFormat = PN_FORMAT;} 
  else if (PMN) {vertexFormat = PMN_FORMAT;} 
  else if (PMNCD) {vertexFormat = PMNCD_FORMAT;} 
  else if (PMNCDD) {vertexFormat = PMNCDD_FORMAT;} 
}
ie::VboDataFormat ie::RenderUnit::getVertexFormat(void)
{
  return vertexFormat;
}


void ie::RenderUnit::addPositionAttrib(glm::vec3 position)
{
  positions.push_back(position);
}
glm::vec3 ie::RenderUnit::getPositionAttrib(unsigned int vertex)
{
  return positions[vertex];
}


void ie::RenderUnit::addNormalAttrib(glm::vec3 n)
{
  unsigned int normal = packNormalAttrib(n);
  normals.push_back(normal);
}
unsigned int ie::RenderUnit::getPackedNormalAttrib(unsigned int vertex)
{
  return normals[vertex];
}
glm::vec3 ie::RenderUnit::getUnpackedNormalAttrib(unsigned int vertex)
{
  return unpackNormalAttrib(normals[vertex]);
}


void ie::RenderUnit::addMapAttrib(glm::vec2 map)
{
  maps.push_back(map);
}
glm::vec2 ie::RenderUnit::getMapAttrib(unsigned int vertex)
{
  return maps[vertex];
}


void ie::RenderUnit::addColorAttrib(glm::vec3 color)
{
  colors.push_back(packColorAttrib(color));
}
glm::u8vec4 ie::RenderUnit::getPackedColorAttrib(unsigned int vertex)
{
  return colors[vertex];
}
glm::vec3 ie::RenderUnit::getUnpackedColorAttrib(unsigned int vertex)
{
  return unpackColorAttrib(colors[vertex]);
}


void ie::RenderUnit::addData1Attrib(glm::u8vec4 data)
{
  data1.push_back(data);
}
glm::u8vec4 ie::RenderUnit::getData1Attrib(unsigned int vertex)
{
  return data1[vertex];
}


void ie::RenderUnit::addData2Attrib(glm::u8vec4 data)
{
  data2.push_back(data);
}
glm::u8vec4 ie::RenderUnit::getData2Attrib(unsigned int vertex)
{
  return data2[vertex];
}


void ie::RenderUnit::addIndexAttrib(unsigned int index)
{
  indices.push_back(index);
}
unsigned int ie::RenderUnit::getIndexAttrib(unsigned int index)
{
  return indices[index];
}


void ie::RenderUnit::clearAttrib(VertexAttribute attrib)
{
  if (attrib == E_ATTRIB)
  {
    indices.clear();
  }
  else if (attrib == P_ATTRIB)
  {
    positions.clear();
  }
  else if (attrib == N_ATTRIB)
  {
    normals.clear();
  }
  else if (attrib == M_ATTRIB)
  {
    maps.clear();
  }
  else if (attrib == C_ATTRIB)
  {
    colors.clear();
  }
  else if (attrib == D1_ATTRIB)
  {
    data1.clear();
  }
  else if (attrib == D2_ATTRIB)
  {
    data2.clear();
  }
}


void ie::RenderUnit::clearAllAttribs(void)
{
    indices.clear();
    positions.clear();
    normals.clear();
    maps.clear();
    colors.clear();
    data1.clear();
    data2.clear();
}


void ie::RenderUnit::deleteVertex(unsigned int vertex)
{
  if (positions.size() > vertex)
  {
    positions.erase(positions.begin() + vertex);
  }
  if (normals.size() > vertex)
  {
    normals.erase(normals.begin() + vertex); 
  }
  if (maps.size() > vertex)
  {
    maps.erase(maps.begin() + vertex);
  }
  if (colors.size() > vertex)
  {
    colors.erase(colors.begin() + vertex);
  }
  if (data1.size() > vertex)
  {
    data1.erase(data1.begin() + vertex);
  }
  if (data2.size() > vertex)
  {
    data2.erase(data2.begin() + vertex);
  }
}


void ie::RenderUnit::removeDuplicates(float sensitivity)
{
  std::map<unsigned int, std::set<unsigned int>> vertices; 
  for (auto index = indices.begin(); index != indices.end(); index++)
  {
    unsigned int vertexArrayIndex = (*index); 
    std::set<unsigned int> set;
    set.insert(vertexArrayIndex);
    glm::vec3 position = getPositionAttrib(vertexArrayIndex);
    unsigned int hash = ie::spatialHash(position, sensitivity);
    auto result = vertices.insert(std::pair<unsigned int, std::set<unsigned int>>(hash, set));
    bool isPositionDuplicate = !result.second;
    if (isPositionDuplicate)
    {
      bool noDuplicateFound = true; 
      for (auto i = (result.first->second).begin(); i != (result.first->second).end(); i++)
      {
        bool isVertexDuplicate = getPackedNormalAttrib((*i)) == getPackedNormalAttrib(vertexArrayIndex) &&
                                 getMapAttrib((*i)) == getMapAttrib(vertexArrayIndex) &&
                                 getPackedColorAttrib((*i)) == getPackedColorAttrib(vertexArrayIndex) &&
                                 getData1Attrib((*i)) == getData1Attrib(vertexArrayIndex) &&
                                 getData2Attrib((*i)) == getData2Attrib(vertexArrayIndex);
        if (isVertexDuplicate)
        {
          unsigned int newTarget = (*i);  
          for (auto n = index; n != indices.end(); n++)
          {
            if ((*n) == vertexArrayIndex)
            {
              (*n) = newTarget;
            }
            else if ((*n) > vertexArrayIndex)
            {
              (*n) -= 1;
            }
          }
          deleteVertex(vertexArrayIndex);
          noDuplicateFound = false;
          break;
        }
      }
      if (noDuplicateFound)
      {
        (result.first->second).insert(vertexArrayIndex);
      }
    }
  }
}

//______________________________________________________________________________
