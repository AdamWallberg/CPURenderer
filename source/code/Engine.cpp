#include "Engine.h"

Engine::Engine()
{
}

void Engine::init()
{
	logger_ = newp Logger();
	window_ = newp Window("CPURenderer", 800, 800, false, false);
	clock_ = newp Clock();
	renderer_ = newp Renderer();

	logger_->log("Engine initialized..", LOG_SYSTEM);
}

Engine::~Engine()
{
	logger_->writeToFile("log.txt");

	delete renderer_;
	delete clock_;
	delete window_;
	delete logger_;
}


bool Engine::update()
{
	window_->pollEvents();
	clock_->update();
	renderer_->render();

	static float timer = 0.0f;
	timer += DELTA_TIME;
	if (timer >= 1.0f)
	{
		timer -= 1.0f;
		window_->setTitle("FPS: " + std::to_string(1.0f / DELTA_TIME));
	}

	window_->swapBuffers();
	return !window_->shouldClose();
}