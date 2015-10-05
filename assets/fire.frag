#version 330 core

uniform sampler2D fireTex;

smooth in vec2 texCoord;
smooth in vec3 gPosition;

out vec4 fColor;

void main()
{
    fColor = texture(fireTex, texCoord);
}