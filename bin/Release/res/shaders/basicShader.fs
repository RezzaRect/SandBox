#version 330

uniform sampler2D specularTexture;
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform vec3 viewPos;
uniform float time;

in vec2 texCoord0;
in vec3 normal0;
in vec3 fragPos0;

out vec4 finalColor;

mat2 rot2D(float a){
    float s = sin(a);
    float c = cos(a);
    return mat2(s, -c, c, s);
}

void main()
{
    // lighPos for pointlight lightDir for directional light
    vec3 lightDir = normalize(vec3(0.2,0.294,-0.3));
    //lightDir.xz *= rot2D(time);

    float dif = clamp(dot(normal0, lightDir), 0.075, 1.0);
    
    // Phong
    vec3 viewDir = normalize(viewPos - fragPos0);
    //vec3 reflectDir = reflect(-lightPos, normal0);
    //float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.);
    //float specular = 0.0;
    // Blinn-Phong
    //vec3 viewDir
    vec3 halfDir = normalize(lightDir + viewDir);
    //if(dot(normal0, lightDir) > 0.0){
    float specAngle = max(dot(halfDir, normal0), 0.0);
    float specular = pow(specAngle, 128.0);
    //}


    finalColor = pow(specular*texture2D(texture_specular1, texCoord0) + dif * texture2D(texture_diffuse1, texCoord0), vec4(vec3(1.0/1.4), 1.0)); //vec4(vec3(0.5),1.0) //pow(texture2D(diffuseTexture, texCoord0) * dif + specular, vec4(vec3(1.0/1.8), 1.0));// * texture2D(diffuseTexture, texCoord0); //vec2(0.32,0.12));
}