#version 450

in vec3 modeltoworldPositionPipeline;
in vec3 modeltoworldNormalPipeline;
in vec3 colorPipeline;

uniform vec3 light0pos;
uniform vec3 ambientLight;

out vec4 fragmentColor;

void main()
{
    vec3 tolightVector = normalize(light0pos - modeltoworldPositionPipeline);
    float diffuseCos = clamp(dot(tolightVector, normalize(modeltoworldNormalPipeline)), 0, 1);
    vec4 diffuseLight = vec4(diffuseCos, diffuseCos, diffuseCos, 1.0);
    fragmentColor = vec4(colorPipeline, 1.0) * (vec4(ambientLight, 1.0) + diffuseLight);
}
