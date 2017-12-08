#pragma once

#include <glm/glm.hpp>

struct Circle
{
	glm::vec2 pos;
	float radius;
};

struct Rect
{
	glm::vec2 pos;
	glm::vec2 size;
};

struct Triangle
{
	glm::vec3 p0;
	glm::vec3 p1;
	glm::vec3 p2;

	bool pointIntersects(glm::vec2 p)
	{
		bool b1, b2, b3;
		b1 = s(p, p0, p1) < 0.0f;
		b2 = s(p, p1, p2) < 0.0f;
		b3 = s(p, p2, p0) < 0.0f;
		return ((b1 == b2) && (b2 == b3));
	}

	float area()
	{
		float area = ((p1.x - p0.x) * (p2.y - p0.y) - (p2.x - p0.x) * (p1.y - p0.y)) * 0.5f;
		return glm::abs(area);
	}

private:
	float s(glm::vec2 p1, glm::vec2 p2, glm::vec2 p3)
	{
		return (p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y);
	}
};

struct Vertex
{
	glm::vec4 p;
	glm::vec4 c;
};

struct VertexTriangle
{
	Vertex v[3];

	bool pointIntersects(glm::vec2 p)
	{
		bool b1, b2, b3;
		b1 = s(p, v[0].p, v[1].p) < 0.0f;
		b2 = s(p, v[1].p, v[2].p) < 0.0f;
		b3 = s(p, v[2].p, v[0].p) < 0.0f;
		return ((b1 == b2) && (b2 == b3));
	}

	Vertex getAt(glm::vec3 p)
	{
		Triangle t0 = { p, v[1].p, v[2].p };
		Triangle t1 = { p, v[0].p, v[2].p };
		Triangle t2 = { p, v[1].p, v[0].p };

		float a0 = t0.area();
		float a1 = t1.area();
		float a2 = t2.area();
		float ta = a0 + a1 + a2;

		float f0 = a0 / ta;
		float f1 = a1 / ta;
		float f2 = a2 / ta;

		Vertex r;
		r.p = v[0].p * f0 + v[1].p * f1 + v[2].p * f2;
		r.c = v[0].c * f0 + v[1].c * f1 + v[2].c * f2;
		return r;
	}

private:
	float s(glm::vec2 p1, glm::vec2 p2, glm::vec2 p3)
	{
		return (p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y);
	}
};