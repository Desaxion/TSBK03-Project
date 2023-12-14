#include "Def.h"
#include "Shader.h"
#include "Mesh.h"
#include "Model.h"
#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//Space texture image <a href="https://www.freepik.com/free-photo/milky-way-galaxy-night_13249998.htm#query=space&position=5&from_view=search&track=sph">Image by tawatchai07</a> on Freepik


const unsigned int WIDTH = DEFAULT_WIDTH;	//1000
const unsigned int HEIGHT = DEFAULT_HEIGHT; //1000

#define SHADER_LEVELS 5
#define ambient 0.3
#define specular 0.8
#define diffuse 0.8
#define PI 3.14159265

const int INVERTED_MESH = 1;
const int DEPTH_SOBEL = 2;
const int ADJACENCY_OUTLINE = 3;
const int QUIT = 9;

const int NONE  = 0;


int main() {
	do{
	int input;
	std::cout << "Please select line rendering model:\n" << "1. For inverted mesh method\n" << "2. Depth map and sobel filter\n" << "3. Adjacency outline in GS\n" << "\n9. Quit \nEnter your input: " << std::endl;
	std::cin >> input;

	if (input == QUIT) {
		return 0;
	}

	//Begin with instantiating the window
	glfwInit(); //Tell compiler to initialize GLFW for usage
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3); //These two lines are to make sure that the user uses OpenGL 3.3 or more, otherwise window will not open
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //Tell GLFW that we use the core profile of OpenGL, and not immediate, thus granting us more access to inner functions

	#ifdef MACOS //If the user uses MACOS, set MACOS above to true. This line below has to be run in that case.
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); 
	#endif

	//Creating our window
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "CelShading", NULL, NULL);
	if (!window) { 
		std::cout << "Failed to create window. " << std::endl;
		glfwTerminate(); //Destroy everything related to window
		return -1; //Exit program with code -1.
	}
	glfwMakeContextCurrent(window); //Make sure that OpenGL draws to this very window.

	//Initialize glad before calling OpenGL functions, since glad help managing OpenGL pointers to the GPU
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) { //We send the OpenGL function pointers adresses to glad, and glfw helps with finding correct functions based on the operating system
		std::cout << "Failed to initialize glad" << std::endl;
		return -1;
	}


	//We load in all of the different shaders here
	////LOADING IN MODEL
	Model model("../assets/models/bowser/bowser.obj");


	Shader shader("../src/shaders/main.vs", "../src/shaders/main.fs", "../src/shaders/main.gs");
	
	Shader lineStripShader("../src/shaders/lineStrip.vs", "../src/shaders/lineStrip.fs", "../src/shaders/lineStrip.gs");

	Shader depthBufferShader("../src/shaders/depthBuffer.vs", "../src/shaders/depthBuffer.fs", "../src/shaders/light.gs");

	Shader frameBufferShader("../src/shaders/framebufferShader.vs", "../src/shaders/framebufferShader.fs", "../src/shaders/framebufferShader.gs");

	Shader adjacencyOutlineShader("../src/shaders/lineStrip.vs", "../src/shaders/lineStrip.fs", "../src/shaders/adjacencyOutline.gs");

	Shader* selectedShader;
	Shader* outlineShader;

	Model light("../assets/models/light/light.obj");
	Shader lightShader("../src/shaders/light.vs", "../src/shaders/light.fs", "../src/shaders/light.gs");
	//Tell OpenGL first the position coordinates for lower left point of the viewport relative to the window, and then the size of the viewport
	glViewport(0, 0, WIDTH, HEIGHT); //Position coordinates for lower left corner go from 0 to 1

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); //Make sure the viewport is resized when user resizes window
	
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // Make sure mouse movement is enabled (DEBUG)
	glfwSetCursorPosCallback(window, mouse_callback); //Enable the callback for mouse movement. (When game is done, remove these lines)
	
	//We generate a triangle
	float vertices[] = {
		// Vertices						
		-0.5f,-0.5f, 0.0f,				 
		 0.5f,-0.5f, 0.0f,				 
		 0.0f, 0.5f, 0.0f,	
		 //Normal
		 0.0f, 0.0f, -1.0f,
		 0.0f, 0.0f, -1.0f,
		 0.0f, 0.0f, -1.0f,
		 //Texcoord
		 0.0f, 0.0f,// Bottom left
		 1.0f, 0.0f,// Bottom right
		 0.5f, 1.0f // Top center
	};

	//We create a vertex buffer object, that can handle sending multiple vertices at once to the vertex shader. It helps with managing the memory on the GPU
	unsigned int VBO;
	glGenBuffers(1, &VBO); //Allocate memory on the GPU for the specified VBO

	unsigned int VAO; //Create a vertex array object. It is used to store pointers to all vertices, indices, buffers etc. related to a speceific mesh.
	glGenVertexArrays(1, &VAO); //Create memory for the speciefied Vertex Array objects.

	glBindVertexArray(VAO);

	//We set the buffer to continue listening after us sending vertices. GL_ARRAY_BUFFER is the target and type of our VBO.
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//From now on, all buffer calls we make to the GL_ARRAY_BUFFER target will configure this specific buffer

	//We copy the data from our vertices into the buffer. We use 'sizeof' to specify the size in bytes of the data we are to send in
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	/*
			The last argument specifies how the GPU is going to use the data:
			GL_STREAM_DRAW: the data is set only once and used by the GPU at most a few times.
			GL_STATIC_DRAW: the data is set only once and used many times.
			GL_DYNAMIC_DRAW: the data is changed a lot and used many times.
	*/



	//Now it's time to specify for OpenGL how the vertex data is being sent, we have to do it manually
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	/*EXPLANATION FOR glVertexAttribPointer
	The first parameter specifies which vertex attribute we want to configure. Remember that we specified the location of the position vertex attribute in the vertex shader with layout (location = 0). This sets the location of the vertex attribute to 0 and since we want to pass data to this vertex attribute, we pass in 0.
	
	The next argument specifies the size of the vertex attribute. The vertex attribute is a vec3 so it is composed of 3 values.
	
	The third argument specifies the type of the data which is GL_FLOAT (a vec* in GLSL consists of floating point values).
	
	The next argument specifies if we want the data to be normalized. If we're inputting integer data types (int, byte) and we've set this to GL_TRUE, the integer data is normalized to 0 (or -1 for signed data) and 1 when converted to float. This is not relevant for us so we'll leave this at GL_FALSE.
	
	The fifth argument is known as the stride and tells us the space between consecutive vertex attributes. Since the next set of position data is located exactly 3 times the size of a float away we specify that value as the stride. Note that since we know that the array is tightly packed (there is no space between the next vertex attribute value) we could've also specified the stride as 0 to let OpenGL determine the stride (this only works when values are tightly packed). Whenever we have more vertex attributes we have to carefully define the spacing between each vertex attribute but we'll get to see more examples of that later on.
	
	The last parameter is of type void* and thus requires that weird cast. This is the offset of where the position data begins in the buffer. Since the position data is at the start of the data array this value is just 0. We will explore this parameter in more detail later on
	*/

	//Enable an FBO for the sobel and depth
	GLuint depthFBO;
	GLuint depthRBO;
	GLuint depthTexture;

	/*GLuint finalFBO;
	GLuint finalTexture;
	*/
	unsigned int rectVAO, rectVBO;

	if (input == DEPTH_SOBEL) {

		float rectangleVertices[] = {
			// Coords    // texCoords
			1.0f, -1.0f, 1.0f, 0.0f,
		   -1.0f, -1.0f, 0.0f, 0.0f,
		   -1.0f,  1.0f, 0.0f, 1.0f,

		    1.0f,  1.0f, 1.0f, 1.0f,
			1.0f, -1.0f, 1.0f, 0.0f,
		   -1.0f,  1.0f, 0.0f, 1.0f
		};


		glGenVertexArrays(1, &rectVAO);
		glGenBuffers(1, &rectVBO);
		glBindVertexArray(rectVAO);
		glBindBuffer(GL_ARRAY_BUFFER, rectVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(rectangleVertices), &rectangleVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2*sizeof(float)));
		
		//Example from course book:
		glGenFramebuffers(1, &depthFBO);
		glBindFramebuffer(GL_FRAMEBUFFER, depthFBO);
		
		glGenTextures(1, &depthTexture);
		glBindTexture(GL_TEXTURE_2D, depthTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, WIDTH, HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, depthTexture, 0);
/*
		glGenFramebuffers(1, &finalFBO);
		glBindFramebuffer(GL_FRAMEBUFFER, finalFBO);

		glGenTextures(1, &finalTexture);
		glBindTexture(GL_TEXTURE_2D, finalTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WIDTH, HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, finalTexture, 0);
*/


		glGenRenderbuffers(1, &depthRBO);

		glBindRenderbuffer(GL_RENDERBUFFER, depthRBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, WIDTH, HEIGHT);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRBO);


		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			std::cout << "Framebuffer is not complete! - Check 1" << std::endl;
	
		}			
		frameBufferShader.use();
		//Send the texture to the shader
		frameBufferShader.setInt("depthTexture", 0);
	}

	GLenum error = glGetError();
	if (error != GL_NO_ERROR) {
		std::cout << "OpenGL error: " << error << std::endl;
	}

	const float modelScale = 0.1f;

	if (model.textures_loaded.size() > 0) {
		std::cout << "Textures loaded." << std::endl;
	}
	else {
		std::cout << "No textures found." << std::endl;
	}
	const float rad = 1.5f;
	glm::vec3 lightPosition = glm::vec3(cos(0) * rad, sin(0 - PI) + 4.5, sin(0) * rad);
	float step = 0.0f;



	////////////    MAIN     RENDERING      LOOP    //////////////


	const float lightStepSize = 0.01f;
	const float lightStepScale = 0.03f;
	while(!glfwWindowShouldClose(window)) { //Check if window is instructed to close
		//We redraw screen every frame, thus we clear the screen in beginning of every loop iteration
		//Enabling depth test
		glEnable(GL_DEPTH_TEST);
		//glDisable(GL_DEPTH_TEST);
		//calculating time variables
		float currentFrame = static_cast<float>(glfwGetTime());
		float time = (float)glfwGetTime();


		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;


		std::vector<bool> shadingTypes = processInput(window);

		glClearColor(.3f, 0.3f, 0.2f, 1.0f); //Specify the color of the cleariness
		//glm::vec3 tempColor = glm::normalize(camera.Front);
		//glClearColor(tempColor.x, tempColor.y, tempColor.z, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Specify which buffer we clear (the others are GL_STENCIL_BUFFER_BIT and GL_DEPTH_BUFFER_BIT)

		//Clear the FBO buffer bit Also:
		if (input == DEPTH_SOBEL) {
			glBindFramebuffer(GL_FRAMEBUFFER, depthFBO);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		// The Light I am rendering is rendered outside of the FBO, (it should not be displayed in the FBO).

		///////
		//Position the light
		if (shadingTypes[2]) {
			lightPosition = lightPosition + glm::vec3(lightStepScale)*glm::vec3(cos(step)*rad, sin(step - PI), sin(step)*rad);
			step += lightStepSize;
		}
		glm::mat4 modelMatrixLight = glm::mat4(1.0f);
		view = glm::translate(view, glm::vec3(0, 0, -10.0f));
		modelMatrixLight = glm::translate(modelMatrixLight, lightPosition);
		modelMatrixLight = glm::scale(modelMatrixLight, glm::vec3(0.5));
		lightShader.use();

		lightShader.setMat4("projection", projection);
		lightShader.setMat4("view", view);
		lightShader.setMat4("model", modelMatrixLight);
		lightShader.setInt("levels", SHADER_LEVELS);
		lightShader.setFloat("ambientStrength", ambient);
		lightShader.setFloat("specularStrength", specular);
		lightShader.setFloat("diffuseStrength", diffuse);
		light.Draw(lightShader);
		///////////
		
		// Rendering objects that is to be rendered in the FBO.
		int renderingPasses = 1;
		int order = 0;
		if (input == INVERTED_MESH) { //This is where we will check which method we want to use and select the correct one
			renderingPasses = 2;
			outlineShader = &lineStripShader;
			order = 0;
		}
		if (input == DEPTH_SOBEL) {
			renderingPasses = 2;
			outlineShader = &depthBufferShader;
			//outlineShader = &frameBufferShader;
			//glClear(GL_DEPTH_BUFFER_BIT); // Clear the depth buffer

			order = 0;
		}
		if (input == ADJACENCY_OUTLINE) {
			renderingPasses = 2;
			outlineShader = &adjacencyOutlineShader;
			order = 1;
		}

			//Settng up model and view matrices
			glm::mat4 modelMatrix = glm::mat4(1.0f);
			modelMatrix = glm::scale(modelMatrix, glm::vec3(modelScale));
			view = glm::translate(view, glm::vec3(0, -1, 0.0f));
			bool bound = false;

			// Render two passes (when the input is selected to Depth_Sobel, we render first to the fbo, and the second time to the screen)
			for (int i = 0; i < renderingPasses; i++) {
					selectedShader = &shader;
				//We check how many rendering passes, so that we can render the outline first and the other one on top
				if (renderingPasses > 0 && i == order) {
					selectedShader = outlineShader;	
					if (input == DEPTH_SOBEL) {
						//selectedShader = &shader;
						glBindFramebuffer(GL_FRAMEBUFFER, depthFBO);
						bound = true;
					}	
				}

				selectedShader->use(); //Tell the render which shader to use for upcoming objects
				// view/projection transformations
				float viewamount = 0.25f;
				selectedShader->setVec2("screenSize", glm::vec2(WIDTH, HEIGHT));
				//view = glm::rotate(view, viewamount, glm::vec3(1.0f, 0.0f, 0.0f));
				//view = glm::rotate(view, viewamount, glm::vec3(0.0f, 1.0f, 0.0f));
				selectedShader->setBool("phong", shadingTypes[0]);
				selectedShader->setBool("celshader", shadingTypes[1]);
				selectedShader->setVec3("lightPos", lightPosition);
				selectedShader->setVec3("viewPosition", camera.Position);
				selectedShader->setVec3("viewDirection", camera.Front);
				selectedShader->setMat4("model", modelMatrix);
				selectedShader->setMat4("projection", projection);
				selectedShader->setMat4("view", view);
				selectedShader->setInt("levels", SHADER_LEVELS);
				selectedShader->setBool("refine", shadingTypes[3]);
				selectedShader->setBool("enabled", shadingTypes[4]);
				// render the loaded model
				model.Draw(*selectedShader);

				if (bound) {
					bound = false;
					glBindFramebuffer(GL_FRAMEBUFFER, 0); //Bind of the FBO after it has been rendered to.
				}
			}
				
			// When objects have been rendered correctly, the final object displaying the FBO texture is rendered.
				if (input == DEPTH_SOBEL) {
					
				glEnable(GL_BLEND); // Enable blending
				glViewport(0, 0, WIDTH, HEIGHT);
				frameBufferShader.use();
				glBindTexture(GL_TEXTURE_2D, depthTexture);
				glBindVertexArray(rectVAO);
				glDrawArrays(GL_TRIANGLES, 0, 6);
				glDisable(GL_BLEND);
			}

		
		glfwSwapBuffers(window); //Swaps buffers so it can begin writing to one buffer and reading from the other
		glfwPollEvents(); //Check if glfw events are triggered (such as mouse and keyboard input)
	}

	//Destroy all things related to glfw and close the program
	glfwTerminate();
	std::cout << std::endl;

	} while (true);
	return 0;
}

