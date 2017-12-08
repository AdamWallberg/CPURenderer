#pragma once

#include "Types.h"

class Shader;

class Renderer
{
public:
	Renderer();
	~Renderer();

	void render();
private:
	void createTexture();

	uint* pixels_;
	uint width_;
	uint height_;
	uint numPixels_;

	uint texture_;

	Shader* quadShader_;
};