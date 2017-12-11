#pragma once

#include <string>
#include "Geometry.h"
#include "Types.h"

class Mesh
{
public:
	Mesh(const char* name);
	~Mesh();

	Vertex* getVertexPointer() const { return vertices_; }
	uint getNumVertices() const { return numVertices_; }
	bool hasUV() const { return hasUV_; }

private:
	void loadFromObj(std::string path);

	Vertex* vertices_;
	uint numVertices_;
	glm::mat4 transformation_;

	bool hasUV_;
};