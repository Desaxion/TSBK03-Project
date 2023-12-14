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
		//Surface normal for face x
		vec3 x_edge_0 = gPos[2] - gPos[0];
		vec3 x_edge_1 = gPos[4] - gPos[0];

		vec3 x_surfaceNormal = vec3(0.0f);

		x_surfaceNormal = normalize(cross(x_edge_0,x_edge_1));

		//Surface normal for face a
		vec3 a_edge_0 = gPos[1] - gPos[0];
		vec3 a_edge_1 = gPos[2] - gPos[0];

		vec3 a_surfaceNormal = vec3(0.0f);

		a_surfaceNormal = normalize(cross(a_edge_0,a_edge_1));

		//Surface normal for face b
		vec3 b_edge_0 = gPos[2] - gPos[4];
		vec3 b_edge_1 = gPos[3] - gPos[4];

		vec3 b_surfaceNormal = vec3(0.0f);

		b_surfaceNormal = normalize(cross(b_edge_0,b_edge_1));

		//Surface normal for face c
		vec3 c_edge_0 = gPos[4] - gPos[5];
		vec3 c_edge_1 = gPos[0] - gPos[5];

		vec3 c_surfaceNormal = vec3(0.0f);

		c_surfaceNormal = normalize(cross(c_edge_0,c_edge_1));

		for(int i = 0; i < maxVerts; i++){
			displace_index[i] = true;
		}

		
		if(dot(x_surfaceNormal,a_surfaceNormal) < EPSILON){ //Displace the point that is not on the connecting edge between the two faces
				displace_index[0] = false;
				displace_index[2] = false;
		}
		if(dot(x_surfaceNormal,b_surfaceNormal) < EPSILON){
				displace_index[2] = false;
				displace_index[4] = false;	
		}
		if(dot(x_surfaceNormal,c_surfaceNormal) < EPSILON){
				displace_index[4] = false;
				displace_index[0] = false;
		}

		for(int i = 0; i < gl_in.length(); i++){
			center += gl_in[i].gl_Position.xyz;
		}
		center /= gl_in.length();



		renderVertex(0,x_surfaceNormal);
		renderVertex(2,x_surfaceNormal);
		renderVertex(4,x_surfaceNormal);
		EndPrimitive();
		
}


/*	for (int i = 0; i < maxVerts; i += 2) {	
	
	if(dot(x_surfaceNormal,viewDirection) > limit) {  //Might be bad practice with if-statements, since both the if and the else are both calculated. However in this case, it is not a big problem.
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
		if(refine) {
			//Balloon dilation (Not yet working)
			if(displace_index[i]) {
				gl_Position = gl_in[i].gl_Position + projection*view*model*vec4(offsetDirection*lineWidth +zVector*backDistance, 0.0);
			} else {
				gl_Position = gl_in[i].gl_Position + projection*view*model*vec4(offsetDirection*tinyLineWidth +zVector*backDistance, 0.0);;
			}
		} else{
			// Regular dilation
			gl_Position = gl_in[i].gl_Position + projection*view*model*vec4(offsetDirection*lineWidth +zVector*backDistance, 0.0);
		}

		if(enabled){
			EmitVertex();
		}
	}
	}*/



						//Balloon dilation (Not yet working)
					/*if(displace_index[i]) {
						gl_Position = gl_in[i].gl_Position + projection*view*model*vec4(offsetDirection*lineWidth +zVector*backDistance, 0.0);
					} else {
						gl_Position = gl_in[i].gl_Position + projection*view*model*vec4(offsetDirection*tinyLineWidth +zVector*backDistance, 0.0);;
					}*/


/*
	
				if(refine) {

				//Lets make a "triangle-shrinker" first.

				outPosition = vec4(center + shrinkSize*(gl_in[i].gl_Position.xyz - center),1.0);
					//Balloon dilation (Not yet working)
					/*if(displace_index[i]) {
						outPosition = gl_in[i].gl_Position + vec4(offsetDirection*lineWidth +zVector*backDistance, 0.0);
					} else {
						outPosition = gl_in[i].gl_Position; //+ //projection*view*model*vec4(offsetDirection*tinyLineWidth +zVector*backDistance, 0.0);;
					}*//*
				
					// Regular dilation
					outPosition =  gl_in[i].gl_Position + vec4(offsetDirection*lineWidth +zVector*backDistance, 0.0);
				}
*/