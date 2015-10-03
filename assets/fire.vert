#version 330 core

in vec4 ciPosition;
in vec2 ciTexCoord0;

uniform mat4 ciModelViewProjection;

smooth out vec2 vTexCoord;
smooth out vec3 vPosition;

void main()
{
    // TODO: move vertices using perlin noise
    vec4 p = ciModelViewProjection * ciPosition;
    vPosition = p.rgb;
    gl_Position = p;
    vTexCoord = ciTexCoord0;
}