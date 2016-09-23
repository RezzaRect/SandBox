#version 330

uniform sampler2D diffuse_texture1;
uniform sampler2D specular_texture1;
uniform vec3 viewPos;
uniform float time;

in vec2 texCoord0;
in vec3 normal0;
in vec3 fragPos0;
//varying vec3 vertPos;

out vec4 finalColor;
mat2 r2D(float a){
    float s = sin(a);
    float c = cos(a);
    return mat2(s, -c, c, s);
}
void main()
{
    vec3 lightPos = normalize(vec3(0.2,0.8,-0.3));
    lightPos.xz *= r2D(time);

    float dif = clamp(dot(normal0 ,lightPos), 0.15, 1.0);
    // Phong
    vec3 viewDir = normalize(viewPos - fragPos0);
    //vec3 reflectDir = reflect(-lightPos, normal0);
    //float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.);
    // Blinn-Phong
    //vec3 viewDir
    vec3 halfDir = normalize(lightPos + viewDir);
    float specAngle = max(dot(halfDir, normal0), 0.0);
    float specular = pow(specAngle, 128.0);


    finalColor = specular * texture2D(specular_texture1, texCoord0) + dif * texture2D(diffuse_texture1, texCoord0); //vec2(0.32,0.12));
}