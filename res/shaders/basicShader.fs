#version 330

//uniform sampler2D specularTexture;
//uniform sampler2D texture_diffuse1;
//uniform sampler2D texture_specular1;
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
    // create true false 1...0 uniform to toggle normalmapping
    //vec3 normal = normalize(tbnMatrix * (255./128. * texture2D(normalMap, texCoord0.xy).xyz - 1.0));
    
    
    //vec3 normal = normalize(tbnMatrix * (255.0/128.0 * texture2D(normalMap, texCoord0).xyz - 1));
    
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
    //color*tx.rgb* , 1.0
    //vec4 sp = texture2D(specularTexture, texCoord0)*specular;
    vec4 tx = texture2D(diffuse, texCoord0);
    finalColor = pow(vec4(tx.rgb*dif+specular, 1.0), vec4(vec3(1.0/1.2),1.0)); //pow(specular*texture2D(texture_specular1, texCoord0) + dif * texture2D(texture_diffuse1, texCoord0), vec4(vec3(1.0/1.4), 1.0)); //vec4(vec3(0.5),1.0) //pow(texture2D(diffuseTexture, texCoord0) * dif + specular, vec4(vec3(1.0/1.8), 1.0));// * texture2D(diffuseTexture, texCoord0); //vec2(0.32,0.12));
}