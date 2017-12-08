#include "Renderer.h"
#include <string.h>
#include "GL.h"
#include "Shader.h"
#include "Engine.h"
#define  GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>

Renderer::Renderer()
{
	width_ = ENGINE->window_->getWidth() / 3;
	//width_ = 64;
	height_ = ENGINE->window_->getHeight() / 3;
	//height_ = width_;
	numPixels_ = width_ * height_;
	pixels_ = newp uint[width_ * height_];
	memset(pixels_, 255, sizeof(uint) * numPixels_);
	zbuffer_ = newp float[width_ * height_];
	memset(zbuffer_, 0.0f, sizeof(float) * numPixels_);

	near_ = 0.1f;
	far_ = 1000.0f;
	fov_ = 45.0f;

	// TEMP
	for (uint i = 0; i <numPixels_; i++)
	{
		pixels_[i] = 0x00000000;
	}

	createTexture();
	setupQuad();

	quadShader_ = newp Shader("quad");
}

Renderer::~Renderer()
{
	delete[] pixels_;
	delete quadShader_;
}

void Renderer::createTexture()
{
	glGenTextures(1, &texture_);
	glBindTexture(GL_TEXTURE_2D, texture_);

	glTexImage2D(
		GL_TEXTURE_2D, 
		0, 
		GL_RGBA, 
		width_, 
		height_, 
		0, 
		GL_RGBA, 
		GL_UNSIGNED_BYTE, 
		pixels_);

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
	// TODO:
	x = glm::clamp(x, 0, (int)width_);
	y = glm::clamp(y, 0, (int)height_);

	pixels_[x + y * width_] = color;
}

void Renderer::clear(int color)
{
	memset(pixels_, color, sizeof(uint) * numPixels_);

	for (uint i = 0; i < numPixels_; i++)
	{
		zbuffer_[i] = std::numeric_limits<float>::max();
	}
}

void Renderer::drawRect(Rect rect, int color)
{
	for (float y = rect.pos.y; y < rect.pos.y + rect.size.y; y++)
	{
		if ((int)y < 0 || (int)y >= (int)height_)
			continue;
		for (float x = rect.pos.x; x < rect.pos.x + rect.size.x; x++)
		{
			if ((int)x < 0 || (int)x >= (int)width_)
				continue;
			pixels_[(int)x + (int)y * (int)width_] = color;
		}
	}
}

void Renderer::drawCircle(Circle circle, int color)
{
	float r = circle.radius;
	for (float y = circle.pos.y - r; y < circle.pos.y + r; y++)
	{
		if ((int)y < 0 || (int)y >= (int)height_)
			continue;
		for (float x = circle.pos.x - r; x < circle.pos.x + r; x++)
		{
			if ((int)x < 0 || (int)x >= (int)width_)
				continue;
			
			glm::vec2 p(x, y);
			glm::vec2 dir = circle.pos - p;
			float distSqr = dir.x * dir.x + dir.y * dir.y;
			if (distSqr < r * r)
			{
				pixels_[(int)x + (int)y * width_] = color;
			}
		}
	}
}

void Renderer::drawTriangle(Triangle triangle, int color)
{
	float xmin = glm::min(triangle.p0.x, triangle.p1.x);
	xmin = glm::min(xmin, triangle.p2.x);
	float xmax = glm::max(triangle.p0.x, triangle.p1.x);
	xmax = glm::max(xmax, triangle.p2.x);

	float ymin = glm::min(triangle.p0.y, triangle.p1.y);
	ymin = glm::min(ymin, triangle.p2.y);
	float ymax = glm::max(triangle.p0.y, triangle.p1.y);
	ymax = glm::max(ymax, triangle.p2.y);

	for (float y = ymin; y < ymax; y++)
	{
		if ((int)y < 0 || (int)y >= (int)height_)
			continue;
		for (float x = xmin; x < xmax; x++)
		{
			if ((int)x < 0 || (int)x >= (int)width_)
				continue;

			glm::vec2 p(x, y);
			if (triangle.pointIntersects(p))
			{
				pixels_[(int)x + (int)y * width_] = color;
			}
		}
	}
}

void Renderer::drawVertexTriangle(VertexTriangle triangle)
{
	if (triangle.p0.p.x == 0.0f)
		triangle.p0.p.x = 0.001f;
	if (triangle.p0.p.y == 0.0f)
		triangle.p0.p.y = 0.001f;

	if (triangle.p1.p.x == 0.0f)
		triangle.p1.p.x = 0.001f;
	if (triangle.p1.p.y == 0.0f)
		triangle.p1.p.y = 0.001f;

	if (triangle.p2.p.x == 0.0f)
		triangle.p2.p.x = 0.001f;
	if (triangle.p2.p.y == 0.0f)
		triangle.p2.p.y = 0.001f;

	triangle.p0.p.x /= triangle.p0.p.w;
	triangle.p0.p.y /= triangle.p0.p.w;
	triangle.p1.p.x /= triangle.p1.p.w;
	triangle.p1.p.y /= triangle.p1.p.w;
	triangle.p2.p.x /= triangle.p2.p.w;
	triangle.p2.p.y /= triangle.p2.p.w;


	glm::vec2 dimensions(width_, height_);
	triangle.p0.p.x = triangle.p0.p.x * dimensions.x * 0.5f + dimensions.x * 0.5f;
	triangle.p0.p.y = triangle.p0.p.y * dimensions.y * 0.5f + dimensions.y * 0.5f;
	triangle.p1.p.x = triangle.p1.p.x * dimensions.x * 0.5f + dimensions.x * 0.5f;
	triangle.p1.p.y = triangle.p1.p.y * dimensions.y * 0.5f + dimensions.y * 0.5f;
	triangle.p2.p.x = triangle.p2.p.x * dimensions.x * 0.5f + dimensions.x * 0.5f;
	triangle.p2.p.y = triangle.p2.p.y * dimensions.y * 0.5f + dimensions.y * 0.5f;

	float xmin = glm::min(triangle.p0.p.x, triangle.p1.p.x);
	xmin = glm::min(xmin, triangle.p2.p.x);
	float xmax = glm::max(triangle.p0.p.x, triangle.p1.p.x);
	xmax = glm::max(xmax, triangle.p2.p.x);

	xmin = glm::max(xmin, 0.0f);
	xmax = glm::min(xmax, (float)width_);

	float ymin = glm::min(triangle.p0.p.y, triangle.p1.p.y);
	ymin = glm::min(ymin, triangle.p2.p.y);
	float ymax = glm::max(triangle.p0.p.y, triangle.p1.p.y);
	ymax = glm::max(ymax, triangle.p2.p.y);

	ymin = glm::max(ymin, 0.0f);
	ymax = glm::min(ymax, (float)height_);

	for (int y = ymin; y < ymax; y++)
	{
		if (y < 0 || y >= (int)height_)
			continue;
		for (int x = xmin; x < xmax; x++)
		{
			if (x < 0 || x >= (int)width_)
				continue;

			glm::vec2 p(x, y);
			if (triangle.pointIntersects(p))
			{
				Vertex v = triangle.getAt(glm::vec3(p, 0.0f));
				if (v.p.z > zbuffer_[x + y * width_])
					continue;

				v.c *= 255.0f;
				byte r = (byte)v.c.r;
				byte g = (byte)v.c.g;
				byte b = (byte)v.c.b;
				byte a = (byte)v.c.a;

				int c = (a << 24) | (b << 16) | (g << 8) | r;

				pixels_[(int)x + (int)y * width_] = c;
				zbuffer_[(int)x + (int)y * width_] = v.p.z;
			}
		}
	}
}

void Renderer::drawVertexBuffer(Vertex* buffer, uint numVertices)
{
	if (numVertices % 3 != 0)
		assert(false);

	for (uint i = 0; i < numVertices; i += 3)
	{
		VertexTriangle t;
		t.p0 = buffer[i];
		t.p1 = buffer[i + 1];
		t.p2 = buffer[i + 2];

		drawVertexTriangle(t);
	}
}

void Renderer::updateTexture()
{
	glBindTexture(GL_TEXTURE_2D, texture_);
	glTexSubImage2D(
		GL_TEXTURE_2D, 
		0, 
		0, 
		0, 
		width_, 
		height_, 
		GL_RGBA, 
		GL_UNSIGNED_BYTE, 
		pixels_);
}

void Renderer::render()
{
	updateTexture();

	clear(0);

	glm::mat4 proj = glm::perspective(glm::radians(fov_), (float)width_ / (float)height_, near_, far_);
	//glm::vec3 camPos = glm::vec3(glm::sin(glm::radians(TIME * 90.0f)) * 5.0f, 0.0f, glm::cos(glm::radians(TIME * 90.0f)) * 5.0f);
	glm::vec3 camPos(0, 0, 5);
	glm::mat4 view = glm::lookAt(camPos, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 mvp = proj * view * model;

	//VertexTriangle t;
	//t.p0 = { mvp * glm::vec4(-1, -1, 0, 1), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f) };
	//t.p1 = { mvp * glm::vec4(1, -1, 0, 1), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f) };
	//t.p2 = { mvp * glm::vec4(-1, 1, 0, 1), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f) };
	//
	//drawVertexTriangle(t);

	model = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f, 0.5f, 0.5f));
	model = glm::rotate(model, glm::radians(TIME * 90.0f), glm::vec3(0, 1, 1));
	mvp = proj * view * model;
	
	Vertex vertexBuffer[] = {
		{ mvp * glm::vec4(-1.0f, -1.0f, -1.0f, 1.0f), glm::vec4(1.0f, 1.0f, 0.0f, 1.0f) },
		{ mvp * glm::vec4(-1.0f, -1.0f, 1.0f, 1.0f), glm::vec4(1.0f, 1.0f, 0.0f, 1.0f) },
		{ mvp * glm::vec4(-1.0f, 1.0f, 1.0f, 1.0f), glm::vec4(1.0f, 1.0f, 0.0f, 1.0f) },
		{ mvp * glm::vec4(1.0f, 1.0f, -1.0f, 1.0f), glm::vec4(1.0f, 1.0f, 0.0f, 1.0f) },
		{ mvp * glm::vec4(-1.0f, -1.0f, -1.0f, 1.0f), glm::vec4(1.0f, 1.0f, 0.0f, 1.0f) },
		{ mvp * glm::vec4(-1.0f, 1.0f, -1.0f, 1.0f), glm::vec4(1.0f, 1.0f, 0.0f, 1.0f) },
		{ mvp * glm::vec4(1.0f, -1.0f, 1.0f, 1.0f), glm::vec4(1.0f, 1.0f, 0.0f, 1.0f) },
		{ mvp * glm::vec4(-1.0f, -1.0f, -1.0f, 1.0f), glm::vec4(1.0f, 1.0f, 0.0f, 1.0f) },
		{ mvp * glm::vec4(1.0f, -1.0f, -1.0f, 1.0f), glm::vec4(1.0f, 1.0f, 0.0f, 1.0f) },
		{ mvp * glm::vec4(1.0f, 1.0f, -1.0f, 1.0f), glm::vec4(1.0f, 1.0f, 0.0f, 1.0f) },
		{ mvp * glm::vec4(1.0f, -1.0f, -1.0f, 1.0f), glm::vec4(1.0f, 1.0f, 0.0f, 1.0f) },
		{ mvp * glm::vec4(-1.0f, -1.0f, -1.0f, 1.0f), glm::vec4(1.0f, 1.0f, 0.0f, 1.0f) },
		{ mvp * glm::vec4(-1.0f, -1.0f, -1.0f, 1.0f), glm::vec4(1.0f, 1.0f, 0.0f, 1.0f) },
		{ mvp * glm::vec4(-1.0f, 1.0f, 1.0f, 1.0f), glm::vec4(1.0f, 1.0f, 0.0f, 1.0f) },
		{ mvp * glm::vec4(-1.0f, 1.0f, -1.0f, 1.0f), glm::vec4(1.0f, 1.0f, 0.0f, 1.0f) },
		{ mvp * glm::vec4(1.0f, -1.0f, 1.0f, 1.0f), glm::vec4(1.0f, 1.0f, 0.0f, 1.0f) },
		{ mvp * glm::vec4(-1.0f, -1.0f, 1.0f, 1.0f), glm::vec4(1.0f, 1.0f, 0.0f, 1.0f) },
		{ mvp * glm::vec4(-1.0f, -1.0f, -1.0f, 1.0f), glm::vec4(1.0f, 1.0f, 0.0f, 1.0f) },
		{ mvp * glm::vec4(-1.0f, 1.0f, 1.0f, 1.0f), glm::vec4(1.0f, 1.0f, 0.0f, 1.0f) },
		{ mvp * glm::vec4(-1.0f, -1.0f, 1.0f, 1.0f), glm::vec4(1.0f, 1.0f, 0.0f, 1.0f) },
		{ mvp * glm::vec4(1.0f, -1.0f, 1.0f, 1.0f), glm::vec4(1.0f, 1.0f, 0.0f, 1.0f) },
		{ mvp * glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), glm::vec4(1.0f, 1.0f, 0.0f, 1.0f) },
		{ mvp * glm::vec4(1.0f, -1.0f, -1.0f, 1.0f), glm::vec4(1.0f, 1.0f, 0.0f, 1.0f) },
		{ mvp * glm::vec4(1.0f, 1.0f, -1.0f, 1.0f), glm::vec4(1.0f, 1.0f, 0.0f, 1.0f) },
		{ mvp * glm::vec4(1.0f, -1.0f, -1.0f, 1.0f), glm::vec4(1.0f, 1.0f, 0.0f, 1.0f) },
		{ mvp * glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), glm::vec4(1.0f, 1.0f, 0.0f, 1.0f) },
		{ mvp * glm::vec4(1.0f, -1.0f, 1.0f, 1.0f), glm::vec4(1.0f, 1.0f, 0.0f, 1.0f) },
		{ mvp * glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), glm::vec4(1.0f, 1.0f, 0.0f, 1.0f) },
		{ mvp * glm::vec4(1.0f, 1.0f, -1.0f, 1.0f), glm::vec4(1.0f, 1.0f, 0.0f, 1.0f) },
		{ mvp * glm::vec4(-1.0f, 1.0f, -1.0f, 1.0f), glm::vec4(1.0f, 1.0f, 0.0f, 1.0f) },
		{ mvp * glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), glm::vec4(1.0f, 1.0f, 0.0f, 1.0f) },
		{ mvp * glm::vec4(-1.0f, 1.0f, -1.0f, 1.0f), glm::vec4(1.0f, 1.0f, 0.0f, 1.0f) },
		{ mvp * glm::vec4(-1.0f, 1.0f, 1.0f, 1.0f), glm::vec4(1.0f, 1.0f, 0.0f, 1.0f) },
		{ mvp * glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), glm::vec4(1.0f, 1.0f, 0.0f, 1.0f) },
		{ mvp * glm::vec4(-1.0f, 1.0f, 1.0f, 1.0f), glm::vec4(1.0f, 1.0f, 0.0f, 1.0f) },
		{ mvp * glm::vec4(1.0f, -1.0f, 1.0f, 1.0f), glm::vec4(1.0f, 1.0f, 0.0f, 1.0f) }
	};

	srand(1);
	for (int i = 0; i < 36; i++)
	{
		Vertex& v = vertexBuffer[i];
		uint c = rand();

		float r = (c >> 32) & 255;
		float g = (c >> 16) & 255;
		float b = (c >> 8) & 255;

		v.c = glm::vec4(r / 255, g / 255, b / 255, 1.0f);
	}

	drawVertexBuffer(vertexBuffer, 36);

	quadShader_->bind();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture_);

	glBindVertexArray(screenQuad_);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}