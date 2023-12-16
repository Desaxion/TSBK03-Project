#version 330 core

in vec4 fragPosition; // World-space position from the geometry shader
in vec3 normal; // Normal vector
in vec2 TexCoord; // Texture coordinates

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

const int shadingLevels = 4;
vec3 colorLevels[shadingLevels + 1];


uniform sampler2D ourTexture; // Texture

void main()
{
    // Calculate the normalized light direction and the view direction
    vec3 lightDir = normalize(lightPos - vec3(fragPosition));
    vec3 viewDir = normalize(viewPos - vec3(fragPosition));

    // Calculate the ambient component
    vec3 ambient = ambientStrength * lightColor;

    // Calculate the diffuse component
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // Calculate the specular component
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular = specularStrength * spec * lightColor;


    // Apply the texture
    vec4 texColor = texture(ourTexture, TexCoord);
     vec3 result = vec3(1.0f);

    if(phong){
        // Calculate the final color
     result = (ambient + diffuse + specular); //* objectColor;
    }

      vec3 final = result;

    // Combine the texture color with the Phong shading result
   // vec3 finalColor = mix(result, result * texColor.rgb, 0.5);

   vec3 thresholdedColor;


   //CEL SHADER
   //Threshold filter the colors
   float colorThreshold = 1.0/shadingLevels;
   for(int i = 0; i < shadingLevels; i++){
    colorLevels[i] = vec3(i*1.0/shadingLevels);
   }
   for(int i = 0; i < shadingLevels; i++){
        float lowerThreshold = float(i) / shadingLevels;
        float upperThreshold = float(i + 1) / shadingLevels;
        float thresholdValue = step(lowerThreshold, result.r) * step(result.r, upperThreshold);
        thresholdedColor += thresholdValue*colorLevels[i];
   }
   if(celshader){
     final = thresholdedColor;
   }

  FragColor = vec4(final,0.0)*texColor;

}
