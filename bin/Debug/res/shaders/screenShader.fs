#version 330 core

uniform sampler2D diffuse;

in vec2 texCoord0;

out vec4 finalColor;

void main()
{
	finalColor = texture2D(diffuse, texCoord0);
}
