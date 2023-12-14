#pragma once
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "Camera.h"
#include <vector>

#define MACOS false

const unsigned int DEFAULT_WIDTH  = 1000;
const unsigned int DEFAULT_HEIGHT = 1000;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
std::vector<bool> processInput(GLFWwindow* window);

bool firstMouse = true;
float yaw = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch = 0.0f;
float fov = 45.0f;

bool phong = true;
bool celshading = true;
bool lightMove = true;
bool shaderRefine = false;
bool enabled = true;

bool phongSamePress = false;
bool celshadingSamePress = false;
bool lightSamePress = false;
bool shaderRefineSamePress = false;
bool enabledSamePress = false;
// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = DEFAULT_WIDTH / 2.0f;
float lastY = DEFAULT_HEIGHT / 2.0f;


// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

//Will resize the OpenGL viewport if the window is resized
void framebuffer_size_callback(GLFWwindow* window, int width, int height) { 
	glViewport(0, 0, width, height); //Set the viewport to the specified arguments
}

//function for processing input to the window
std::vector<bool> processInput(GLFWwindow* window) {
	//Press escape -> Exit application

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
	//Keyboard
	//Movement
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		camera.ProcessKeyboard(LEFT, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		camera.ProcessKeyboard(RIGHT, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		camera.ProcessKeyboard(FORWARD, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		camera.ProcessKeyboard(UP, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
		camera.ProcessKeyboard(DOWN, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
		if (!phongSamePress) {
			phong = !phong;
			phongSamePress = true;
		}
	}
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
		if (!celshadingSamePress) {
			celshading = !celshading;
			celshadingSamePress = true;
		}
		
	}
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
		if (!shaderRefineSamePress) {
			shaderRefine = !shaderRefine;
			shaderRefineSamePress = true;
		}

	}
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
		if (!enabledSamePress) {
			enabled = !enabled;
			enabledSamePress = true;
		}

	}

	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_RELEASE) {
		phongSamePress = false;
	}

	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_RELEASE) {
		celshadingSamePress = false;
	}


	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_RELEASE) {
		shaderRefineSamePress = false;
	}

	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_RELEASE) {
		enabledSamePress = false;
	}


	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
		if (!lightSamePress) {
			lightMove = !lightMove;
			lightSamePress = true;
		}

	}



	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_RELEASE) {
		lightSamePress = false;
	}

	return std::vector<bool>{ phong, celshading, lightMove, shaderRefine, enabled };
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	float sensitivity = 0.1f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	glm::vec3 direction;
	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	camera.Front = glm::normalize(direction);
	
	// also re-calculate the Right and Up vector
	camera.Right = glm::normalize(glm::cross(camera.Front, glm::vec3(0,1,0)));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
	camera.Up = glm::normalize(glm::cross(camera.Right, camera.Front));
	
	
}