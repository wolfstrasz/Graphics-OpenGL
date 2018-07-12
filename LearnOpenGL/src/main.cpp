#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <string>

#include "vertices.h"
#include "indices.h"
#include "shader.h"
#include "window.h"
#include "camera.h"
#include "point_light.h"
#include "dir_light.h"

#pragma region _FUNCTION_INIT
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
void calculateDelta();
void switchCameras();
unsigned int loadTexture(char const * path);
#pragma endregion _FUNCTION_INIT

#pragma region _DATA_INIT_CUBES_AND_LIGHTS
glm::vec3 cubePositions[] = {
	glm::vec3(0.0f,   0.0f,   0.0f),
	glm::vec3(2.0f,   5.0f, -15.0f),
	glm::vec3(-1.5f,  -2.2f,  -2.5f),
	glm::vec3(-3.8f,  -2.0f, -12.3f),
	glm::vec3(2.4f,  -0.4f,  -3.5f),
	glm::vec3(-1.7f,   3.0f,  -7.5f),
	glm::vec3(1.3f,  -2.0f,  -2.5f),
	glm::vec3(1.5f,   2.0f,  -2.5f),
	glm::vec3(1.5f,   0.2f,  -1.5f),
	glm::vec3(-1.3f,   1.0f,  -1.5f)
};
glm::vec3 pointLightPositions[] = {
	glm::vec3(0.7f,  0.2f,  2.0f),
	glm::vec3(2.3f, -3.3f, -4.0f),
	glm::vec3(-4.0f,  2.0f, -12.0f),
	glm::vec3(0.0f,  0.0f, -3.0f)
};
#pragma endregion _DATA_INIT_CUBES_AND_LIGHTS

#pragma region _DATA_INIT_COMPONENTS
// LIGHTS
#define NR_POINT_LIGHTS 4
#define NR_DIR_LIGHTS 1
PointLight pointLights[4];
DirLight dirLights[1];

// Pointers to currents
Window* curWindow = nullptr;
Camera* curCamera = nullptr;
// Path to textures folder
std::string TEX_PATH = "res/textures/";

// Initialise windows
Window window1 = Window();
// Initialise cameras
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 10.0f);
Camera camera1(1, cameraPos);
Camera camera2(2, cameraPos);

bool firstMouse = true;
float lastX = 800 / 2.0f;
float lastY = 600 / 2.0f;

// Calculate time difference
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// Last frame the "TAB" btn was pressed
float lastFrame_Tab = 0.0f;

// lighting
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

// SPACE MATRICES:
glm::mat4 projection = glm::mat4(1.0f);
glm::mat4 view = glm::mat4(1.0f);
glm::mat4 model = glm::mat4(1.0f);
#pragma endregion _DATA_INIT_COMPONENTS

int main(void)
{
#pragma region _SET_UP
	// Set camera as current
	curCamera = &camera1;
	// Initialise window and set it as current
	if (!window1.init())return -1;
	curWindow = &window1;

	// Set context
	glfwMakeContextCurrent(curWindow->getWindow());

	// SET UP GLAD POINTERS
	if (!gladLoadGL())
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
	// Set Depth Testing for shaders
	glEnable(GL_DEPTH_TEST);
	fprintf(stderr, "OpenGL %s\n", glGetString(GL_VERSION));

	// Set callbacks
	glfwSetFramebufferSizeCallback(curWindow->getWindow(), framebuffer_size_callback);
	glfwSetCursorPosCallback(curWindow->getWindow(), mouse_callback);
	glfwSetScrollCallback(curWindow->getWindow(), scroll_callback);

#pragma endregion _SET_UP

#pragma region _BINDINGS
	// Bind VAOs, VBOs and set Attribute pointers
	//-------------------------------------------

	// first, configure the cube's VAO (and VBO)
	unsigned int VBO, cubeVAO;
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindVertexArray(cubeVAO);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(0 * sizeof(float)));
	glEnableVertexAttribArray(0);
	// normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// texture coordinates
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	// second, configure the light's VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube)
	unsigned int lightVAO;
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);

	// we only need to bind to the VBO (to link it with glVertexAttribPointer), no need to fill it;
	//the VBO's data already contains all we need (it's already bound, but we do it again for educational purposes)
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(0 * sizeof(float)));
	glEnableVertexAttribArray(0);

#pragma endregion _BINDINGS

#pragma region _CREATE_TEXTURES
	// Load textures (we now use a utility function to keep the code more organized)
	// -----------------------------------------------------------------------------
	unsigned int diffuseMap = loadTexture("res/textures/wooden_container.png");
	unsigned int specularMap = loadTexture("res/textures/wooden_container_specular.png");
	unsigned int emissionMap = loadTexture("res/textures/matrixblue.jpg");
#pragma endregion _CREATE_TEXTURES

#pragma region _CREATE_SHADERS
	// Create shader programs, compile them and set textures
	//------------------------------------------------------
	// Compile Shader program
	//Shader lightingShader("basic.lighting.4.5", "basic.lighting.4.5");
	//Shader lightingShader("basic.lighting.4.5", "multiple.lighting");
	Shader lightingShader("multiple.lighting", "multiple.lighting");
	Shader lampShader("basic.lamp.4.5", "basic.lamp.4.5");
	// Pre-set Textures
	lightingShader.use();
	lightingShader.setInt("material.diffuse", 0);
	lightingShader.setInt("material.specular", 1);
	lightingShader.setInt("material.emission", 2);
#pragma endregion

#pragma region _BIND_TEXTURES_AND_MAPS
	// Binding textures and diffuse/specular maps to TEXTURE units
	//------------------------------------------------------------
	// bind diffuse map
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, diffuseMap);

	// bind specular map
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, specularMap);

	// bind emission map
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, emissionMap);
#pragma endregion _BIND_TEXTURES_AND_MAPS

#pragma region _CREATE_LIGHTS
	for (int i = 0; i < NR_POINT_LIGHTS; i++)
	{
		pointLights[i] = PointLight(pointLightPositions[i]);
	}

	for (int i = 0; i < NR_DIR_LIGHTS; i++)
	{
		dirLights[i] = DirLight();
	}
#pragma endregion _CREATE_LIGHTS

#pragma region _MAIN_LOOP
	while(!curWindow->shouldClose())
	{
		// Get frame difference
		calculateDelta();
		// Check for keyboard input and update the window
		processInput(curWindow->getWindow());
		curWindow->update();
		// be sure to activate shader when setting uniforms/drawing objects
		lightingShader.use();
		lightingShader.setVec3("viewPos", curCamera->getPosition());
		lightingShader.setFloat("material.shininess", 32.0f);

#pragma region _LIGHTS
		// directional lights
		for (int i = 0; i < NR_DIR_LIGHTS; i++)
		{
			dirLights[i].setLight(lightingShader, i);
		}
		// point lights
		for (int i = 0; i < NR_POINT_LIGHTS; i++)
		{
			pointLights[i].setLight(lightingShader, i);
		}
		// spot lights
		lightingShader.setVec3("spotLight.position", curCamera->getPosition());
		lightingShader.setVec3("spotLight.direction", curCamera->getFront());
		lightingShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
		lightingShader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
		lightingShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
		lightingShader.setFloat("spotLight.constant", 1.0f);
		lightingShader.setFloat("spotLight.linear", 0.09f);
		lightingShader.setFloat("spotLight.quadratic", 0.032f);
		lightingShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
		lightingShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));
#pragma endregion _LIGHTS
		// Get the view, model and projection matrices
		view = curCamera->getView();
		projection = glm::perspective(glm::radians(curCamera->getZoom()), curWindow->getRatio(), 0.1f, 100.0f);
		model = glm::mat4(1.0f);

		// Set the matrices in the lighting shader
		lightingShader.setMat4("projection", projection);
		lightingShader.setMat4("view", view);
		lightingShader.setMat4("model", model);

		// render containers
		glBindVertexArray(cubeVAO);
		for (unsigned int i = 0; i < 10; i++)
		{
			// calculate the model matrix for each object and pass it to shader before drawing
			model = glm::mat4(1.0f);
			model = glm::translate(model, cubePositions[i]);
			float angle = 20.0f * i;
			model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
			lightingShader.setMat4("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		// also draw the lamp object(s)
		lampShader.use();
		lampShader.setMat4("projection", projection);
		lampShader.setMat4("view", view);

		// we now draw as many light bulbs as we have point lights.
		glBindVertexArray(lightVAO);
		for (unsigned int i = 0; i < 4; i++)
		{
			model = glm::mat4(1.0f);
			model = glm::translate(model, pointLightPositions[i]);
			model = glm::scale(model, glm::vec3(0.2f));
			lampShader.setMat4("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
	}
#pragma endregion _MAIN_LOOP

#pragma region _TERMINATION
	// optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------
	glDeleteVertexArrays(1, &cubeVAO);
	glDeleteVertexArrays(1, &lightVAO);
	glDeleteBuffers(1, &VBO);
	// Clean all resources
	glfwTerminate();
	return 0;
#pragma endregion _TERMINATION
}
#pragma region _FUNCTIONS_DECLARATION
void framebuffer_size_callback(GLFWwindow * window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = (float) xpos;
		lastY = (float) ypos;
		firstMouse = false;
	}

	float xoffset = (float) xpos - lastX;
	float yoffset =  lastY - (float)ypos; // reversed since y-coordinates go from bottom to top

	lastX = (float) xpos;
	lastY = (float) ypos;

	//window.getCamera()->proccesMouseMovement(xoffset, yoffset);
	curCamera->ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	curCamera->ProcessMouseScroll((float) yoffset);
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
	// Close Window
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	// Move Camera
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		curCamera->processKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		curCamera->processKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		curCamera->processKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		curCamera->processKeyboard(RIGHT, deltaTime);
	// Switch CAMERAS
	if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS)
	{
		if ((float)lastFrame - lastFrame_Tab > 0.5f)
		{
			switchCameras();
			lastFrame_Tab = lastFrame;
		}
	}
}

// calculate frame difference
// --------------------------
void calculateDelta()
{
	deltaTime = (float)glfwGetTime() - lastFrame;
	lastFrame += deltaTime;
}

// operate the camera switching
// ----------------------------
void switchCameras()
{
	if ((int)curCamera->getID() == 1) curCamera = &camera2;
	else if ((int)curCamera->getID() == 2) curCamera = &camera1;
}

// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int loadTexture(char const * path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}


#pragma endregion _FUNCTIONS_DECLARATION
