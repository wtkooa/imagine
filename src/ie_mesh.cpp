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

#include <iostream>
#include <map>
#include <set>

#include <glm/gtc/type_precision.hpp>
#include <glm/gtx/hash.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "ie_enum.h"
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
  vertexFormat = IE_NULL_FORMAT;
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
  detectVertexFormat();
  bool usesPosition = (vertexFormat == IE_P_FORMAT) ||
                      (vertexFormat == IE_PN_FORMAT) ||
                      (vertexFormat == IE_PMN_FORMAT) ||
                      (vertexFormat == IE_PMNCD_FORMAT) ||
                      (vertexFormat == IE_PMNCDD_FORMAT);
  if (usesPosition)
  {
    return positions.size();
  }
}


void ie::RenderUnit::setVertexFormat(IEenum format)
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
  if (P) {vertexFormat = IE_P_FORMAT;}
  else if (PN) {vertexFormat = IE_PN_FORMAT;} 
  else if (PMN) {vertexFormat = IE_PMN_FORMAT;} 
  else if (PMNCD) {vertexFormat = IE_PMNCD_FORMAT;} 
  else if (PMNCDD) {vertexFormat = IE_PMNCDD_FORMAT;} 
}
ie::IEenum ie::RenderUnit::getVertexFormat(void)
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


void ie::RenderUnit::addUnpackedNormalAttrib(glm::vec3 n)
{
  unsigned int normal = packNormalAttrib(n);
  normals.push_back(normal);
}
void ie::RenderUnit::addPackedNormalAttrib(unsigned int normal)
{
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


void ie::RenderUnit::addUnpackedColorAttrib(glm::vec3 color)
{
  colors.push_back(packColorAttrib(color));
}
void ie::RenderUnit::addPackedColorAttrib(glm::u8vec4 color)
{
  colors.push_back(color);
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


void ie::RenderUnit::clearAttrib(IEenum attrib)
{
  if (attrib == IE_ELEMENT_ATTRIB)
  {
    indices.clear();
  }
  else if (attrib == IE_POSITION_ATTRIB)
  {
    positions.clear();
  }
  else if (attrib == IE_NORMAL_ATTRIB)
  {
    normals.clear();
  }
  else if (attrib == IE_MATERIAL_ATTRIB)
  {
    maps.clear();
  }
  else if (attrib == IE_COLOR_ATTRIB)
  {
    colors.clear();
  }
  else if (attrib == IE_DATA1_ATTRIB)
  {
    data1.clear();
  }
  else if (attrib == IE_DATA2_ATTRIB)
  {
    data2.clear();
  }
}


unsigned int ie::RenderUnit::getAttribAmount(IEenum attrib)
{
  if (attrib == IE_ELEMENT_ATTRIB)
  {
    return indices.size();
  }
  else if (attrib == IE_POSITION_ATTRIB)
  {
    return positions.size();
  }
  else if (attrib == IE_NORMAL_ATTRIB)
  {
    return normals.size();
  }
  else if (attrib == IE_MATERIAL_ATTRIB)
  {
    return maps.size();
  }
  else if (attrib == IE_COLOR_ATTRIB)
  {
    return colors.size();
  }
  else if (attrib == IE_DATA1_ATTRIB)
  {
    return data1.size();
  }
  else if (attrib == IE_DATA2_ATTRIB)
  {
    return data2.size();
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

        bool isNormalDuplicate;
        bool isMapDuplicate; 
        bool isColorDuplicate;
        bool isData1Duplicate;
        bool isData2Duplicate; 
        if (normals.size() > 0)
        {
          isNormalDuplicate = getPackedNormalAttrib((*i)) == getPackedNormalAttrib(vertexArrayIndex);
        } else {isNormalDuplicate = true;}
        if (maps.size() > 0)
        {
          isMapDuplicate = getMapAttrib((*i)) == getMapAttrib(vertexArrayIndex);
        } else {isMapDuplicate = true;}
        if (colors.size() > 0)
        {
          isColorDuplicate = getPackedColorAttrib((*i)) == getPackedColorAttrib(vertexArrayIndex);
        } else {isColorDuplicate = true;}
        if (data1.size() > 0)
        {
          isData1Duplicate = getData1Attrib((*i)) == getData1Attrib(vertexArrayIndex);
        } else {isData1Duplicate = true;}
        if (data2.size() > 0)
        {
          isData2Duplicate = getData2Attrib((*i)) == getData2Attrib(vertexArrayIndex);
        } else {isData2Duplicate = true;}

        bool isVertexDuplicate = isNormalDuplicate && isMapDuplicate &&
                                 isColorDuplicate && isData1Duplicate &&
                                 isData2Duplicate;

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
