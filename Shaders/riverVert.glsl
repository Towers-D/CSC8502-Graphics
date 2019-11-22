#version 150 core

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform mat4 textureMatrix;

uniform sampler2D red;
uniform sampler2D green;
uniform sampler2D blue;
uniform float time;
uniform float defy;

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

void main ( void ) {

    vec4 worldPos = vec4(position, 1);
    OUT.colour = colour;
    OUT.texCoord = texCoord;//(textureMatrix * vec4(texCoord, 0.0, 1.0)).xy;

    float tr = texture2D(red, OUT.texCoord / 16.0f).r;
    float tg = texture2D(green, OUT.texCoord / 16.0f).g;
    float tb = texture2D(blue, OUT.texCoord / 16.0f).b;
    
    float startHeight = worldPos.y;
    float minH = (startHeight + 200);

    int timediv = 4;
    if (worldPos.x < time) {
            worldPos.y -= (((time - worldPos.x) / timediv  > minH) ? minH :  (time - worldPos.x)/timediv) * tg;

    }

    float aft = (time - 8000);
     if (worldPos.x < aft && aft >= 0) {
            worldPos.y -= (((aft - worldPos.x) / timediv  > minH) ? minH :  (aft - worldPos.x)/timediv) * tr;
            worldPos.y += (((aft - worldPos.x) / timediv  > minH) ? minH :  (aft - worldPos.x)/timediv) * tb;
     }

    

    mat3 normalMatrix = transpose(inverse(mat3(modelMatrix)));

    OUT.normal = normalize(normalMatrix * normalize(normal));
    OUT.tangent = normalize(normalMatrix * normalize(tangent));
    OUT.binormal = normalize(normalMatrix * normalize(cross(normal, tangent)));

    OUT.worldPos = (modelMatrix * worldPos).xyz;
    OUT.shadowProj = (textureMatrix * vec4(position + (normal * 1.5), 1));
    gl_Position = (projMatrix * viewMatrix) * vec4(OUT.worldPos, 1.0);
}