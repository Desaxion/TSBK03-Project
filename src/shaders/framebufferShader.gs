#version 330 core
layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

const int maxVerts = 3;

in vec3 gNormal[maxVerts];
in vec2 gTexCoord[maxVerts];

out vec3 normal;
out vec2 TexCoord;
out vec3 fragPos;

void main()
{
for(int i = 0; i < gl_in.length(); i++)
{
	//fragPos = gl_in[i].gl_Position.xyz;
	gl_Position = gl_in[i].gl_Position;
	normal = gNormal[i];
	TexCoord = gTexCoord[i];
	EmitVertex();
	}
EndPrimitive();
}