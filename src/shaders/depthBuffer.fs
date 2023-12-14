#version 330 core
uniform sampler2D depthTexture;
uniform vec2 screenSize;
uniform vec3 viewPos;

out vec4 fragColor;

const float near = 0.1; 
const float far  = 100.0; 
  
float linearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near))/10;	
}

void main() {
   //float depthValue = texture(depthTexture, gl_FragCoord.xy / screenSize).r;
   //fragColor = vec4(vec3(depthValue), 1.0);
   vec3 relativeFragPos = gl_FragCoord.xyz - viewPos;


   fragColor = vec4(vec3(linearizeDepth(relativeFragPos.z)),1.0);
   //fragColor = vec4(0.0,1,0,1.0);
    
}
