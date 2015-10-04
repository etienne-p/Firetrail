#version 330 core

in vec4 ciPosition;
in vec2 ciTexCoord0;
in vec3	ciNormal;

uniform mat4 ciModelViewProjection;
uniform float normalOffset;
uniform float spread;
uniform vec3 eyePosition;

smooth out vec2 vTexCoord;
smooth out vec3 vPosition;
out float vNormalOffset;

void main()
{
    // TODO: move vertices using perlin noise
    vNormalOffset = normalOffset;
    vec3 eyeDir = normalize(ciPosition.xyz - eyePosition);
    vec4 p = ciModelViewProjection * (ciPosition + vec4(eyeDir, ciPosition.w) * normalOffset * spread);
    vPosition = p.rgb;
    gl_Position = p;
    vTexCoord = ciTexCoord0;
}