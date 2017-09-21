#version 450

in vec3 mtwPosition;
in vec3 texturePipe;
in vec3 mtwNormal;

uniform vec3 cameraPos;

uniform vec3 pointLightPos;
uniform vec3 globalAmbient;
uniform vec3 lightAmbient;
uniform vec3 lightSpecular;
uniform vec3 lightDiffuse; 
uniform float lightConstantFalloff;
uniform float lightLinearFalloff;
uniform float lightQuadraticFalloff;

uniform vec3 materialSpecular;
uniform float materialShininess;
uniform vec3 materialDiffuse;
uniform vec3 materialAmbient;
uniform vec3 materialEmission;

uniform sampler2D textureID;
uniform int usingTexture;

out vec4 fragmentColor;

void main()
{

    //CALCULATING DIFFUSE LIGHT TERM
    vec3 L = normalize(pointLightPos - mtwPosition);
    float diffuseCos = clamp(dot(L, normalize(mtwNormal)), 0, 1);
    vec4 diffuseCalc = vec4(diffuseCos, diffuseCos, diffuseCos, 1.0);

    //CALCULATING SPECULAR LIGHT TERM
    vec3 P = normalize(cameraPos - mtwPosition);
    vec3 S = normalize(P + L);
    float shine = pow(clamp(dot(S, normalize(mtwNormal)), 0, 1), materialShininess);
    vec4 specular = shine * vec4(lightSpecular, 1.0) * vec4(materialSpecular, 1.0);

    //CALCULATING FALLOFF
    float distance = length(pointLightPos - mtwPosition);
    float falloff = 1 / (lightConstantFalloff +
                        (lightLinearFalloff * distance) +
                        (lightQuadraticFalloff * distance * distance));

    //CALCULATING REMAINING PHONG SHADING TERMS
    vec4 emission = vec4(materialEmission, 1.0);    
    vec4 globalAmbient = vec4(globalAmbient, 1.0) * vec4(materialAmbient, 1.0);
    vec4 lightAmbient = vec4(lightAmbient, 1.0) * vec4(materialAmbient, 1.0);
    vec4 diffuse = diffuseCalc * vec4(lightDiffuse, 1.0) * vec4(materialDiffuse, 1.0);

    if (usingTexture == 0) //NO TEXTURE: PREFORM STANDARD PHONG SHADING
    {
        fragmentColor = (emission + globalAmbient + falloff) * (lightAmbient + diffuse + specular);
    }
    else //HAS TEXTURE: PREFORM SPLIT SPECULAR AND TEXTURE BLENDING
    {
        vec4 textureColor = texture(textureID, texturePipe.xy);
        vec4 primaryColor = (emission + globalAmbient + falloff) * (lightAmbient + diffuse);
        vec4 secondaryColor = falloff * specular;
        fragmentColor = (primaryColor * textureColor) + secondaryColor;
    }
}
