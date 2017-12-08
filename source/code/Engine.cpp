#include "Engine.h"

Engine::Engine()
{
}

void Engine::init()
{
	logger_ = newp Logger();
	window_ = newp Window("Minecraft clone", 800, 800);
	clock_ = newp Clock();
	renderer_ = newp Renderer();

	logger_->log("Engine initialized..", LOG_SYSTEM);
}

Engine::~Engine()
{
	logger_->writeToFile("log.txt");

	delete renderer_;
	delete window_;
	delete logger_;
}


bool Engine::update()
{
	window_->pollEvents();
	clock_->update();
	renderer_->render();

	window_->swapBuffers();
	return !window_->shouldClose();
}