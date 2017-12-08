#include "Renderer.h"
#include <string.h>
#include "GL.h"
#include "Shader.h"
#include "Engine.h"

Renderer::Renderer()
{
	width_ = 128;
	height_ = 128;
	numPixels_ = width_ * height_;
	pixels_ = new uint[width_ * height_];
	memset(pixels_, 255, sizeof(uint) * numPixels_);

	// TEMP
	for (uint i = 0; i <numPixels_; i++)
	{
		pixels_[i] = 0x000000;
	}

	createTexture();
	setupQuad();

	quadShader_ = new Shader("quad");
}

Renderer::~Renderer()
{

}

void Renderer::createTexture()
{
	glGenTextures(1, &texture_);
	glBindTexture(GL_TEXTURE_2D, texture_);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width_, height_, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels_);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

void Renderer::setupQuad()
{
	static const float quadVertices[] =
	{
		-1.0f, 1.0f, 0.0f, 1.0f,
		-1.0f, -1.0f, 0.0f, 0.0f,
		1.0f, -1.0f, 1.0f, 0.0f,

		-1.0f, 1.0f, 0.0f, 1.0f,
		1.0f, -1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 1.0f, 1.0f
	};

	glGenVertexArrays(1, &screenQuad_);
	glGenBuffers(1, &screenQuad_);
	glBindVertexArray(screenQuad_);
	glBindBuffer(GL_ARRAY_BUFFER, screenQuad_);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(sizeof(float) * 2));
}

void Renderer::setPixel(int x, int y, int color)
{
	x = glm::clamp(x, 0, (int)width_);
	y = glm::clamp(y, 0, (int)height_);

	pixels_[x + y * width_] = color;
}

void Renderer::clear(int color)
{
	drawRect(0, 0, width_, height_, color);
}

void Renderer::drawRect(int x, int y, int width, int height, int color)
{
	for (int yy = y; yy < y + height; yy++)
	{
		yy = glm::clamp(yy, 0, (int)height_);
		for (int xx = x; xx < x + width; xx++)
		{
			xx = glm::clamp(xx, 0, (int)width_);
			pixels_[xx + yy * width_] = color;
		}
	}
}

void Renderer::updateTexture()
{
	glBindTexture(GL_TEXTURE_2D, texture_);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width_, height_, GL_RGBA, GL_UNSIGNED_BYTE, pixels_);
}

void Renderer::render()
{
	updateTexture();

	clear(0);
	//setPixel((int)TIME, (int)TIME, 0x0000ff);
	drawRect((int)TIME, (int)TIME, 4, 4, 0x0000ff);

	//glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	quadShader_->bind();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture_);

	glBindVertexArray(screenQuad_);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}