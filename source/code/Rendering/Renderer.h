#pragma once

#include "Types.h"

class Shader;

class Renderer
{
public:
	Renderer();
	~Renderer();

	void render();
	void setPixel(int x, int y, int color);
	void clear(int color);
	void drawRect(int x, int y, int width, int height, int color);
private:
	void createTexture();
	void setupQuad();
	void updateTexture();

	uint* pixels_;
	uint width_;
	uint height_;
	uint numPixels_;

	uint texture_;

	Shader* quadShader_;
	uint screenQuad_;
};