#version 150 core

uniform sampler2D diffuseTex;
uniform sampler2D bumpTex;
uniform sampler2DShadow shadowTex;

uniform vec4 diffuseColour;
uniform vec4 specColour;
uniform vec3 lightPos;
uniform float lightRadius;

uniform vec4 diffuseColour2;
uniform vec4 specColour2;
uniform vec3 lightPos2;
uniform float lightRadius2;

uniform vec3 cameraPos;

uniform int secondLight;

in Vertex {
    vec4 colour;
    vec2 texCoord;
    vec3 normal;
    vec3 tangent;
    vec3 binormal;
    vec3 worldPos;
    vec4 shadowProj;
} IN;

out vec4 fragColour;

void main (void) {
    mat3 TBN = mat3(IN.tangent, IN.binormal, IN.normal);
    vec3 normal = normalize(TBN * (texture2D(bumpTex, IN.texCoord).rgb * 2.0 - 1.0));

    //vec4 diffuse = texture2D(diffuseTex, IN.texCoord);
    vec4 diffuse = (texture(diffuseTex, IN.texCoord) ) * vec4(0,1,0,1);
    if (diffuse.x < 0.01 && diffuse.y < 0.01 && diffuse.z < 0.01 ) {
		diffuse = vec4(0.396,0.263,0.129,1);
        //Trunk normals are facing the wrong way
        normal *= -1;
    }

    vec3 incident = normalize(lightPos - IN.worldPos);
    float lambert = max(0.0, dot(incident, normal));

    float dist = length(lightPos - IN.worldPos);
    float atten = 1.0 - clamp(dist/lightRadius, 0.0, 1.0);

    vec3 viewDir = normalize(cameraPos - IN.worldPos);
    vec3 halfDir = normalize(incident + viewDir);

    float rFactor = max(0.0, dot(halfDir, normal));
    float sFactor = pow(rFactor, 33.0);

    float shadow = 1.0;

    if (IN.shadowProj.w > 0.0) {
        shadow = textureProj(shadowTex, IN.shadowProj);
    }

    lambert *= shadow;

    vec3 colour = (diffuse.rgb * diffuseColour.rgb);
    colour += (specColour.rgb * sFactor) * 0.33;
    vec4 light1 = vec4(colour * atten * lambert, diffuse.a);
    light1.rgb += (diffuse.rgb * diffuseColour.rgb) * 0.3;
    

    vec3 incident2 = normalize(lightPos2 - IN.worldPos);
    float lambert2 = max(0.0, dot(incident2, normal));

    float dist2 = length(lightPos2 - IN.worldPos);
    float atten2 = 1.0 - clamp(dist2/lightRadius2, 0.0, 1.0);

    vec3 viewDir2 = normalize(cameraPos - IN.worldPos);
    vec3 halfDir2 = normalize(incident2 + viewDir2);

    float rFactor2 = max(0.0, dot(halfDir2, normal));
    float sFactor2 = pow(rFactor2, 33.0);

    vec3 colour2 = diffuseColour2.rgb;
    colour2 += (specColour2.rgb * sFactor2) * 0.33;
    vec4 light2 = vec4(colour2 * atten2 * lambert2, diffuse.a);
    light2.rgb += (diffuse.rgb * diffuseColour2.rgb) * 0.3;
    if (secondLight == 1)
        fragColour = light1*0.5 + light2*0.5;
    else
    fragColour = light1;
}