#version 330 core
layout(triangles_adjacency) in;
layout(triangle_strip, max_vertices = 6) out;
//layout(line_strip, max_vertices = 18) out;

const int maxVerts = 6;

in vec3 gNormal[maxVerts];
in vec2 gTexCoord[maxVerts];
in vec4 gfragPosition[maxVerts];
in vec3 gModifiedNormal[maxVerts];
in vec3 viewPos[maxVerts];
in vec3 gPos[maxVerts];



const float EPSILON = 0.000001;
float limit = EPSILON;

out vec3 normal;
out vec2 TexCoord;
out vec4 fragPosition;
out float dotColor;

uniform bool refine;
uniform bool enabled;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

uniform vec3 viewDirection;
uniform vec3 viewPosition;

const float lineWidth = 0.1; 
const float tinyLineWidth = 0.01;
const float backDistance = 0.1;

bool displace_index[maxVerts];

const float shrinkSize = 0.8;
vec3 center = vec3(0.0);

 




 //Fix this ploz

void renderVertex(int i, vec3 surfaceNormal) {
//i is the vertex index
			vec4 outPosition;
			if(dot(surfaceNormal,viewDirection) > limit) {  //Might be bad practice with if-statements, since both the if and the else are both calculated. However in this case, it is not a big problem.
				normal = gNormal[i];
				TexCoord = gTexCoord[i];
				//If that is the case, then we can displace the middle 3 points
				//project normal onto viewplane
				vec3 viewVector = viewDirection * 1000;
				//This gives the projected direction onto the viewing plane
				vec3 offsetDirection = normalize(gNormal[i] - (dot(gNormal[i], viewVector)/dot(viewVector,viewVector))*viewVector);

				//also offset the model a little in the z-direction of the viewcoodirnates
				vec3 zVector = normalize(gPos[i] - viewPos[i]);

				//Find if the dot product of x and the other surfaces normals is below 0. In that case -> dont move the vertices.
				
		
				// Regular dilation
				outPosition =  gl_in[i].gl_Position + vec4(offsetDirection*lineWidth +zVector*backDistance, 0.0);
				

				if(enabled){
				gl_Position = projection*view*model*(outPosition);
					EmitVertex();
				}
			}
}



void main() {
			/*

		Adjacency clarification on what each index corresponds to:

						3
					   / \
					  / b \
					 2-----4
					/ \ x / \
				   / a \ / c \
				  1-----0-----5

		So it contains a total of 4 triangles

		*/
		vec3 V0 = gl_PositionIn[0].xyz;
vec3 V1 = gl_PositionIn[1].xyz;
vec3 V2 = gl_PositionIn[2].xyz;
vec3 V3 = gl_PositionIn[3].xyz;
vec3 V4 = gl_PositionIn[4].xyz;
vec3 V5 = gl_PositionIn[5].xyz;
vec3 N042 = cross( V4-V0, V2-V0 ); // the center triangle’s normal
vec3 N021 = cross( V2-V0, V1-V0 );
vec3 N243 = cross( V4-V2, V3-V2 );
vec3 N405 = cross( V0-V4, V5-V4 );
if( dot( N042, N021 ) < 0. ) // make sure each outer triangle’s
N021 = vec3(0.,0.,0.) - N021; // normal is in the same general direction
if( dot( N042, N243 ) < 0. )
N243 = vec3(0.,0.,0.) - N243;
if( dot( N042, N405 ) < 0. )
N405 = vec3(0.,0.,0.) - N405;
if( N042.z * N021.z <= 0. )
{
gl_Position = gl_ProjectionMatrix * vec4( V0, 1. );
EmitVertex( );
gl_Position = gl_ProjectionMatrix * vec4( V2, 1. );
EmitVertex( );
EndPrimitive( );
}
if( N042.z * N243.z <= 0. )
{
gl_Position = gl_ProjectionMatrix * vec4( V2, 1. );
EmitVertex( );
gl_Position = gl_ProjectionMatrix * vec4( V4, 1. );
EmitVertex( );
EndPrimitive( );
}
if( N042.z * N405.z <= 0. )
{
gl_Position = gl_ProjectionMatrix * vec4( V4, 1. );
EmitVertex( );
gl_Position = gl_ProjectionMatrix * vec4( V0, 1. );
EmitVertex( );
EndPrimitive( );
}
/*
		renderVertex(0,x_surfaceNormal);
		renderVertex(2,x_surfaceNormal);
		renderVertex(4,x_surfaceNormal);
		EndPrimitive();
	*/	
}

