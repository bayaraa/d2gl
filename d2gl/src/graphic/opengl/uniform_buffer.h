#pragma once

#include "graphic/uniform_buffer.h"

namespace d2gl::opengl {

class UniformBuffer final : public d2gl::UniformBuffer {
	GLuint m_id = 0;
	size_t m_size = 0;
	uint32_t m_binding = 0;
	std::unordered_map<std::string, UboVariableData> m_variable_data;

public:
	UniformBuffer(const UniformBufferCreateInfo& info);
	~UniformBuffer();

	void updateData1f(const std::string& name, const float& value);
	void updateDataVec2f(const std::string& name, const glm::vec2& value);
	void updateDataVec4f(const std::string& name, const glm::vec4& value);
	void updateDataMat4f(const std::string& name, const glm::mat4& value);
	void updateData(const std::string& name, const void* data);
	void updateHash(const std::string& name, uint32_t hash);
	uint32_t getHash(const std::string& name);

public:
	inline const uint32_t& getBinding() const { return m_binding; }

private:
	bool checkVariable(const std::string& name);
};

}