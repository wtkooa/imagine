#version 450

in layout(location=0) vec3 position;
in layout(location=1) vec3 texture;
in layout(location=2) vec3 normal;

uniform mat4 mtwMatrix;
uniform mat4 transformationMatrix;

out vec3 mtwPosition;
out vec3 texturePipe;
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