#pragma once

class Clock
{
public:
	Clock();
	void update();
	float deltaTime;
	float time;
private:
	float lastTime;
	bool firstUpdate;
};