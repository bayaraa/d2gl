#pragma once

#include <string>
#include <unordered_map>
#include <vector>

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
	bool result = false;
	std::string source;
	std::string err_msg;
	std::vector<std::string> samplers;
	std::unordered_map<std::string, std::string> uniforms;
};

Result getGLSLCode(
	ShaderStage stage,
	GLVersion version,
	const std::string& source,
	const std::string& id = "");

}