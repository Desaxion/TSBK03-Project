#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec3 gPos;
out vec3 gNormal;
out vec3 gModifiedNormal;
out vec3 viewPos;
out vec2 gTexCoord;
out vec4 gfragPosition;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;

uniform vec3 viewDirection;
uniform vec3 viewPosition;

const float lineWidth = 0.3; 
const float backDistance = 0.1;

void main()
{
    gPos = aPos;

    //project normal onto viewplane
    vec3 viewVector = viewDirection * 1000;
    //This gives the projected direction onto the viewing plane
    vec3 offsetDirection = normalize(aNormal - (dot(aNormal, viewVector)/dot(viewVector,viewVector))*viewVector);

    //also offset the model a little in the z-direction of the viewcoodirnates
    //vec3 zVector = normalize(aPos - viewPos);

    gl_Position = vec4(aPos,1.0);//projection*view*model*vec4(aPos,1.0);// + offsetDirection*lineWidth +zVector*backDistance, 1.0);
    gfragPosition = model * vec4(aPos, 1.0); // World-space position
    gNormal = aNormal;
    gModifiedNormal = vec4(projection*view*model*vec4(gNormal,1.0)).xyz;
    viewPos = vec3(view[3]); //Get the final row from the viewmatrix, ie the position
    gTexCoord = aTexCoord;
}