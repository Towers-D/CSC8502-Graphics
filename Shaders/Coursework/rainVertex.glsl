//Author David Towers (160243066)
#version 150 core

 in vec3 position;
 in vec2 texCoord;


out Vertex {
	vec2 texCoord;
} OUT;

void main(void) {
	gl_Position = vec4(position, 1.0);
	OUT.texCoord = texCoord;
}