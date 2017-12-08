#pragma once

#include "Core.h"

class Camera
{
public:
	Camera(float fov, float aspect, float near, float far);
	void update();

	glm::vec3 pos_;
	glm::vec3 rot_;
	glm::mat4 proj_;
	glm::mat4 view_;
	float fov_;
	float near_;
	float far_;
};