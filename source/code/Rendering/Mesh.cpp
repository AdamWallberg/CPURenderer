#include "Mesh.h"

#include <vector>
#include "Core.h"

Mesh::Mesh(const char* path)
{
	loadFromObj(path);
}

Mesh::~Mesh()
{
	delete[] vertices_;
}

void Mesh::loadFromObj(std::string path)
{
	path = "data/models/" + path;

	std::vector<uint> vertexIndices, uvIndices, normalIndices;
	std::vector<glm::vec3> positions;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;

	FILE* file = fopen(path.c_str(), "r");
	assert(file);

	while (true)
	{
		char lineHeader[128];
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
			break;

		if (strcmp(lineHeader, "v") == 0)
		{
			glm::vec3 position;
			fscanf(file, "%f %f %f\n", &position.x, &position.y, &position.z);
			positions.push_back(position);
		}
		else if (strcmp(lineHeader, "vt") == 0)
		{
			glm::vec2 uv;
			fscanf(file, "%f %f\n", &uv.x, &uv.y);
			uvs.push_back(uv);
		}
		else if (strcmp(lineHeader, "vn") == 0)
		{
			glm::vec3 normal;
			fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
			normals.push_back(normal);
		}
		else if (strcmp(lineHeader, "f") == 0)
		{
			std::string v1, v2, v3;
			uint vertexIndex[3];
			uint uvIndex[3];
			uint normalIndex[3];

			int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n",
				&vertexIndex[0],
				&uvIndex[0],
				&normalIndex[0],
				&vertexIndex[1],
				&uvIndex[1],
				&normalIndex[1],
				&vertexIndex[2],
				&uvIndex[2],
				&normalIndex[2]);

			if (matches == 9)
				hasUV_ = true;
			else if (matches == 6)
				hasUV_ = false;
			else
				assert(false);

			vertexIndices.push_back(vertexIndex[0]);
			vertexIndices.push_back(vertexIndex[1]);
			vertexIndices.push_back(vertexIndex[2]);
			uvIndices.push_back(uvIndex[0]);
			uvIndices.push_back(uvIndex[1]);
			uvIndices.push_back(uvIndex[2]);
			normalIndices.push_back(normalIndex[0]);
			normalIndices.push_back(normalIndex[1]);
			normalIndices.push_back(normalIndex[2]);
		}
	}

	numVertices_ = vertexIndices.size() * 3;
	vertices_ = newp Vertex[numVertices_];

	for (uint i = 0; i < vertexIndices.size(); i++)
	{

		uint uvIndex;
		glm::vec2 uv = glm::vec2(0, 0);

		if (uvs.size() > 0)
		{
			uvIndex = uvIndices[i];
			uv = uvs[uvIndex - 1];
		}

		uint vertexIndex = vertexIndices[i];
		uint normalIndex = normalIndices[i];

		glm::vec3 position = positions[vertexIndex - 1];
		glm::vec3 normal = normals[normalIndex - 1];

		Vertex vertex;
		vertex.p = glm::vec4(position, 1);
		vertex.uv = uv;
		vertex.normal = normal;
		vertex.c = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		
		vertices_[i] = vertex;
	}
}