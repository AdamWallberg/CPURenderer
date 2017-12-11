#pragma once

#include "Core.h"
#include <string>

class Texture
{
public:
	Texture(std::string path);
	Texture(int width, int height, int color = 0xffffffff);
	~Texture();

	glm::vec4 getTexelAt(glm::vec2 pos) const;
private:
	glm::vec4* data_;
	int width_;
	int height_;
};