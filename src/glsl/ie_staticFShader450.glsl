//___|"staticFShader450.glsl"|__________________________________________________
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

uniform sampler2D textureId;
uniform int usingTexture;

out vec4 fragmentColor;

void main()
{

  //SAMPLING TEXTURE
  vec4 textureColor = vec4(0.0);
  if (usingTexture == 1)
  {
    textureColor = texture(textureId, texturePipe.xy);
  }

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
  vec4 combinedDiffuse = vec4(materialDiffuse, 1.0) + textureColor;
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
