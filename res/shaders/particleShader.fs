#version 330 core

// Interpolated values from the vertex shaders
in vec2 UV;
in vec4 particlecolor;

// Ouput data
out vec4 color;

uniform sampler2D myTextureSampler;

void main(){
	// Output color = color of the texture at the specified UV
	color = texture( myTextureSampler, vec2(UV.x, UV.y*-1.0) ) * vec4(particlecolor.rgba);
	//vec4(vec3(1.0), particlecolor.a) * 
	//color.a = 1.0;
}