#include "Renderer.h"
#include <string.h>
#include "GL.h"
#include "Shader.h"
#include "Engine.h"
#include "Camera.h"
#include "Texture.h"
#include "Mesh.h"

Renderer::Renderer()
	: exitThreads_(false)
{
	width_ = ENGINE->window_->getWidth();
	//width_ = 512;
	height_ = ENGINE->window_->getHeight();
	//height_ = width_;
	numPixels_ = width_ * height_;
	pixels_ = newp uint[width_ * height_];
	zbuffer_ = newp float[width_ * height_];

	createTexture();
	setupQuad();

	quadShader_ = newp Shader("quad");

	camera_ = newp Camera(60.0f, (float)width_ / (float)height_, 0.1f, 300.0f);

	testMesh_ = newp Mesh("suzanne.obj");
	testTexture_ = newp Texture("test.bmp");

	for (int i = 0; i < NUM_THREADS; i++)
	{
		threads_[i] = newp std::thread(threadWorkerFunction, this, i);
		renderTasks_[i].reserve(1024);
	}
}

Renderer::~Renderer()
{
	exitThreads_ = true;
	for (int i = 0; i < 4; i++)
	{
		threads_[i]->join();
		delete threads_[i];
	}
	delete testTexture_;
	delete testMesh_;
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

	for (unsigned long long i = 0; i < numPixels_; i++)
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

void Renderer::drawVertexTriangle(VertexTriangle triangle, glm::mat4 model, bool hasUV)
{
	const glm::vec2 dimensions(width_, height_);

	glm::vec3 worldNormal = model * glm::vec4(triangle.normal(), 0);
	//glm::vec3 worldNormal = model * glm::vec4(triangle.v[0].normal, 0);
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

	if (triangle.v[0].p.w < camera_->near_ &&
		triangle.v[1].p.w < camera_->near_ &&
		triangle.v[2].p.w < camera_->near_)
		return;

	if (triangle.v[0].p.z < camera_->near_ &&
		triangle.v[1].p.z < camera_->near_ &&
		triangle.v[2].p.z < camera_->near_)
		return;

	if (triangle.v[0].p.z > camera_->far_ &&
		triangle.v[1].p.z > camera_->far_ &&
		triangle.v[2].p.z > camera_->far_)
		return;

	if (triangle.v[0].p.x < 0.0f &&
		triangle.v[1].p.x < 0.0f &&
		triangle.v[2].p.x < 0.0f)
		return;

	if (triangle.v[0].p.x >= width_ &&
		triangle.v[1].p.x >= width_ &&
		triangle.v[2].p.x >= width_)
		return;

	if (triangle.v[0].p.y < 0.0f &&
		triangle.v[1].p.y < 0.0f &&
		triangle.v[2].p.y < 0.0f)
		return;

	if (triangle.v[0].p.y >= height_ &&
		triangle.v[1].p.y >= height_ &&
		triangle.v[2].p.y >= height_)
		return;

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

	static const glm::vec3 sunDir = glm::normalize(glm::vec3(-1, -1, 1));
	float brightness = glm::clamp(glm::dot(-worldNormal, sunDir), 0.0f, 1.0f);
	brightness *= brightness;
	brightness = glm::clamp(brightness, 0.1f, 1.0f);

	int minTasks = INT_MAX;
	int taskList = 0;
	for (int i = 0; i < NUM_THREADS; i++)
	{
		if (renderTasks_[i].size() < minTasks)
		{
			minTasks = renderTasks_[i].size();
			taskList = i;
		}
	}

	std::lock_guard<std::mutex> guard(renderTasksMutex_[taskList]);
	RenderTask t;
	t.xmin = xmin;
	t.xmax = xmax;
	t.ymin = ymin;
	t.ymax = ymax;
	t.t = triangle;
	t.brightness = brightness;
	renderTasks_[taskList].push_back(t);
}

void Renderer::drawVertexBuffer(Vertex* buffer, uint numVertices, glm::mat4 model, bool hasUV)
{
	if (numVertices % 3 != 0)
		assert(false);

	for (uint i = 0; i < numVertices; i += 3)
	{
		VertexTriangle t;
		t.v[0] = buffer[i];
		t.v[1] = buffer[i + 1];
		t.v[2] = buffer[i + 2];

		drawVertexTriangle(t, model, hasUV);
	}
}

void Renderer::updateTexture()
{
	std::lock_guard<std::mutex> guard(bufferMutex_);

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

	camera_->update();

	clear(0);

	for (int i = 0; i < 3; i++)
	{
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(i * 2.f, 0, 0));
		model = glm::rotate(model, glm::radians(TIME * 90.0f), glm::vec3(0, 1, 0));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		drawVertexBuffer(testMesh_->getVertexPointer(), testMesh_->getNumVertices(), model, testMesh_->hasUV());
	}

	bool wait = true;
	while (wait)
	{
		wait = false;
		for (int i = 0; i < NUM_THREADS; i++)
		{
			std::lock_guard<std::mutex> guard(renderTasksMutex_[i]);
			if (renderTasks_[i].size() > 0)
				wait = true;
		}
	}

	updateTexture();

	quadShader_->bind();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture_);

	glBindVertexArray(screenQuad_);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Renderer::threadWorkerFunction(Renderer* r, int thread)
{
	bool hasTask = false;
	RenderTask t;

	while (true)
	{
		if (r->exitThreads_)
			return;

		if (!hasTask)
		{
			std::lock_guard<std::mutex> guard(r->renderTasksMutex_[thread]);
			int numTasks = r->renderTasks_[thread].size();
			if (numTasks <= 0)
				continue;

			t = r->renderTasks_[thread][0];
			hasTask = true;
		}
		else
		{
			for (int y = glm::clamp((int)t.ymin, 0, (int)r->height_); y < glm::clamp((int)t.ymax, 0, (int)r->height_) + 1; y++)
			{
				if (y < 0 || y >= r->height_)
					continue;

				for (int x = glm::clamp((int)t.xmin, 0, (int)r->width_); x < glm::clamp((int)t.xmax, 0, (int)r->width_) + 1; x++)
				{
					if (x < 0 || x >= r->width_)
						continue;

					const glm::vec2 p(x, y);
					if (t.t.pointIntersects(p))
					{
						Vertex v = t.t.getAt(p);

						std::lock_guard<std::mutex> bufferGuard(r->bufferMutex_);
						if (v.p.z < r->camera_->near_ ||
							v.p.z > r->camera_->far_ ||
							v.p.z > r->zbuffer_[x + y * r->width_])
							continue;

						glm::vec4 c;
						c = r->testTexture_->getTexelAt(v.uv);

						c.r *= t.brightness;
						c.g *= t.brightness;
						c.b *= t.brightness;

						int col = ((byte)c.a << 24) | ((byte)c.b << 16) | ((byte)c.g << 8) | (byte)c.r;
						//col += thread * 100000;

						r->pixels_[x + y * r->width_] = col;
						r->zbuffer_[x + y * r->width_] = v.p.z;
					}
				}
			}
			hasTask = false;
			std::lock_guard<std::mutex> guard(r->renderTasksMutex_[thread]);
			r->renderTasks_[thread].erase(r->renderTasks_[thread].begin());
		}
	}
}