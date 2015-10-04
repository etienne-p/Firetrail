// outputs camera aligned quads from input points

uniform vec3 viewDir;

layout (points) in;
layout (quads, max_vertices = 4) out;

smooth out vec2 texCoord;

void main()
{
    // 4 corners

    gl_Position = gl_in[0].gl_Position + ?;
    EmitVertex();
    
    gl_Position = gl_in[0].gl_Position + ?;
    EmitVertex();
    
    gl_Position = gl_in[0].gl_Position + ?;
    EmitVertex();
    
    gl_Position = gl_in[0].gl_Position + ?;
    EmitVertex();

}