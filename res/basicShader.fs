#version 330

uniform sampler2D diffuse;
uniform vec3 viewPos;

in vec2 texCoord0;
in vec3 normal0;
in vec3 fragPos0;

out vec4 finalColor;

void main()
{
    // lighPos for pointlight lightDir for directional light
    vec3 lightDir = normalize(vec3(0.2,0.45,0.3));

    float dif = clamp(dot(normal0 ,lightDir), 0.15, 1.0);
    
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


    finalColor = specular + dif * texture2D(diffuse, texCoord0); //vec2(0.32,0.12));
}