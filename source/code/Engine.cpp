#include "Engine.h"

Engine::Engine()
{
	logger_ = newp Logger();
	window_ = newp Window("Minecraft clone", 800, 800);

	logger_->log("Engine initialized..", LOG_SYSTEM);
}

Engine::~Engine()
{
	logger_->writeToFile("log.txt");

	delete window_;
	delete logger_;
}


bool Engine::update()
{
	window_->pollEvents();
	window_->swapBuffers();

	return !window_->shouldClose();
}