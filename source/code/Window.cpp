#include "Window.h"

#include "Engine.h"

void errorCallback(int error, const char* description)
{
	fprintf(stderr, "Error: %sn", description);
}

Window::Window(
	std::string title, 
	uint width, 
	uint height, 
	bool fullscreen /* = false */, 
	bool vsync /* = false */)
	: window_(nullptr)
	, threadContext_(nullptr)
	, title_(title)
	, width_(width)
	, height_(height)
	, fullscreen_(fullscreen)
	, vsync_(vsync)
{
	createWindow();
}



Window::~Window()
{
	destroyWindow();
}

void Window::createWindow()
{
	if (!glfwInit())
	{
		LOGGER->logError("Couldn't init GLFW!", LOG_SYSTEM);
		assert(false);
	}

	glfwSetErrorCallback(errorCallback);

	// Set window properties
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, false);

	// Create thread context
	glfwWindowHint(GLFW_VISIBLE, false);
	threadContext_ = glfwCreateWindow(
		1,
		1,
		"MinecraftClone_thread_context",
		nullptr,
		nullptr);

	if (!threadContext_)
	{
		glfwTerminate();
		LOGGER->logError(
			"Couldn't create thread context!", 
			LOG_SYSTEM);
		assert(false);
	}
	glfwWindowHint(GLFW_VISIBLE, true);

	// Create window
	GLFWmonitor* monitor = nullptr;
	if (fullscreen_)
	{
		monitor = glfwGetPrimaryMonitor();
		const GLFWvidmode* vidMode = glfwGetVideoMode(monitor);
		width_ = vidMode->width;
		height_ = vidMode->height;
	}

	window_ = glfwCreateWindow(
		width_,
		height_,
		title_.c_str(),
		monitor,
		threadContext_);

	if (!window_)
	{
		glfwTerminate();
		LOGGER->logError("Couldn't create window!", LOG_SYSTEM);
		assert(false);
	}

	glfwMakeContextCurrent(window_);
	glfwSwapInterval(vsync_);

	// Init glew
	glewExperimental = true;
	if (glewInit() != GLEW_OK)
	{
		glfwTerminate();
		LOGGER->logError("Couldn't init glew!", LOG_SYSTEM);
		assert(false);
	}
}

void Window::destroyWindow()
{
	glfwTerminate();
}

void Window::pollEvents()
{
	glfwPollEvents();
}

void Window::swapBuffers()
{
	glfwSwapBuffers(window_);
}

bool Window::shouldClose()
{
	return glfwWindowShouldClose(window_);
}