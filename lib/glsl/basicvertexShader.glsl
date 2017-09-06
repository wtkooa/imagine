#version 450

in layout(location=0) vec3 position;
in layout(location=1) vec3 vertexColor;
in layout(location=2) vec3 normal;

uniform mat4 modeltoworldMatrix;
uniform mat4 transformationMatrix;

out vec3 colorPipeline;
out vec3 modeltoworldPositionPipeline;
out vec3 modeltoworldNormalPipeline;

void main()
{
    gl_Position = transformationMatrix * vec4(position, 1.0);
    modeltoworldPositionPipeline = vec3(modeltoworldMatrix * vec4(position, 1.0));
    modeltoworldNormalPipeline = vec3(modeltoworldMatrix * vec4(normal, 0.0));
    colorPipeline = vertexColor;
}
