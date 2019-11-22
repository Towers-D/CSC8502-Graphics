#version 150 core

uniform mat4 modelMatrix;
uniform mat4 textureMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

in vec3 position;
in vec2 texCoord;
in vec4 colour;
in vec3 normal;
in vec3 tangent;

out Vertex {
    vec4 colour;
    vec2 texCoord;
} OUT;

void main(void) {
    gl_Position = (projMatrix * viewMatrix * modelMatrix) * vec4(position, 1.0);
	OUT.texCoord = texCoord;
	OUT.colour = colour;
}