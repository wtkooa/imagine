//___|"ie_terrainFShader130.glsl"|______________________________________________
//
// Project: Imagine: 3D Environment Engine
// Version: 0.1.0
// Author: David Lipps
// License: MIT License
//
// Copyright (c) 2017 David E Lipps
//______________________________________________________________________________

#version 130

in vec3 mtwPosition;
in vec2 texturePipe;
in vec3 mtwNormal;
in vec3 colorPipe;
in mat3 blendPipe;

uniform vec3 cameraPos;

uniform vec3 pointLightPos;
uniform vec3 globalAmbient;
uniform vec3 lightAmbient;
uniform vec3 lightSpecular;
uniform vec3 lightDiffuse; 
uniform float lightConstantFalloff;
uniform float lightLinearFalloff;
uniform float lightQuadraticFalloff;
uniform int usesGlobalAmbient;
uniform int usesLightAmbient;
uniform int usesLightDiffuse;
uniform int usesLightSpecular;
uniform int usesLightFalloff;

uniform float materialShininess;
uniform vec3 materialSpecular;
uniform vec3 materialDiffuse;
uniform vec3 materialAmbient;
uniform vec3 materialEmission;

uniform sampler2D texture1;
uniform sampler2D texture2;
uniform sampler2D texture3;
uniform sampler2D texture4;
uniform sampler2D texture5;
uniform sampler2D texture6;
uniform sampler2D texture7;
uniform sampler2D texture8;
uniform int textureAmount; 

out vec4 fragmentColor;

void main()
{

  //SAMPLING TEXTURES AND APPLYING BLEND RATIOS
  float b1 = blendPipe[0][0];
  float b2 = blendPipe[1][0];
  float b3 = blendPipe[2][0];
  float b4 = blendPipe[0][1];
  float b5 = blendPipe[1][1];
  float b6 = blendPipe[2][1];
  float b7 = blendPipe[0][2];
  float b8 = blendPipe[1][2];

  vec4 t1c = vec4(0.0);
  vec4 t2c = vec4(0.0);
  vec4 t3c = vec4(0.0);
  vec4 t4c = vec4(0.0);
  vec4 t5c = vec4(0.0);
  vec4 t6c = vec4(0.0);
  vec4 t7c = vec4(0.0);
  vec4 t8c = vec4(0.0);
  int n = 0;
  if (n < textureAmount)
  {
   t1c = texture(texture1, texturePipe.xy);
   t1c *= b1;
   n++;
  }
  if (n < textureAmount)
  {
   t2c = texture(texture2, texturePipe.xy);
   t2c *= b2;
   n++;
  }
  if (n < textureAmount)
  {
   t3c = texture(texture3, texturePipe.xy);
   t3c *= b3;
   n++;
  }
  if (n < textureAmount)
  {
   t4c = texture(texture4, texturePipe.xy);
   t4c *= b4;
   n++;
  }
  if (n < textureAmount)
  {
   t5c = texture(texture5, texturePipe.xy);
   t5c *= b5;
   n++;
  }
  if (n < textureAmount)
  {
   t6c = texture(texture6, texturePipe.xy);
   t6c *= b6;
   n++;
  }
  if (n < textureAmount)
  {
   t7c = texture(texture7, texturePipe.xy);
   t7c *= b7;
   n++;
  }
  if (n < textureAmount)
  {
   t8c = texture(texture8, texturePipe.xy);
   t8c *= b8;
  }
  
  vec4 textureColor = t1c + t2c + t3c + t4c + t5c + t6c + t7c + t8c;


  //CALCULATING DIFFUSE LIGHT COSINE 
  vec3 L = normalize(pointLightPos - mtwPosition);
  float diffuseCos = clamp(dot(L, normalize(mtwNormal)), 0, 1);
  vec4 diffuseCalc = vec4(diffuseCos, diffuseCos, diffuseCos, 1.0);

  //CALCULATING SPECULAR LIGHT TERM
  vec3 P = normalize(cameraPos - mtwPosition);
  vec3 S = normalize(P + L);
  float shine = pow(clamp(dot(S, normalize(mtwNormal)), 0, 1), materialShininess);
  vec4 specular = shine * vec4(lightSpecular, 1.0) * vec4(materialSpecular, 1.0);

  //CALCULATING FALLOFF
  float distance = length(pointLightPos - mtwPosition);
  float falloff = 1 / (lightConstantFalloff +
                      (lightLinearFalloff * distance) +
                      (lightQuadraticFalloff * distance * distance));

  //CALCULATING REMAINING PHONG SHADING TERMS
  vec4 combinedDiffuse = vec4(materialDiffuse, 1.0) + textureColor +
                         vec4(colorPipe, 1.0);
  vec4 emission = vec4(materialEmission, 1.0);    
  vec4 globalAmbient = vec4(globalAmbient, 1.0) * vec4(materialAmbient, 1.0) *
                       combinedDiffuse;
  vec4 lightAmbient = vec4(lightAmbient, 1.0) * vec4(materialAmbient, 1.0) *
                      combinedDiffuse;
  vec4 diffuse = diffuseCalc * vec4(lightDiffuse, 1.0) * combinedDiffuse;

  //LIGHTING OPT-OUT
  if (usesGlobalAmbient != 1)
  {
    globalAmbient = vec4(0.0);
  }
  if (usesLightAmbient != 1)
  {
    lightAmbient = vec4(0.0);
  }
  if (usesLightDiffuse != 1)
  {
    diffuse = vec4(materialDiffuse, 1.0) + textureColor;
  }
  if (usesLightSpecular != 1)
  {
    specular = vec4(0.0);
  }
  if (usesLightFalloff != 1)
  {
    falloff = 1.0;
  }

  fragmentColor = (emission + globalAmbient) + 
                   (falloff * (lightAmbient + diffuse + specular));
}
