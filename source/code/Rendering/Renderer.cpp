#include "Renderer.h"
#include <string.h>
#include "GL.h"
#include "Shader.h"
#include "Engine.h"
#include "Camera.h"
#include "Texture.h"

Renderer::Renderer()
{
	width_ = ENGINE->window_->getWidth() / 4;
	//width_ = width_ / 8;
	height_ = ENGINE->window_->getHeight() / 4;
	//height_ = width_;
	numPixels_ = width_ * height_;
	pixels_ = newp uint[width_ * height_];
	zbuffer_ = newp float[width_ * height_];

	createTexture();
	setupQuad();

	quadShader_ = newp Shader("quad");

	camera_ = newp Camera(60.0f, (float)width_ / (float)height_, 0.1f, 1000.0f);

	testTexture_ = newp Texture("test.bmp");
}

Renderer::~Renderer()
{
	delete camera_;
	delete[] zbuffer_;
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
	if (x < 0 || x >= (int)width_)
		return;
	if (y < 0 || y >= (int)height_)
		return;

	pixels_[x + y * width_] = color;
}

void Renderer::clear(int color)
{
	memset(pixels_, color, sizeof(uint) * numPixels_);

	for (uint i = 0; i < numPixels_; i++)
	{
		zbuffer_[i] = 0.0f;
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

void Renderer::drawVertexTriangle(VertexTriangle triangle, glm::mat4 model)
{
	const glm::vec2 dimensions(width_, height_);

	glm::vec3 worldNormal = model * glm::vec4(triangle.normal(), 0);
	worldNormal = glm::normalize(worldNormal);

	for (byte i = 0; i < 3; i++)
	{
		triangle.v[i].p = camera_->proj_ * camera_->view_ * model * triangle.v[i].p;

		if (triangle.v[i].p.x == 0.0f)
			triangle.v[i].p.x = 0.001f;
		if (triangle.v[i].p.y == 0.0f)
			triangle.v[i].p.y = 0.001f;

		triangle.v[i].p.x /= triangle.v[i].p.w;
		triangle.v[i].p.y /= triangle.v[i].p.w;

		triangle.v[i].p.x *= -1.0f;


		triangle.v[i].p.x = triangle.v[i].p.x * dimensions.x * 0.5f + dimensions.x * 0.5f;
		triangle.v[i].p.y = triangle.v[i].p.y * dimensions.y * 0.5f + dimensions.y * 0.5f;
	}

	glm::vec3 normal = triangle.normal();
	if (normal.z > 0.0f)
		return;

	float xmin = glm::min(triangle.v[0].p.x, triangle.v[1].p.x);
	xmin = glm::min(xmin, triangle.v[2].p.x);
	float xmax = glm::max(triangle.v[0].p.x, triangle.v[1].p.x);
	xmax = glm::max(xmax, triangle.v[2].p.x);

	xmin = glm::max(xmin, 0.0f);
	xmax = glm::min(xmax, (float)width_);

	float ymin = glm::min(triangle.v[0].p.y, triangle.v[1].p.y);
	ymin = glm::min(ymin, triangle.v[2].p.y);
	float ymax = glm::max(triangle.v[0].p.y, triangle.v[1].p.y);
	ymax = glm::max(ymax, triangle.v[2].p.y);

	ymin = glm::max(ymin, 0.0f);
	ymax = glm::min(ymax, (float)height_);

	for (int y = (int)ymin; y < (int)ymax + 1; y++)
	{
		if (y < 0 || y >= (int)height_)
			continue;
		for (int x = (int)xmin; x < (int)xmax + 1; x++)
		{
			if (x < 0 || x >= (int)width_)
				continue;

			glm::vec2 p(x, y);
			if (triangle.pointIntersects(p))
			{
				Vertex v = triangle.getAt(p);
				if (v.p.z < zbuffer_[x + y * width_] || v.p.z < camera_->near_)
					continue;

				static const glm::vec3 sunDir = glm::normalize(glm::vec3(-1, -1, 1));
				float brightness = glm::clamp(glm::dot(-worldNormal, sunDir), 0.2f, 1.0f);

				//v.c *= 255.0f;
				//byte r = (byte)(v.c.r);
				//byte g = (byte)(v.c.g);
				//byte b = (byte)(v.c.b);
				//byte a = (byte)(v.c.a);
				//
				//int c = (a << 24) | (b << 16) | (g << 8) | r;

				int c = testTexture_->getTexelAt(v.uv);

				float a = (float)((c >> 24) & 0xff);
				float r = (float)((c >> 16) & 0xff);
				float g = (float)((c >> 8) & 0xff);
				float b = (float)((c) & 0xff);

				r *= brightness;
				g *= brightness;
				b *= brightness;

				c = ((byte)a << 24) | ((byte)b << 16) | ((byte)g << 8) | (byte)r;

				pixels_[x + y * width_] = c;
				zbuffer_[x + y * width_] = v.p.z;
			}
		}
	}
}

void Renderer::drawVertexBuffer(Vertex* buffer, uint numVertices, glm::mat4 model)
{
	if (numVertices % 3 != 0)
		assert(false);

	for (uint i = 0; i < numVertices; i += 3)
	{
		VertexTriangle t;
		t.v[0] = buffer[i];
		t.v[1] = buffer[i + 1];
		t.v[2] = buffer[i + 2];

		drawVertexTriangle(t, model);
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

	camera_->update();

	clear(0);

	glm::mat4 model = glm::mat4(1.0f);
	//model = glm::translate(glm::vec3(0, 0, TIME));

	Vertex quadBuffer[] = {
		{ glm::vec4(-1, 1, 0, 1), glm::vec4(1, 1, 1, 1),	glm::vec2(0, 1) },
		{ glm::vec4(-1, -1, 0, 1), glm::vec4(1, 1, 1, 1),	glm::vec2(0, 0) },
		{ glm::vec4(1, -1, 0, 1), glm::vec4(1, 1, 1, 1),	glm::vec2(1, 0) },
		{ glm::vec4(-1, 1, 0, 1), glm::vec4(1, 1, 1, 1),	glm::vec2(0, 1) },
		{ glm::vec4(1, -1, 0, 1), glm::vec4(1, 1, 1, 1),	glm::vec2(1, 0) },
		{ glm::vec4(1, 1, 0, 1), glm::vec4(1, 1, 1, 1),		glm::vec2(1, 1) },
	};

	//drawVertexBuffer(quadBuffer, 6, model);

	model = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f, 0.5f, 0.5f));
	model = glm::rotate(model, glm::radians(TIME * 90.0f), glm::vec3(0, 1, 1));

	Vertex vertexBuffer[] = {
		{ glm::vec4(-1.0f, -1.0f, -1.0f, 1.0f), glm::vec4(1.0f, 1.0f, 0.0f, 1.0f), glm::vec2(0, 1) },
		{ glm::vec4(-1.0f, -1.0f, 1.0f, 1.0f), glm::vec4(1.0f, 1.0f, 0.0f, 1.0f), glm::vec2(0, 0) },
		{ glm::vec4(-1.0f, 1.0f, 1.0f, 1.0f), glm::vec4(1.0f, 1.0f, 0.0f, 1.0f), glm::vec2(1, 0) },
		{ glm::vec4(1.0f, 1.0f, -1.0f, 1.0f), glm::vec4(1.0f, 1.0f, 0.0f, 1.0f), glm::vec2(0, 1) },
		{ glm::vec4(-1.0f, -1.0f, -1.0f, 1.0f), glm::vec4(1.0f, 1.0f, 0.0f, 1.0f), glm::vec2(1, 0) },
		{ glm::vec4(-1.0f, 1.0f, -1.0f, 1.0f), glm::vec4(1.0f, 1.0f, 0.0f, 1.0f), glm::vec2(1, 1) },
		{ glm::vec4(1.0f, -1.0f, 1.0f, 1.0f), glm::vec4(1.0f, 1.0f, 0.0f, 1.0f), glm::vec2(0, 1) },
		{ glm::vec4(-1.0f, -1.0f, -1.0f, 1.0f), glm::vec4(1.0f, 1.0f, 0.0f, 1.0f), glm::vec2(0, 0) },
		{ glm::vec4(1.0f, -1.0f, -1.0f, 1.0f), glm::vec4(1.0f, 1.0f, 0.0f, 1.0f), glm::vec2(1, 0) },
		{ glm::vec4(1.0f, 1.0f, -1.0f, 1.0f), glm::vec4(1.0f, 1.0f, 0.0f, 1.0f), glm::vec2(0, 1) },
		{ glm::vec4(1.0f, -1.0f, -1.0f, 1.0f), glm::vec4(1.0f, 1.0f, 0.0f, 1.0f), glm::vec2(1, 0) },
		{ glm::vec4(-1.0f, -1.0f, -1.0f, 1.0f), glm::vec4(1.0f, 1.0f, 0.0f, 1.0f), glm::vec2(1, 1) },
		{ glm::vec4(-1.0f, -1.0f, -1.0f, 1.0f), glm::vec4(1.0f, 1.0f, 0.0f, 1.0f), glm::vec2(0, 1) },
		{ glm::vec4(-1.0f, 1.0f, 1.0f, 1.0f), glm::vec4(1.0f, 1.0f, 0.0f, 1.0f), glm::vec2(0, 0) },
		{ glm::vec4(-1.0f, 1.0f, -1.0f, 1.0f), glm::vec4(1.0f, 1.0f, 0.0f, 1.0f), glm::vec2(1, 0) },
		{ glm::vec4(1.0f, -1.0f, 1.0f, 1.0f), glm::vec4(1.0f, 1.0f, 0.0f, 1.0f), glm::vec2(0, 1) },
		{ glm::vec4(-1.0f, -1.0f, 1.0f, 1.0f), glm::vec4(1.0f, 1.0f, 0.0f, 1.0f), glm::vec2(1, 0) },
		{ glm::vec4(-1.0f, -1.0f, -1.0f, 1.0f), glm::vec4(1.0f, 1.0f, 0.0f, 1.0f), glm::vec2(1, 1) },
		{ glm::vec4(-1.0f, 1.0f, 1.0f, 1.0f), glm::vec4(1.0f, 1.0f, 0.0f, 1.0f), glm::vec2(0, 1) },
		{ glm::vec4(-1.0f, -1.0f, 1.0f, 1.0f), glm::vec4(1.0f, 1.0f, 0.0f, 1.0f), glm::vec2(0, 0) },
		{ glm::vec4(1.0f, -1.0f, 1.0f, 1.0f), glm::vec4(1.0f, 1.0f, 0.0f, 1.0f), glm::vec2(1, 0) },
		{ glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), glm::vec4(1.0f, 1.0f, 0.0f, 1.0f), glm::vec2(0, 1) },
		{ glm::vec4(1.0f, -1.0f, -1.0f, 1.0f), glm::vec4(1.0f, 1.0f, 0.0f, 1.0f), glm::vec2(1, 0) },
		{ glm::vec4(1.0f, 1.0f, -1.0f, 1.0f), glm::vec4(1.0f, 1.0f, 0.0f, 1.0f), glm::vec2(1, 1) },
		{ glm::vec4(1.0f, -1.0f, -1.0f, 1.0f), glm::vec4(1.0f, 1.0f, 0.0f, 1.0f), glm::vec2(0, 1) },
		{ glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), glm::vec4(1.0f, 1.0f, 0.0f, 1.0f), glm::vec2(0, 0) },
		{ glm::vec4(1.0f, -1.0f, 1.0f, 1.0f), glm::vec4(1.0f, 1.0f, 0.0f, 1.0f), glm::vec2(1, 0) },
		{ glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), glm::vec4(1.0f, 1.0f, 0.0f, 1.0f), glm::vec2(0, 1) },
		{ glm::vec4(1.0f, 1.0f, -1.0f, 1.0f), glm::vec4(1.0f, 1.0f, 0.0f, 1.0f), glm::vec2(1, 0) },
		{ glm::vec4(-1.0f, 1.0f, -1.0f, 1.0f), glm::vec4(1.0f, 1.0f, 0.0f, 1.0f), glm::vec2(1, 1) },
		{ glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), glm::vec4(1.0f, 1.0f, 0.0f, 1.0f), glm::vec2(0, 1) },
		{ glm::vec4(-1.0f, 1.0f, -1.0f, 1.0f), glm::vec4(1.0f, 1.0f, 0.0f, 1.0f), glm::vec2(0, 0) },
		{ glm::vec4(-1.0f, 1.0f, 1.0f, 1.0f), glm::vec4(1.0f, 1.0f, 0.0f, 1.0f), glm::vec2(1, 0) },
		{ glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), glm::vec4(1.0f, 1.0f, 0.0f, 1.0f), glm::vec2(0, 1) },
		{ glm::vec4(-1.0f, 1.0f, 1.0f, 1.0f), glm::vec4(1.0f, 1.0f, 0.0f, 1.0f), glm::vec2(1, 0) },
		{ glm::vec4(1.0f, -1.0f, 1.0f, 1.0f), glm::vec4(1.0f, 1.0f, 0.0f, 1.0f), glm::vec2(1, 1) },
	};

	srand(1);
	for (int i = 0; i < 36; i++)
	{
		Vertex& v = vertexBuffer[i];
		uint c = rand();

		float r = (c >> 16) & 255;
		float g = (c >> 8) & 255;
		float b = (c) & 255;

		v.c = glm::vec4(r / 255, g / 255, b / 255, 1.0f);
	}

	drawVertexBuffer(vertexBuffer, 36, model);

	quadShader_->bind();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture_);

	glBindVertexArray(screenQuad_);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}