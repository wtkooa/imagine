//___|"staticVShader130.glsl"|__________________________________________________
//
// Project: Imagine: 3D Environment Engine
// Version: 0.1.0
// Author: David Lipps
// License: MIT License
//
// Copyright (c) 2017 David E Lipps
//______________________________________________________________________________

#version 130

in vec3 position;
in vec2 texture;
in vec3 normal;

uniform mat4 mtwMatrix;
uniform mat4 transformationMatrix;

out vec3 mtwPosition;
out vec2 texturePipe;
out vec3 mtwNormal;

void main()
{
  gl_Position = transformationMatrix * vec4(position, 1.0);

  //PASSING MODEL TO WORLD TRANSFORMED VECTORS TO FRAG SHADER
  mtwPosition = vec3(mtwMatrix * vec4(position, 1.0));
  mtwNormal = vec3(mtwMatrix * vec4(normal, 0.0));

  //PASSING TEXTURE COORDS TO FRAG SHADER
  texturePipe = texture;
}
