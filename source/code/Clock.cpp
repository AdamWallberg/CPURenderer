#include "Clock.h"
#include "Types.h"
#include "GL.h"

Clock::Clock()
	: deltaTime(0.0f)
	, time(0.0f)
	, firstUpdate(true)
{
}

void Clock::update()
{
	if (firstUpdate)
	{
		lastTime = FLOAT_S(glfwGetTime());
		firstUpdate = false;
	}

	float newTime = FLOAT_S(glfwGetTime());
	deltaTime = newTime - lastTime;
	if (deltaTime >= 0.1f)
	{
		deltaTime = 0.1f;
	}

	time += deltaTime;

	lastTime = newTime;
}