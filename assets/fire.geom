#version 330 core

// outputs camera aligned quads from input points

uniform mat4 ciProjectionMatrix;

layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

in float[] size;

out float fSize;

smooth out vec2 texCoord;
smooth out vec3 vPosition;

void main()
{
    const vec4 dw = vec4(.5, .0, .0, .0);
    const vec4 dh = vec4(.0, 1.0, .0, .0);

    texCoord = vec2(.0, .0);
    vec4 worldPos0 = gl_in[0].gl_Position + (-dw - dh) * .5 * size[0];
    vPosition = worldPos0.xyz;
    fSize = size[0];
    gl_Position = ciProjectionMatrix * worldPos0;
    EmitVertex();
    
    texCoord = vec2(1.0, .0);
    vec4 worldPos1 = gl_in[0].gl_Position + (dw - dh) * .5 * size[0];
    vPosition = worldPos1.xyz;
    fSize = size[0];
    gl_Position = ciProjectionMatrix * worldPos1;
    EmitVertex();
    
    texCoord = vec2(.0, 1.0);
    vec4 worldPos3 = gl_in[0].gl_Position + (-dw + dh) * .5 * size[0];
    vPosition = worldPos3.xyz;
    fSize = size[0];
    gl_Position = ciProjectionMatrix * worldPos3;
    EmitVertex();
    
    texCoord = vec2(1.0, 1.0);
    vec4 worldPos2 = gl_in[0].gl_Position + (dw + dh) * .5 * size[0];
    vPosition = worldPos2.xyz;
    fSize = size[0];
    gl_Position = ciProjectionMatrix * worldPos2;
    EmitVertex();
}