#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec3 gNormal;
out vec3 gModifiedNormal;
out vec3 viewPos;
out vec2 gTexCoord;
out vec4 gfragPosition;

const float distance = 0.01;

uniform vec3 viewDirection;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;


void main()
{
    gl_Position = projection*view*model*vec4(aPos, 1.0);
    gfragPosition = model * vec4(aPos, 1.0); // World-space position
    gNormal = aNormal;
    gModifiedNormal = mat3(transpose(inverse(model)))*gNormal;
    viewPos = vec3(view[3]); //Get the final row from the viewmatrix, ie the position
    gTexCoord = aTexCoord;
}