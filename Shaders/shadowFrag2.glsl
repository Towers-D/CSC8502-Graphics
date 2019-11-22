#version 150 core

uniform sampler2D diffuseTex;

out vec4 fragColour;

in Vertex {
    vec4 colour;
    vec2 texCoord;
} IN;

void main(void) {
    fragColour = (texture(diffuseTex, IN.texCoord) ) * vec4(0,1,0,1);
	if (fragColour.x < 0.01 && fragColour.y < 0.01 && fragColour.z < 0.01 )
		fragColour = vec4(0.396,0.263,0.129,1);
}