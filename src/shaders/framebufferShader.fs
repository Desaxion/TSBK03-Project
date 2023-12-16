#version 330 core
out vec4 FragColor;
  
in vec2 TexCoord;
in vec3 fragPos;

uniform bool enabled;
uniform sampler2D depthTexture;

const float threshold = 0.5;


void main(){
    // Use the texture function to sample the color from the depth texture
   vec4 inputColor = texture(depthTexture, TexCoord);

       // Sobel operators
    mat3 sobelX = mat3(-1,  0,  1, 
                       -2,  0,  2,
                       -1,  0,  1);
    mat3 sobelY = mat3(-1, -2, -1,
                        0,  0,  0,
                        1,  2,  1);

    // Calculate gradients in x and y directions
    vec3 colorX = texture(depthTexture, TexCoord - 1.0 / textureSize(depthTexture, 0)).rgb * sobelX[0] +
                  texture(depthTexture, TexCoord).rgb * sobelX[1] +
                  texture(depthTexture, TexCoord + 1.0 / textureSize(depthTexture, 0)).rgb * sobelX[2];

    vec3 colorY = texture(depthTexture, TexCoord - 1.0 / textureSize(depthTexture, 0)).rgb * sobelY[0] +
                  texture(depthTexture, TexCoord).rgb * sobelY[1] +
                  texture(depthTexture, TexCoord + 1.0 / textureSize(depthTexture, 0)).rgb * sobelY[2];

    // Combine gradients to get the Sobel gradient
   vec3 sobelGradient = sqrt(colorX * colorX + colorY * colorY)*50;

   vec3 edges = vec3(step(threshold, sobelGradient));
  
       if(edges.y > 0.9){
          if(enabled){
            FragColor = vec4(vec3(0.0,0.0,0.0),1.0);
          }else{
          discard;
          }
       }
       else{
       //FragColor = vec4(0.0);    
            discard;
       }

   //FragColor = inputColor;
   //FragColor = vec4(TexCoord + vec2(0.5),0.0,1.0);
}
