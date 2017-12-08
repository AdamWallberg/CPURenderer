#pragma once

#include "Core.h"
#include <string>

enum
{
	LOG_DEFAULT = 0b0000'0000'0000'0001,
	LOG_IO = 0b0000'0000'0000'0010,
	LOG_RENDERING = 0b0000'0000'0000'0100,
	LOG_MEMORY = 0b0000'0000'0000'1000,
	LOG_GAME_OBJECT = 0b0000'0000'0001'0000,
	LOG_SYSTEM = 0b0000'0000'0010'0000,
};

class Logger
{
public:
	Logger();

	void log(const std::string& message, uint flag = LOG_DEFAULT);
	void logWarning(const std::string& message, uint flag = LOG_DEFAULT);
	void logError(const std::string& message, uint flag = LOG_DEFAULT);

	void setFlag(uint flag, bool value);
	void setFlagTrace(uint flag, bool value);
	void setFlagWarning(uint flag, bool value);
	void setFlagError(uint flag, bool value);

	void writeToFile(const std::string& filename);
private:
	void print(std::string message, uint level);

	uint traceFlags_;
	uint warningFlags_;
	uint errorFlags_;

	std::string log_;
};