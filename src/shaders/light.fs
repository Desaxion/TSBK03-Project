#version 330 core
out vec4 FragColor;
  
in vec3 normal;
in vec2 TexCoord;

uniform int levels;

void main()
{
//FragColor = vec4(normal,1.0);
   vec4 color =  vec4(1.0);
   FragColor = color;
}