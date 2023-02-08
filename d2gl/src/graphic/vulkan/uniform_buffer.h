#pragma once

#include "buffer.h"
#include "graphic/uniform_buffer.h"

namespace d2gl::vulkan {

class Context;

class UniformBuffer final : public d2gl::UniformBuffer {
	Context* vkc;
	Buffer m_buffer;
	size_t m_size = 0;
	std::unordered_map<std::string, UboVariableData> m_variable_data;

	friend class Pipeline;

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

private:
	vk::DescriptorBufferInfo descriptorBufferInfo(uint32_t offset = 0);
	bool checkVariable(const std::string& name);
};

}