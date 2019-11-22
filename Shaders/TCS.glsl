#version 400 core

layout (vertices = 3) out;

uniform int tli;
uniform int tlo;

in Vertex {
    vec4 colour;
    vec2 texCoord;
} IN[];

out Vertex {
    vec4 colour;
    vec2 texCoord;
} OUT[];

void main() {
    gl_TessLevelInner [0] = tli;
    gl_TessLevelInner [1] = tli;                       
	gl_TessLevelOuter [0] = tlo;
	gl_TessLevelOuter [1] = tlo;
	gl_TessLevelOuter [2] = tlo;
    gl_TessLevelOuter [3] = tlo;

    OUT[gl_InvocationID].texCoord = IN[gl_InvocationID].texCoord;
    OUT[gl_InvocationID].colour = IN[gl_InvocationID].colour;
	gl_out[gl_InvocationID].gl_Position = gl_in [gl_InvocationID].gl_Position;
}