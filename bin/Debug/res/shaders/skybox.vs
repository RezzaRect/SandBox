#version 330 core
layout (location = 0) in vec3 position;

out vec3 TexCoords;
out vec3 camToPixel;

uniform mat4 projection;
uniform mat4 view;
uniform vec3 viewPos;

void main()
{
    vec4 pos = projection * view * vec4(position, 1.0);
    gl_Position = pos.xyww;
    TexCoords = position;
    camToPixel = position - viewPos;
}