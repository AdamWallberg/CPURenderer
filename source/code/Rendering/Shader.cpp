#include "Shader.h"
#include "GL.h"
#include <fstream>
#include <vector>
#include "Engine.h"

Shader::Shader(const std::string& shader)
	: m_id(0)
{
	LOGGER->log("Creating shader: " + shader, LOG_RENDERING);

	std::string path = "data/shaders/";
	std::string vertexName = shader + "_v.glsl";
	std::string fragmentName = shader + "_f.glsl";

	uint vertexShaderID = loadShaderFromFile(path.c_str(), vertexName.c_str(), GL_VERTEX_SHADER);
	uint fragmentShaderID = loadShaderFromFile(path.c_str(), fragmentName.c_str(), GL_FRAGMENT_SHADER);

	//Create and link the program
	m_id = glCreateProgram();
	glAttachShader(m_id, vertexShaderID);
	glAttachShader(m_id, fragmentShaderID);
	glLinkProgram(m_id);

	// Check for errors
	int result = false;
	int infoLogLength;
	glGetProgramiv(m_id, GL_LINK_STATUS, &result);
	glGetProgramiv(m_id, GL_INFO_LOG_LENGTH, &infoLogLength);
	if (infoLogLength > 0)
	{
		std::vector<char> errorMessage(infoLogLength + 1);
		glGetProgramInfoLog(m_id, infoLogLength, nullptr, &errorMessage[0]);
		LOGGER->logError(&errorMessage[0], LOG_RENDERING);
	}

	// Detach and delete shaders
	glDetachShader(m_id, vertexShaderID);
	glDetachShader(m_id, fragmentShaderID);
	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);
}

Shader::~Shader()
{
	glDeleteProgram(m_id);
}

uint Shader::loadShaderFromFile(const char* directory, const char* fileName, int shaderType)
{
	// Read code
	std::string code;
	std::string filePath = directory;
	filePath += fileName;
	std::ifstream stream(filePath, std::ios::in);
	if (!stream.is_open())
	{
		LOGGER->logError("Failed to open shader: " + filePath, LOG_RENDERING);
		return 0;
	}
	else
	{
		std::string line = "";
		while (getline(stream, line))
			code += "\n" + line;
		stream.close();
	}

	// Create shader
	uint ID = glCreateShader(shaderType);
	int result = false;
	int infoLogLength;

	// Compile shader
	const char* sourcePointer = code.c_str();
	glShaderSource(ID, 1, &sourcePointer, nullptr);
	glCompileShader(ID);

	// Check for errors
	glGetShaderiv(ID, GL_COMPILE_STATUS, &result);
	glGetShaderiv(ID, GL_INFO_LOG_LENGTH, &infoLogLength);
	if (infoLogLength > 0)
	{
		std::vector<char> errorMessage(infoLogLength + 1);
		glGetShaderInfoLog(ID, infoLogLength, nullptr, &errorMessage[0]);
		LOGGER->logError(std::string(fileName) + ": " + &errorMessage[0], LOG_RENDERING);
	}

	return ID;
}

void Shader::bind()
{
	glUseProgram(m_id);
}

void Shader::setBool(const char* name, bool value) const
{
	glUniform1i(glGetUniformLocation(m_id, name), (int)value);
}

void Shader::setInt(const char* name, int value) const
{
	glUniform1i(glGetUniformLocation(m_id, name), value);
}

void Shader::setFloat(const char* name, float value) const
{
	glUniform1f(glGetUniformLocation(m_id, name), value);
}

void Shader::setVec2(const char* name, glm::vec2 value) const
{
	glUniform2f(glGetUniformLocation(m_id, name), value.x, value.y);
}

void Shader::setVec3(const char* name, glm::vec3 value) const
{
	glUniform3f(glGetUniformLocation(m_id, name), value.x, value.y, value.z);
}

void Shader::setVec4(const char* name, glm::vec4 value) const
{
	glUniform4f(glGetUniformLocation(m_id, name), value.x, value.y, value.z, value.w);
}

void Shader::setMat4(const char* name, glm::mat4 value) const
{
	glUniformMatrix4fv(glGetUniformLocation(m_id, name), 1, GL_FALSE, &value[0][0]);
}