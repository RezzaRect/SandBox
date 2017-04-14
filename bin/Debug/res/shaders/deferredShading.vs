#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoord;
//layout(location = 2) in vec3 normal;


out vec2 texCoord0;


void main()
{
    gl_Position = vec4(position.x, position.y, 0.0, 1.0);
    //mat4 mvp = projection * view * model;
    //mat4 inverseCurrentMVP = inverse(mvp);
    texCoord0 = texCoord;
    //blurVec = 
}

