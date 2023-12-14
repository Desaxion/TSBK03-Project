#version 330 core
layout(triangles_adjacency) in;
layout(triangle_strip, max_vertices = 3) out;

const int maxVerts = 6;

in vec3 gNormal[maxVerts];
in vec2 gTexCoord[maxVerts];
in vec4 gfragPosition[maxVerts];
in vec3 gModifiedNormal[maxVerts];
in vec3 viewPos[maxVerts];


out vec3 normal;
out vec2 TexCoord;
out vec4 fragPosition;

void main()
{
for(int i = 0; i < gl_in.length(); i+=(maxVerts/3))
{
		gl_Position = gl_in[i].gl_Position;
		normal = gNormal[i];
		TexCoord = gTexCoord[i];
		fragPosition = gfragPosition[i];
		EmitVertex();
}
EndPrimitive();
}

