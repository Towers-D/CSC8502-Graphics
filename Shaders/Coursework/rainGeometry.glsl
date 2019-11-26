//Author David Towers (160243066)
#version 330 core
layout (triangles) in;
layout (points, max_vertices = 1) out;

uniform float time;
uniform float defy;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

in Vertex {
    //vec4 colour;
    vec2 texCoord;
} IN[];

out Vertex {
	vec2 texCoord;
    float render;
} OUT;

//function found online on stackoverflow made by appas: https://stackoverflow.com/questions/4200224/random-noise-functions-for-glsl
float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

void main() {
    float tim = time;
    vec4 pos = gl_in[0].gl_Position;
    vec4 worldPos = modelMatrix * pos;
    tim -= 12000;
    if (tim >= 0 ) {
        float starty = rand(vec2(worldPos.z, worldPos.x)) * 1000;
        float speed = rand(vec2(worldPos.x, worldPos.z)) + 1;
        worldPos.x += rand(vec2(worldPos.y, worldPos.z)) * 100;
        worldPos.z += rand(vec2(worldPos.y, worldPos.x)) * 100;
        worldPos.y += starty;
        worldPos.y -= (tim/speed) * 4;

        while (worldPos.y < 0) {
            worldPos.y += defy + starty;
        }
        OUT.render = 1;
    }
    else {
        OUT.render = 0;
    }

    gl_Position = projMatrix * viewMatrix * worldPos;
    OUT.texCoord = IN[0].texCoord;

    
    EmitVertex();
    EndPrimitive();
}