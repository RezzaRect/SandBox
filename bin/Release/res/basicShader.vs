#version 120

attribute vec3 position;
attribute vec2 texCoord;
attribute vec3 normal;

// just View Projection no model or
// transform really... :/ :D
//uniform mat4 MVP;//viewProj;
//uniform mat4 transform;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

varying vec2 texCoord0;
varying vec3 normal0;
//varying vec3 vertPos;
varying vec3 fragPos0;

void main()
{

    mat4 mvp = projection * view * model;
    gl_Position = mvp * vec4(position, 1.0);
    texCoord0 = texCoord;
    normal0 = (model * vec4(normal, 0.0)).xyz;
    //world position..?
    fragPos0 = (model * vec4(position, 1.0)).xyz;
}