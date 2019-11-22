# version 330 core

uniform float particleSize = 0.33f;
uniform mat4 modelMatrix = mat4 (1.0f);
uniform mat4 viewMatrix = mat4 (1.0f);
uniform mat4 projMatrix = mat4 (1.0f);

uniform float shatter = 0f;
layout(triangles) in;
layout (triangle_strip, max_vertices = 3) out;



in Vertex {
	vec2 texCoord;
} IN[];

out Vertex {
	vec2 texCoord;
} OUT;

void main(){
	mat4 mvp = (projMatrix * viewMatrix * modelMatrix);

		for (int i = 0; i < gl_in.length (); ++ i ) {
			gl_Position = mvp * gl_in [i].gl_Position ;
			
			OUT.texCoord = IN [i].texCoord ;
			EmitVertex ();
		}
}