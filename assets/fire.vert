#version 330 core

in vec4 ciPosition;
in float vSize;

out float size;

uniform mat4 ciModelView;

void main()
{
    size = .4 + .6 * vSize;
    gl_Position = ciModelView * ciPosition;
}