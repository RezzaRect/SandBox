#version 330 core

out vec4 color;

//uniform sampler2D screenTexture;
uniform sampler2D gAlbedoSpec;
uniform sampler2D gNormal;
uniform sampler2D gPosition;
/*
struct Light {
    vec3 Position;
    vec3 Color;

    float Linear;
    float Quadratic;
    float Radius;
};

const int NR_LIGHTS = 20;
uniform Light lights[NR_LIGHTS];


struct Light {
    vec3 Position;
    vec3 Color;
    
    float Linear;
    float Quadratic;
    float Radius;
};
const int NR_LIGHTS = 30;
uniform Light lights[NR_LIGHTS];
*/


uniform float time;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 viewPos;

uniform vec3 camDir;

in vec2 texCoord0;


void main()
{

    // Retrieve data from gbuffer
    vec3 FragPos = texture(gPosition, texCoord0).rgb;
    vec3 Normal = texture(gNormal, texCoord0).rgb;
    vec3 Diffuse = texture(gAlbedoSpec, texCoord0).rgb;
    float Specular = texture(gAlbedoSpec, texCoord0).a;
    
    // Then calculate lighting as usual
    vec3 lighting  = Diffuse * 0.1; // hard-coded ambient component
    vec3 viewDir  = normalize(viewPos - FragPos);
    // light color and light direction
    vec3 lightColor = vec3(0.3,0.6,0.7);
    vec3 lightDir = normalize(vec3(0.2,0.294,-0.3));
    
    vec3 diffuse = max(dot(Normal, lightDir), 0.0) * Diffuse * lightColor;
    // Specular
    vec3 halfwayDir = normalize(lightDir + camDir);  // camDir..?
    float spec = pow(max(dot(Normal, halfwayDir), 0.0), 116.0);
    vec3 specular = lightColor * spec;// * Specular;
    
    lighting += diffuse + specular;

    vec4 gama = vec4(vec3(1.0/1.75), 1.0);
    if(texCoord0.x < .5)
    color = pow(vec4(Normal, 1.0), gama);
    if(texCoord0.x > .5)
    color = vec4(Diffuse, 1.0);
} 
