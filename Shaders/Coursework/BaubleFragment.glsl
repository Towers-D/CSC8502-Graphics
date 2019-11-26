//Author David Towers (160243066)
#version 150 core
uniform sampler2D diffuseTex;

in Vertex {
	vec2 texCoord;
} IN;

out vec4 FragColor;

void main(void){
	FragColor = vec4(1, 1, 0, 1);
}