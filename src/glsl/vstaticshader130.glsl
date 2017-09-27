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
