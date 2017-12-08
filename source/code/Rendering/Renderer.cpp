#include "Renderer.h"
#include <string.h>
#include "GL.h"
#include "Shader.h"


Renderer::Renderer()
{
	width_ = 128;
	height_ = 128;
	numPixels_ = width_ * height_;
	pixels_ = new uint[width_ * height_];
	memset(pixels_, 0, sizeof(uint) * numPixels_);

	createTexture();

	quadShader_ = new Shader("quad");
}

Renderer::~Renderer()
{

}

void Renderer::createTexture()
{
	glGenTextures(1, &texture_);
	glBindTexture(GL_TEXTURE_2D, texture_);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_POINT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_POINT);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width_, height_, 0, GL_RGBA, GL_UNSIGNED_INT, pixels_);

}

void Renderer::render()
{
	
}