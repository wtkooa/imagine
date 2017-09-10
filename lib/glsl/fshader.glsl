#version 450

in vec3 mtwPosition;
in vec3 mtwNormal;

uniform vec3 light0pos;
uniform vec3 ambientLight;

out vec4 fragmentColor;

void main()
{
    vec3 tolightVector = normalize(light0pos - mtwPosition);
    float diffuseCos = clamp(dot(tolightVector, normalize(mtwNormal)), 0, 1);
    vec4 diffuseLight = vec4(diffuseCos, diffuseCos, diffuseCos, 1.0);
    fragmentColor = vec4(1.0, 1.0, 1.0, 1.0) * (vec4(ambientLight, 1.0) + diffuseLight);
}
