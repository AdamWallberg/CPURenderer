#pragma once

#include "Types.h"
#include "Geometry.h"
#include <thread>
#include <vector>
#include <mutex>

class Shader;
class Camera;
class Texture;
class Mesh;

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
	void drawVertexTriangle(VertexTriangle triangle, glm::mat4 model, bool hasUV = true);
	void drawVertexBuffer(Vertex* buffer, uint numVertices, glm::mat4 model, bool hasUV = true);
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

	Camera* camera_;

	Mesh* testMesh_;
	Texture* testTexture_;

	// Threading
	struct RenderTask
	{
		int xmin;
		int xmax;
		int ymin;
		int ymax;
		VertexTriangle t;
		float brightness;
	};

	bool exitThreads_;
	static const int NUM_THREADS = 4;
	std::thread* threads_[NUM_THREADS];
	std::vector<RenderTask> renderTasks_[NUM_THREADS];
	std::mutex renderTasksMutex_[NUM_THREADS];
	std::mutex bufferMutex_;

	static void threadWorkerFunction(Renderer* renderer, int zone);
};