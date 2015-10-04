#version 330 core

in vec4 ciPosition;
in vec2 ciTexCoord0;
in vec3	ciNormal;

uniform mat4 ciModelViewProjection;
uniform float normalOffset;

smooth out vec2 vTexCoord;
smooth out vec3 vPosition;

void main()
{
    // TODO: move vertices using perlin noise
    vec4 p = ciModelViewProjection * (ciPosition + normalOffset * vec4(normalize(ciNormal), ciPosition.w));
    vPosition = p.rgb;
    gl_Position = p;
    vTexCoord = ciTexCoord0;
}