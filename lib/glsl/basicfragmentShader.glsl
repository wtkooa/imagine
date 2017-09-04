#version 450

out vec4 fragmentColor;
in vec3 colorPipeline;

void main()
{
    fragmentColor = vec4(colorPipeline, 1.0);
}
