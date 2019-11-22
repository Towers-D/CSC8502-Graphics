#version 150 core

uniform sampler2D diffuseTex;
uniform sampler2D bumpTex;
uniform sampler2DShadow shadowTex;
uniform sampler2D grass;
uniform sampler2D mud;
uniform sampler2D grassNorm;
uniform sampler2D mudNorm;

uniform float time;

uniform vec4 diffuseColour;
uniform vec4 specColour;
uniform vec3 lightPos;
uniform vec3 cameraPos;
uniform float lightRadius;

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

void main(void) {
    mat3 TBN = mat3(IN.tangent, IN.binormal, IN.normal);
    
    float tim = time - 16000;
    float change = 1;
    if (tim > 0 && IN.worldPos.y > 40)
        change -= tim/1000;
    if (change < 0 )
        change = 0;
    vec4 gra = texture2D(grass, IN.texCoord);
    vec4 mu  = texture2D(mud, IN.texCoord);
    vec4 rock = texture2D(diffuseTex, IN.texCoord);

    vec3 graN = texture2D(grassNorm, IN.texCoord).rgb;
    vec3 muN = texture2D(mudNorm, IN.texCoord).rgb;
    vec3 rockN = texture2D(bumpTex, IN.texCoord).rgb;

    vec4 diffuse = (rock * change) + (mu * (1 - change));
    vec3 normal = normalize(TBN * (((rockN * change) + (muN * (1 - change))) * 2.0 - 1.0));

    tim = time - 18000;
    if (change == 0 && IN.worldPos.y > 60 && tim > 0) {
        float change2 = 1;
        change2 -= tim/1000;
        if (change2 < 0 )
            change2 = 0;
        diffuse = (mu * change2) + (gra * (1 - change2));
        //vec3 normal = normalize(TBN * (texture2D(bumpTex, IN.texCoord).rgb * 2.0 - 1.0));
        normal = normalize(TBN * (  (  (muN * change2) + (graN * (1 - change2)  ) ) * 2.0 - 1.0  )   );
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
    fragColour = vec4(colour * atten * lambert, diffuse.a);
    fragColour.rgb += (diffuse.rgb * diffuseColour.rgb) * 0.1;
    //fragColour = vec4(normal, 1);
}