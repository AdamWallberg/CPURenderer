#pragma once

#include "Core.h"
#include <string>

class Texture
{
public:
	Texture(std::string path);
	Texture(int width, int height, int color = 0xffffffff);

	uint getTexelAt(glm::vec2 pos);
private:
	uint* data_;
	int width_;
	int height_;
};