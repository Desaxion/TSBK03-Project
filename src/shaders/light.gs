#version 330 core
layout(triangles_adjacency) in;
layout(triangle_strip, max_vertices = 6) out;

const int maxVerts = 6;

in vec3 gNormal[maxVerts];
in vec2 gTexCoord[maxVerts];

out vec3 normal;
out vec2 TexCoord;

void main()
{




for(int i = 0; i < gl_in.length(); i+=2)
{
gl_Position = gl_in[i].gl_Position;
normal = gNormal[i];
TexCoord = gTexCoord[i];
EmitVertex();
}
EndPrimitive();
}