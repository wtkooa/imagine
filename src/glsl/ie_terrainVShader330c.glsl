//___|"ie_terrainVShader450.glsl"|______________________________________________
//
// Project: Imagine: 3D Environment Engine
// Version: 0.1.0
// Author: David Lipps
// License: MIT License
//
// Copyright (c) 2017 David E Lipps
//______________________________________________________________________________

#version 330 core

in layout(location=0) vec3 position;
in layout(location=1) vec2 texture;
in layout(location=2) vec3 normal;
in layout(location=3) vec3 color;
in layout(location=4) uvec2 blend;

uniform mat4 mtwMatrix;
uniform mat4 transformationMatrix;

out vec3 mtwPosition;
out vec2 texturePipe;
out vec3 mtwNormal;
out vec3 colorPipe;
out mat3 blendPipe;

void main()
{
  float demod = 100.0;

  uint udata = blend.x;
  float fdata = uintBitsToFloat(udata);
  float b = mod(fdata, demod); 
  float b4 = b / demod;
  fdata = trunc(fdata / demod);

  b = mod(fdata, demod); 
  float b3 = b / demod;
  fdata = trunc(fdata / demod);

  b = mod(fdata, demod); 
  float b2 = b / demod;
  fdata = trunc(fdata / demod);

  b = mod(fdata, demod); 
  float b1 = b / demod;
  fdata = trunc(fdata / demod);

  udata = blend.y;
  fdata = uintBitsToFloat(udata);
  b = mod(fdata, demod); 
  float b8 = b / demod;
  fdata = trunc(fdata / demod);

  b = mod(fdata, demod); 
  float b7 = b / demod;
  fdata = trunc(fdata / demod);

  b = mod(fdata, demod); 
  float b6 = b / demod;
  fdata = trunc(fdata / demod);

  b = mod(fdata, demod); 
  float b5 = b / demod;
  fdata = trunc(fdata / demod);

  mat3 blends;
  blends[0][0] = b1;
  blends[1][0] = b2;
  blends[2][0] = b3;
  blends[0][1] = b4;
  blends[1][1] = b5;
  blends[2][1] = b6;
  blends[0][2] = b7;
  blends[1][2] = b8;
  blendPipe = blends;

  gl_Position = transformationMatrix * vec4(position, 1.0);

  //PASSING MODEL TO WORLD TRANSFORMED VECTORS TO FRAG SHADER
  mtwPosition = vec3(mtwMatrix * vec4(position, 1.0));
  mtwNormal = vec3(mtwMatrix * vec4(normal, 0.0));

  //PASSING TEXTURE COORDS TO FRAG SHADER
  texturePipe = texture;

  //PASSING PER VERTEX COLOR TO FRAG SHADER
  colorPipe = color;
}
