#version 330 core

in vec4 fragPosition; // World-space position from the geometry shader
in vec3 normal; // Normal vector
in vec2 TexCoord; // Texture coordinates
in float dotColor;


out vec4 FragColor;

uniform vec3 lightPos; // Position of the light source
uniform vec3 viewPos; // Position of the camera (viewer)
vec3 lightColor = vec3(1,1,1); // Color of the light
vec3 objectColor = vec3(1,0,0); // Color of the object
uniform float ambientStrength;
uniform float specularStrength;
uniform float diffuseStrength;
uniform bool phong;
uniform bool celshader;
int shininess = 5; // Shininess factor

uniform int levels;

const int shadingLevels = 3;
vec3 colorLevels[shadingLevels + 1];


uniform sampler2D ourTexture; // Texture

void main()
{
   //FragColor =  texture(ourTexture, TexCoord);
  // FragColor = (vec4(vec3(dotColor),1.0));
  FragColor = vec4(0,0,0,1.0);

}
