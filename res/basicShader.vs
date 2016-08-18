#version 330

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec3 normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 texCoord0;
out vec3 normal0;
out vec3 fragPos0;

void main()
{

    mat4 mvp = projection * view * model;
    gl_Position = mvp * vec4(position, 1.0);
    texCoord0 = texCoord;
    normal0 = (model * vec4(normal, 0.0)).xyz;
    //world position..?
    fragPos0 = (model * vec4(position, 1.0)).xyz;
}