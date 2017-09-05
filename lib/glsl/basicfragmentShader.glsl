#version 450

out vec4 fragmentColor;
in vec3 colorPipeline;

uniform vec3 ambientLight;

void main()
{
    fragmentColor = vec4(colorPipeline, 1.0) * vec4(ambientLight, 1.0);
}
