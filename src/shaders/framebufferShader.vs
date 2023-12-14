#version 330 core
layout (location = 0) in vec3 aPos;
//layout (location = 1) in vec3 aNormal;
layout (location = 1) in vec2 aTexCoord;

out vec3 gNormal;
out vec2 gTexCoord;

// Funciton to contrast stretch the model to make it easier for the sobel filter


void main()
{
    
    gl_Position = vec4(aPos, 1.0);
    gTexCoord = aTexCoord;
}