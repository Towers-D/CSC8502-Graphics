//Author David Towers (160243066)
#version 150 core
uniform sampler2D diffuseTex;

in Vertex {
	vec2 texCoord;
	float render;
} IN;

out vec4 gl_FragColor;

void main(void){
	gl_FragColor = texture(diffuseTex, IN.texCoord);
	gl_FragColor.a = IN.render;
}