//___|"ie_terrainFShader450.glsl"|______________________________________________
//
// Project: Imagine: 3D Environment Engine
// Version: 0.1.0
// Author: David Lipps
// License: MIT License
//
// Copyright (c) 2017 David E Lipps
//______________________________________________________________________________

#version 450

in vec3 mtwPosition;
in vec2 texturePipe;
in vec3 mtwNormal;

uniform vec3 cameraPos;

uniform vec3 pointLightPos;
uniform vec3 globalAmbient;
uniform vec3 lightAmbient;
uniform vec3 lightSpecular;
uniform vec3 lightDiffuse; 
uniform float lightConstantFalloff;
uniform float lightLinearFalloff;
uniform float lightQuadraticFalloff;

out vec4 fragmentColor;

void main()
{
  fragmentColor = vec4(0.6, 0.6, 0.6, 1.0);
}
