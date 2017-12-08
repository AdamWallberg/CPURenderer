#pragma once

#include "GL.h"
#include "Core.h"
#include <string>

class Window
{
public:
	Window(
		std::string title, 
		uint width, 
		uint height, 
		bool fullscreen = false, 
		bool vsync = false);

	~Window();

	void pollEvents();
	void swapBuffers();
	bool shouldClose();

	int getWidth() const { return width_; }
	int getHeight() const { return height_; }

	void setTitle(const std::string& title);
private:
	void createWindow();
	void destroyWindow();

	GLFWwindow* window_;
	GLFWwindow* threadContext_;

	std::string title_;
	uint width_;
	uint height_;
	bool fullscreen_;
	bool vsync_;
};