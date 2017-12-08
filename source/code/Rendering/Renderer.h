#pragma once

#include "Types.h"
#include "Geometry.h"

class Shader;

class Renderer
{
public:
	Renderer();
	~Renderer();

	void render();
	void setPixel(int x, int y, int color);
	void clear(int color);
	void drawRect(Rect rect, int color);
	void drawCircle(Circle circle, int color);
	void drawTriangle(Triangle triangle, int color);
	void drawVertexTriangle(VertexTriangle triangle);
	void drawVertexBuffer(Vertex* buffer, uint numVertices);
private:
	void createTexture();
	void setupQuad();
	void updateTexture();

	uint* pixels_;
	float* zbuffer_;
	uint width_;
	uint height_;
	uint numPixels_;

	uint texture_;

	Shader* quadShader_;
	uint screenQuad_;

	float near_;
	float far_;
	float fov_;
};