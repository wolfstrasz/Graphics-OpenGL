#include "window.h"
Window::Window(int width, int height, glm::vec4 colors)
{
	mWidth = width;
	mHeight = height;
	mScrColors = colors;
}

void Window::update()
{
	glfwSwapBuffers(mWindow);
	calculateDelta();
	checkCameraError();
	processInput();
	glfwPollEvents();
	glfwGetFramebufferSize(mWindow, (int*)&mWidth, (int*)&mHeight);
	glViewport(0, 0, mWidth, mHeight);
	cls();
}

int Window::init()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	if (!createWindow() == 1) return -1;
	setCurrentContext();
	mLastFrame = 0.0f;
	mDeltaTime = 0.0f;
	glfwSetInputMode(mWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	return 1;
}

int Window::createWindow()
{
	mWindow = glfwCreateWindow(mWidth, mHeight, mTitle, nullptr, nullptr);
	if (mWindow == nullptr)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	return 1;
}



void Window::processInput()
{
	if (glfwGetKey(mWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(mWindow, true);
	//float cameraSpeed = 0.05f; // adjust accordingly
	if (glfwGetKey(mWindow, GLFW_KEY_W) == GLFW_PRESS)
		currentCamera->processKeyboard(FORWARD, mDeltaTime);
	if (glfwGetKey(mWindow, GLFW_KEY_S) == GLFW_PRESS)
		currentCamera->processKeyboard(BACKWARD, mDeltaTime);
	if (glfwGetKey(mWindow, GLFW_KEY_A) == GLFW_PRESS)
		//currentCamera->moveLeft(mDeltaTime);
		currentCamera->processKeyboard(LEFT, mDeltaTime);
	if (glfwGetKey(mWindow, GLFW_KEY_D) == GLFW_PRESS)
		currentCamera->processKeyboard(RIGHT, mDeltaTime);
}

void Window::cls()
{
	glClearColor(mScrColors.x, mScrColors.y, mScrColors.z, mScrColors.w);
	//glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

int Window::shouldClose()
{
	return glfwWindowShouldClose(mWindow);
}

void Window::bindCamera(Camera * camera)
{
	currentCamera = camera;
}
void Window::checkCameraError()
{
	if (currentCamera == nullptr)
	{
		std::cout << "ERROR: NO CAMERA BINDED TO CURRENT WINDOW!" << std::endl;
	}
}

Camera * Window::getCamera()
{
	return currentCamera;
}

GLFWwindow * Window::getWindow()
{
	return mWindow;
}

void Window::setCurrentContext()
{
	glfwMakeContextCurrent(mWindow);
}

float Window::getRatio()
{
	return ((float)mWidth / (float)mHeight);
}

void Window::calculateDelta()
{
	mDeltaTime = (float)glfwGetTime() - mLastFrame;
	mLastFrame += mDeltaTime;
}


