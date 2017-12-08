#include "Camera.h"
#include "GL.h"
#include "Engine.h"

Camera::Camera(float fov, float aspect, float near, float far)
{
	fov_ = fov;
	near_ = near;
	far_ = far;
	proj_ = glm::perspective(glm::radians(fov), aspect, near, far);
	pos_ = glm::vec3(0, 0, 0);
	rot_ = glm::vec3(0.0f, 0.0f, 0.0f);
}

void Camera::update()
{
	// Camera rotation
	if (glfwGetKey(ENGINE->window_->getWindow(), GLFW_KEY_LEFT))
		rot_.y += 90.0f * DELTA_TIME;
	if (glfwGetKey(ENGINE->window_->getWindow(), GLFW_KEY_RIGHT))
		rot_.y -= 90.0f * DELTA_TIME;

	// Setup at vector
	glm::vec3 at(
		glm::sin(glm::radians(rot_.y)) * glm::cos(glm::radians(rot_.x)),
		glm::sin(glm::radians(rot_.x)),
		glm::cos(glm::radians(rot_.y)) * glm::cos(glm::radians(rot_.x)));
	// TODO: Proper
	glm::vec3 right = glm::cross(at, glm::vec3(0, 1, 0));

	// Movement
	if (glfwGetKey(ENGINE->window_->getWindow(), GLFW_KEY_W))
		pos_ += at * DELTA_TIME;
	if (glfwGetKey(ENGINE->window_->getWindow(), GLFW_KEY_S))
		pos_ -= at * DELTA_TIME;
	if (glfwGetKey(ENGINE->window_->getWindow(), GLFW_KEY_A))
		pos_ -= right * DELTA_TIME;
	if (glfwGetKey(ENGINE->window_->getWindow(), GLFW_KEY_D))
		pos_ += right * DELTA_TIME;

	// Setup matrices
	view_ = glm::lookAt(pos_, pos_ + at, glm::vec3(0, 1, 0));
}