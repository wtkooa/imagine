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

#include <glm/gtc/packing.hpp>
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


unsigned int ie::Mesh::removeMeshDuplicates(float sensitivity)
{
  unsigned int duplicateAmount = 0;
  for (auto ru = renderUnits.begin(); ru != renderUnits.end(); ru++)
  {
    duplicateAmount += (*ru)->removeDuplicates(sensitivity);
  }

  log->info("%s removed %i vertex duplicates using %f sensitivity",
            getName().c_str(), duplicateAmount, sensitivity);
  
  return duplicateAmount;
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
  if (dataChannelAmount.x == 0) {d1 = false;} else {d1 = true;}
  if (dataChannelAmount.y == 0) {d2 = false;} else {d2 = true;}
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
  glm::vec4 w(n.x, n.y, n.z, 1.0f);
  normals.push_back(glm::packSnorm3x10_1x2(w));
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
  glm::vec4 n = glm::unpackSnorm3x10_1x2(normals[vertex]);
  return glm::vec3(n.x, n.y, n.z);
}


void ie::RenderUnit::addMapAttrib(glm::vec2 map)
{
  maps.push_back(map);
}
glm::vec2 ie::RenderUnit::getMapAttrib(unsigned int vertex)
{
  return maps[vertex];
}


void ie::RenderUnit::addUnpackedColorAttrib(glm::vec3 c)
{
  glm::vec4 w(c.x, c.y, c.z, 0.0f);
  colors.push_back(glm::packUnorm4x8(w));
}
void ie::RenderUnit::addPackedColorAttrib(unsigned int color)
{
  colors.push_back(color);
}
unsigned int ie::RenderUnit::getPackedColorAttrib(unsigned int vertex)
{
  return colors[vertex];
}
glm::vec3 ie::RenderUnit::getUnpackedColorAttrib(unsigned int vertex)
{
  glm::vec4 w = glm::unpackUnorm4x8(colors[vertex]);
  return glm::vec3(w.x, w.y, w.z);
}


void ie::RenderUnit::dynamicDataIncrease(unsigned int vertex)
{
  if (vertex > data.size())
  {
    data.push_back(glm::uvec4(0, 0, 0, 0));
  }
}
void ie::RenderUnit::addUnpackedDataAttrib(short channel, glm::u8vec4 input)
{
  if (channel == 1)
  {
    unsigned int vertex = dataChannelAmount.x + 1;
    dynamicDataIncrease(vertex);
    data[vertex].x = glm::packUnorm4x8(glm::vec4(float(input.x),
                                                 float(input.y),
                                                 float(input.z), 0.0f));
    dataChannelAmount.x += 1;
  }
  else if (channel == 2)
  {
    unsigned int vertex = dataChannelAmount.y + 1;
    dynamicDataIncrease(vertex);
    data[vertex].y = glm::packUnorm4x8(glm::vec4(float(input.x),
                                                 float(input.y),
                                                 float(input.z), 0.0f));
    dataChannelAmount.y += 1;
  }
  else if (channel == 3)
  {
    unsigned int vertex = dataChannelAmount.z + 1;
    dynamicDataIncrease(vertex);
    data[vertex].z = glm::packUnorm4x8(glm::vec4(float(input.x),
                                                 float(input.y),
                                                 float(input.z), 0.0f));
    dataChannelAmount.z += 1;
  }
  else if (channel == 4)
  {
    unsigned int vertex = dataChannelAmount.w + 1;
    dynamicDataIncrease(vertex);
    data[vertex].w = glm::packUnorm4x8(glm::vec4(float(input.x),
                                                 float(input.y),
                                                 float(input.z), 0.0f));
    dataChannelAmount.w += 1;
  }

}
void ie::RenderUnit::addPackedDataAttrib(short channel, unsigned int input)
{
  if (channel == 1)
  {
    unsigned int vertex = dataChannelAmount.x + 1;
    dynamicDataIncrease(vertex);
    (data[vertex]).x = input; 
    dataChannelAmount.x += 1;
  }
  else if (channel == 2)
  {
    unsigned int vertex = dataChannelAmount.y + 1;
    dynamicDataIncrease(vertex);
    data[vertex].y = input; 
    dataChannelAmount.y += 1;
  }
  else if (channel == 3)
  {
    unsigned int vertex = dataChannelAmount.z + 1;
    dynamicDataIncrease(vertex);
    data[vertex].z = input; 
    dataChannelAmount.z += 1;
  }
  else if (channel == 4)
  {
    unsigned int vertex = dataChannelAmount.w + 1;
    dynamicDataIncrease(vertex);
    data[vertex].w = input; 
    dataChannelAmount.w += 1;
  }
}
glm::u8vec4 ie::RenderUnit::getUnpackedDataAttrib(short channel, unsigned int vertex)
{
  if (channel == 1)
  {
    glm::vec4 w = glm::unpackUnorm4x8(data[vertex].x);
    glm::u8vec4 data;
    data.x = (unsigned int)w.x;
    data.y = (unsigned int)w.y;
    data.z = (unsigned int)w.z;
    data.w = (unsigned int)w.w;
    return data; 
  }
  else if (channel == 2)
  {
    glm::vec4 w = glm::unpackUnorm4x8(data[vertex].y);
    glm::u8vec4 data;
    data.x = (unsigned int)w.x;
    data.y = (unsigned int)w.y;
    data.z = (unsigned int)w.z;
    data.w = (unsigned int)w.w;
    return data; 
  }
  else if (channel == 3)
  {
    glm::vec4 w = glm::unpackUnorm4x8(data[vertex].z);
    glm::u8vec4 data;
    data.x = (unsigned int)w.x;
    data.y = (unsigned int)w.y;
    data.z = (unsigned int)w.z;
    data.w = (unsigned int)w.w;
    return data; 
  }
  else if (channel == 4)
  {
    glm::vec4 w = glm::unpackUnorm4x8(data[vertex].w);
    glm::u8vec4 data;
    data.x = (unsigned int)w.x;
    data.y = (unsigned int)w.y;
    data.z = (unsigned int)w.z;
    data.w = (unsigned int)w.w;
    return data; 
  }
}
unsigned int ie::RenderUnit::getPackedDataAttrib(short channel, unsigned int vertex)
{
  if (channel == 1)
  {
    return data[vertex].x; 
  }
  else if (channel == 2)
  {
    return data[vertex].y; 
  }
  else if (channel == 3)
  {
    return data[vertex].z; 
  }
  else if (channel == 4)
  {
    return data[vertex].w; 
  }
}
glm::uvec4 ie::RenderUnit::getDataAttrib(unsigned int vertex)
{
  return data[vertex];
}


void ie::RenderUnit::addIndexAttrib(unsigned int index)
{
  indices.push_back(index);
}
unsigned int ie::RenderUnit::getIndexAttrib(unsigned int index)
{
  return indices[index];
}

void ie::RenderUnit::clearDataChannel(short channel)
{
    if (channel == 1) {dataChannelAmount.x = 0;}
    else if (channel == 2) {dataChannelAmount.y = 0;}
    else if (channel == 3) {dataChannelAmount.z = 0;}
    else if (channel == 4) {dataChannelAmount.w = 0;}
  for (auto it = data.begin(); it != data.end(); it++)
  {
    if (channel == 1) {(*it).x = 0;}
    else if (channel == 2) {(*it).y = 0;}
    else if (channel == 3) {(*it).z = 0;}
    else if (channel == 4) {(*it).w = 0;}
  }
}
void ie::RenderUnit::clearAttrib(IEenum attrib)
{
  if (attrib == IE_ELEMENT_ATTRIB) {indices.clear();}
  else if (attrib == IE_POSITION_ATTRIB) {positions.clear();}
  else if (attrib == IE_NORMAL_ATTRIB) {normals.clear();}
  else if (attrib == IE_MATERIAL_ATTRIB) {maps.clear();}
  else if (attrib == IE_COLOR_ATTRIB) {colors.clear();}
  else if (attrib == IE_DATA_ATTRIB) {data.clear();}
  else if (attrib == IE_DATA1_ATTRIB) {clearDataChannel(1);}
  else if (attrib == IE_DATA2_ATTRIB) {clearDataChannel(2);}
  else if (attrib == IE_DATA3_ATTRIB) {clearDataChannel(3);}
  else if (attrib == IE_DATA4_ATTRIB) {clearDataChannel(4);}
}


unsigned int ie::RenderUnit::getAttribAmount(IEenum attrib)
{
  if (attrib == IE_ELEMENT_ATTRIB) {return indices.size();}
  else if (attrib == IE_POSITION_ATTRIB) {return positions.size();}
  else if (attrib == IE_NORMAL_ATTRIB) {return normals.size();}
  else if (attrib == IE_MATERIAL_ATTRIB) {return maps.size();}
  else if (attrib == IE_COLOR_ATTRIB) {return colors.size();}
  else if (attrib == IE_DATA_ATTRIB) {return data.size();}
  else if (attrib == IE_DATA1_ATTRIB) {return dataChannelAmount.x;}
  else if (attrib == IE_DATA2_ATTRIB) {return dataChannelAmount.y;}
  else if (attrib == IE_DATA3_ATTRIB) {return dataChannelAmount.z;}
  else if (attrib == IE_DATA4_ATTRIB) {return dataChannelAmount.w;}
}


void ie::RenderUnit::clearAllAttribs(void)
{
    indices.clear();
    positions.clear();
    normals.clear();
    maps.clear();
    colors.clear();
    dataChannelAmount = glm::uvec4(0, 0, 0, 0);
    data.clear();
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
  if (data.size() > vertex)
  {
    data.erase(data.begin() + vertex);
  }
}


unsigned int ie::RenderUnit::removeDuplicates(float sensitivity)
{
  unsigned int duplicateAmount = 0;
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
        bool isDataDuplicate;
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
        if (data.size() > 0)
        {
          isDataDuplicate = getDataAttrib((*i)) == getDataAttrib(vertexArrayIndex);
        } else {isDataDuplicate = true;}

        bool isVertexDuplicate = isNormalDuplicate && isMapDuplicate &&
                                 isColorDuplicate && isDataDuplicate;

        if (isVertexDuplicate)
        {
          duplicateAmount++;
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
  return duplicateAmount;
}

//______________________________________________________________________________
