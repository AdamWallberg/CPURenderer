#pragma once

#include "ISingleton.h"

#include "Window.h"
#include "Logger.h"
#include "Clock.h"
#include "Rendering/Renderer.h"

#define ENGINE Engine::get()
#define LOGGER ENGINE->logger_
#define DELTA_TIME ENGINE->clock_->deltaTime
#define TIME ENGINE->clock_->time

class Engine : public ISingleton<Engine>
{
public:
	Engine();
	~Engine();

	void init();
	bool update();

	Window* window_;
	Logger* logger_;
	Clock* clock_;
	Renderer* renderer_;
};