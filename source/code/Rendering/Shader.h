#pragma once

#include "Core.h"
#include <string>
#include "glm/glm.hpp"

class Shader
{
public:
	Shader() {}
	Shader(const std::string& shader);
	~Shader();
	void bind();
	void setBool(const char* name, bool value) const;
	void setInt(const char* name, int value) const;
	void setFloat(const char* name, float value) const;
	void setVec2(const char* name, glm::vec2 value) const;
	void setVec3(const char* name, glm::vec3 value) const;
	void setVec4(const char* name, glm::vec4 value) const;
	void setMat4(const char* name, glm::mat4 value) const;

	uint getID() const { return m_id; }
private:
	uint loadShaderFromFile(const char* directory, const char* fileName, int shaderType);

	uint m_id;
};