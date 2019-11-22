#version 400 core

layout(triangles, equal_spacing, ccw) in;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform sampler2D diffuseTex;
uniform sampler2D moveTex;
uniform float time;
uniform float defy;

in Vertex {
    vec4 colour;
    vec2 texCoord;
} IN[];

out Vertex {
    vec4 colour;
    vec2 texCoord;
    vec3 binormal;
    vec3 tangent;
    vec3 normal;
    vec3 worldPos;
} OUT;



void main()	{

}