//___|"ie_scenegraph.cpp"|______________________________________________________
//
// Project: Imagine: 3D Environment Engine
// Version: 0.1.0
// Author: David Lipps
// License: MIT License
//
// Copyright (c) 2017 David E Lipps
//______________________________________________________________________________

#include "ie_scenegraph.h"

#include <iostream>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "ie_assets.h"

//___|BASE GRAPH NODE IMPLEMENTATION|___________________________________________

ie::GraphNode::GraphNode()
{
  std::cout << "Initializing GraphNode Stuff"  << std::endl;  
  translation = glm::vec3(0.0f);
  rotation = glm::vec3(0.0f);
  scale = glm::vec3(0.0f);
  transformation = glm::mat4();
  parentNode = NULL;
}

ie::GraphNode::~GraphNode()
{
  for (auto it = children.begin(); it != children.end(); it++)
  {
    delete *it;
  }
}

void ie::GraphNode::addChild(GraphNode* child)
{
  child->setParentNode(this); 
  children.push_back(child);
}

void ie::GraphNode::update(void)
{
  glm::mat4 transl = glm::translate(glm::mat4(), translation);
  glm::mat4 scal = glm::scale(glm::mat4(), scale);
  glm::mat4 rotX = glm::rotate(glm::mat4(), rotation.x,
                               glm::vec3(1.0f, 0.0f, 0.0f));
  glm::mat4 rotY = glm::rotate(glm::mat4(), rotation.y,
                               glm::vec3(0.0f, 1.0f, 0.0f));
  glm::mat4 rotZ = glm::rotate(glm::mat4(), rotation.z,
                               glm::vec3(0.0f, 0.0f, 1.0f));
  glm::mat4 rotate = rotX * rotY * rotZ;
  transformation = transl * rotate * scal;
  if (parentNode)
  {
    transformation = parentNode->transformation * transl * rotate * scal;
  }
  for (auto it = children.begin(); it != children.end(); it++)
  {
    (*it)->update();
  }
}

void ie::GraphNode::render(void)
{
  for (auto it = children.begin(); it != children.end(); it++)
  {
    (*it)->render();
  }
}

void ie::GraphNode::setParentNode(GraphNode* parent)
{
  parentNode = parent;
}

//______________________________________________________________________________

//___|ENTITY GRAPH NODE IMPLEMENTATION|_________________________________________

ie::EntityNode::EntityNode()
{
  std::cout << "Initializing Entity Stuff"  << std::endl; 
  name = "None";
  type = NONE;
  assetId = 0;
  hidden = false;
  usesGlobalAmbient = true;
  usesLightAmbient = true;
  usesLightDiffuse = true;
  usesLightSpecular = true;
  usesLightFalloff = true;
}




