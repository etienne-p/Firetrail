#version 330 core

in vec4 ciPosition;

uniform mat4 ciModelViewProjection;

smooth out vec2 vTexCoord;
smooth out vec3 vPosition;
out float vNormalOffset;

void main()
{
    // TODO: move vertices using perlin noise
    vNormalOffset = normalOffset;
    vec4 p = ciModelViewProjection * ciPosition;
    vPosition = p.rgb;
    gl_Position = p;
    vTexCoord = ciTexCoord0;
}