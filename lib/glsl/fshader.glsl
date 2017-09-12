#version 450

in vec3 mtwPosition;
in vec3 texturePipe;
in vec3 mtwNormal;

uniform vec3 light0pos;
uniform vec3 eyePos;
uniform vec3 ambientGlobal; //Global Ambient Light
uniform vec3 ambientLight;  //Ambient from Light Source
uniform vec3 specularLight; //Specular Light Color
uniform vec3 diffuseLight;  //Diffuse Light Color
uniform vec3 Ks;            //Material Specular
uniform float Ns;           //Material Shininess
uniform vec3 Kd;            //Material Diffuse
uniform vec3 Ka;            //Material Ambient
uniform vec3 Ke;            //Material Emission
uniform float Kc;           //Falloff Constant
uniform float Kl;           //Falloff Linear
uniform float Kq;           //Falloff Quadratic
uniform sampler2D texID;
uniform int hasTexture;

out vec4 fragmentColor;

void main()
{

    //CALCULATING DIFFUSE LIGHT TERM
    vec3 L = normalize(light0pos - mtwPosition);
    float diffuseCos = clamp(dot(L, normalize(mtwNormal)), 0, 1);
    vec4 diffuseCalc = vec4(diffuseCos, diffuseCos, diffuseCos, 1.0);

    //CALCULATING SPECULAR LIGHT TERM
    vec3 P = normalize(eyePos - mtwPosition);
    vec3 S = normalize(P + L);
    float shine = pow(clamp(dot(S, normalize(mtwNormal)), 0, 1), Ns);
    vec4 specular = shine * vec4(specularLight, 1.0) * vec4(Ks, 1.0);

    //CALCULATING FALLOFF
    float distance = length(light0pos - mtwPosition);
    float falloff = 1 / (Kc + (Kl*distance) + (Kq * distance * distance));

    //CALCULATING REMAINING PHONG SHADING TERMS
    vec4 emission = vec4(Ke, 1.0);    
    vec4 ambientGlobal = vec4(ambientGlobal, 1.0) * vec4(Ks, 1.0);
    vec4 ambientSource = vec4(ambientLight, 1.0) * vec4(Ks, 1.0);
    vec4 diffuse = diffuseCalc * vec4(diffuseLight, 1.0) * vec4(Kd, 1.0);

    if (hasTexture == 0) //NO TEXTURE: PREFORM STANDARD PHONG SHADING
    {
        fragmentColor = (emission + ambientGlobal + falloff) * (ambientSource + diffuse + specular);
    }
    else //HAS TEXTURE: PREFORM SPLIT SPECULAR AND TEXTURE BLENDING
    {
        vec4 tex = texture(texID, texturePipe.xy);
        vec4 primary = (emission + ambientGlobal + falloff) * (ambientSource + diffuse);
        vec4 secondary = falloff * specular;
        fragmentColor = (primary * tex) + secondary;
    }
}









