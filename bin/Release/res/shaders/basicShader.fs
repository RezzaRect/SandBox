#version 330

uniform sampler2D specularTexture;
uniform sampler2D diffuse;
uniform sampler2D normalMap;

uniform vec3 viewPos;
uniform float time;
uniform vec3 randColor;
uniform int useNormalMap;

in vec2 texCoord0;
in vec3 normal0;
in vec3 fragPos0;
in mat3 tbnMatrix;

out vec4 finalColor;

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;

void main()
{
    vec3 lightDir = normalize(vec3(0.2,0.294,-0.3));
    
    vec3 normal = normal0;

    float dif = clamp(dot(normal, lightDir), 0.175, 1.0);
    
    // Phong
    vec3 viewDir = normalize(viewPos - fragPos0);
    //vec3 reflectDir = reflect(-lightPos, normal0);
    //float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.);
    //float specular = 0.0;
    // Blinn-Phong
    //vec3 viewDir
    vec3 halfDir = normalize(lightDir + viewDir);
    //if(dot(normal0, lightDir) > 0.0){
    float specAngle = max(dot(halfDir, normal), 0.0);
    float specular = pow(specAngle, 128.0);
    //}

    vec4 tex = texture2D(diffuse, texCoord0);
    finalColor = pow(vec4(tex.rgb*dif+specular, 1.0), vec4(vec3(1.0/1.2),1.0));
    
    //gAlbedoSpec = tex; //vec4(normal0, 1.0);//tex;
    //gAlbedoSpec.a = specular; //specular;
    //gNormal.xyz = normalize(vec4(normal0, 1.0)).xyz;
    //gPosition = fragPos0; 
}