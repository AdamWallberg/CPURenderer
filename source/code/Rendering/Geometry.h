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

	inline bool pointIntersects(const glm::vec2& p) const
	{
		bool b1, b2, b3;
		b1 = s(p, p0, p1) < 0.0f;
		b2 = s(p, p1, p2) < 0.0f;
		b3 = s(p, p2, p0) < 0.0f;
		return ((b1 == b2) && (b2 == b3));
	}

	inline float area() const
	{
		return glm::abs(p0.x * (p1.y - p2.y) + p1.x * (p2.y - p0.y) + p2.x * (p0.y - p1.y)) * 0.5f;
	}

private:
	inline float s(const glm::vec2& p1, const glm::vec2& p2, const glm::vec2& p3) const
	{
		return (p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y);
	}
};

struct Vertex
{
	glm::vec4 p;
	glm::vec4 c;
	glm::vec2 uv;
	glm::vec3 normal;
};

struct VertexTriangle
{
	Vertex v[3];

	inline bool pointIntersects(glm::vec2 p) const
	{
		bool b1, b2, b3;
		b1 = s(p, v[0].p, v[1].p) < 0.0f;
		b2 = s(p, v[1].p, v[2].p) < 0.0f;
		b3 = s(p, v[2].p, v[0].p) < 0.0f;
		return ((b1 == b2) && (b2 == b3));
	}

	inline Vertex getAt(const glm::vec2& p) const
	{
		const Triangle t0 = { glm::vec3(p, 0), v[2].p, v[1].p };
		const Triangle t1 = { glm::vec3(p, 0), v[0].p, v[2].p };
		const Triangle t2 = { glm::vec3(p, 0), v[1].p, v[0].p };

		const float a0 = t0.area() * v[1].p.w * v[2].p.w;
		const float a1 = t1.area() * v[0].p.w * v[2].p.w;
		const float a2 = t2.area() * v[1].p.w * v[0].p.w;

		const float ta = a0 + a1 + a2;

		const float f0 = a0 / ta;
		const float f1 = a1 / ta;
		const float f2 = a2 / ta;

		Vertex r;
		r.p = v[0].p * f0 + v[1].p * f1 + v[2].p * f2;
		r.c = v[0].c * f0 + v[1].c * f1 + v[2].c * f2;
		r.uv = v[0].uv * f0 + v[1].uv * f1 + v[2].uv * f2;
		r.normal = v[0].normal * f0 + v[1].normal * f1 + v[2].normal * f2;
		return r;
	}

	inline glm::vec3 normal() const
	{
		return glm::normalize(glm::cross(
			glm::vec3(v[1].p - v[0].p),
			glm::vec3(v[2].p - v[0].p)));
	}

private:
	float s(const glm::vec2& p1, const glm::vec2& p2, const glm::vec2& p3) const
	{
		return (p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y);
	}
};