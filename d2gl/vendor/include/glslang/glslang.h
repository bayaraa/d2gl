#pragma once

#include <string>

namespace glslang {

enum class ShaderStage {
	Vertex,
	Fragment,
};

struct GLVersion {
	uint8_t major;
	uint8_t minor;
};

struct Result {
	bool result;
	std::string msg;
	std::string str;
};

Result getGLSLCode(
	ShaderStage stage,
	GLVersion version,
	const std::string& source,
	const std::string& id = "");

}