#version 450

in layout(location=0) vec3 position;
in layout(location=1) vec3 vertexColor;

uniform mat4 transformationMatrix;

out vec3 colorPipeline;

void main()
{
    gl_Position = transformationMatrix * vec4(position, 1.0);
    colorPipeline = vertexColor;
}
