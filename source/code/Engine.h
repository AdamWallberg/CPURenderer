#pragma once

#include "ISingleton.h"

#include "Window.h"
#include "Logger.h"

#define ENGINE Engine::get()
#define LOGGER ENGINE->logger_

class Engine : public ISingleton<Engine>
{
public:
	Engine();
	~Engine();
	bool update();

	Window* window_;
	Logger* logger_;
};