//Author David Towers (160243066)
#version 150 core
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform mat4 textureMatrix;

uniform float time;

in vec3 position;
in vec4 colour;
in vec3 normal;
in vec3 tangent;
in vec2 texCoord;

out Vertex {
    vec4 colour;
    vec2 texCoord;
    vec3 normal;
    vec3 tangent;
    vec3 binormal;
    vec3 worldPos;
    vec4 shadowProj;
} OUT;

void main(void) {
    OUT.colour = colour;
    OUT.texCoord = (textureMatrix * vec4(texCoord, 0.0, 1.0)).xy;

    mat3 normalMatrix = transpose(inverse(mat3(modelMatrix)));

    OUT.normal = normalize(normalMatrix * normalize(normal));
    OUT.tangent = normalize(normalMatrix * normalize(tangent));
    OUT.binormal = normalize(normalMatrix * normalize(cross(normal, tangent)));

    OUT.worldPos = (modelMatrix * vec4(position, 1)).xyz;
    float tim = time - 12500;
    if (tim >= 0 ) {
        
        if (tim > 4000) {
            OUT.worldPos.y += 80;
        }
        else {
            OUT.colour = vec4(1,0,0,1);
            OUT.worldPos.y += tim/50;
        }
    } else { 
            OUT.colour = vec4(1,0,0,1);
    }
    OUT.shadowProj = (textureMatrix * vec4(OUT.worldPos + (normal * 1.5), 1));
    gl_Position = (projMatrix * viewMatrix) * vec4(OUT.worldPos, 1);
}