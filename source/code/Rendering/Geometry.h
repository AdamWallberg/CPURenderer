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
		return glm::abs(p0.x * (p1.y - p2.y) + p1.x * (p2.y - p0.y) + p2.x * (p0.y - p1.y)) * 0.5f;
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
	glm::vec2 uv;
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

	Vertex getAt(glm::vec2 p)
	{
		Triangle t0 = { glm::vec4(p, 0, 1), v[2].p, v[1].p };
		Triangle t1 = { glm::vec4(p, 0, 1), v[0].p, v[2].p };
		Triangle t2 = { glm::vec4(p, 0, 1), v[1].p, v[0].p };

		float a0 = t0.area() / v[0].p.w;
		float a1 = t1.area() / v[1].p.w;
		float a2 = t2.area() / v[2].p.w;
		float ta = a0 + a1 + a2;

		float f0 = a0 / ta;
		float f1 = a1 / ta;
		float f2 = a2 / ta;

		Vertex r;
		r.p = v[0].p * f0 + v[1].p * f1 + v[2].p * f2;
		r.c = v[0].c * f0 + v[1].c * f1 + v[2].c * f2;
		r.uv = v[0].uv * f0 + v[1].uv * f1 + v[2].uv * f2;
		return r;
	}

	glm::vec3 normal()
	{
		return glm::normalize(glm::cross(
			glm::normalize(glm::vec3(v[1].p - v[0].p)),
			glm::normalize(glm::vec3(v[2].p - v[0].p))));
	}

private:
	float s(glm::vec2 p1, glm::vec2 p2, glm::vec2 p3)
	{
		return (p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y);
	}
};