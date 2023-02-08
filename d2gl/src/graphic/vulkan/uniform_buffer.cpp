#include "pch.h"
#include "context.h"
#include "utils.h"

namespace d2gl::vulkan {

UniformBuffer::UniformBuffer(const UniformBufferCreateInfo& info)
	: VULKAN_CONTEXT_CLASS
{
	uint32_t offset = 0;
	for (auto& variable : info.variables) {
		m_variable_data.insert({ variable.name, { variable.size, offset } });
		m_size += variable.size;
		offset = m_size;
	}

	auto const alignment = static_cast<uint32_t>(vkc->getPhysicalDevice().properties.limits.minUniformBufferOffsetAlignment);
	m_size = alignment * ((m_size + alignment - 1) / alignment);

	const auto usage = vk::BufferUsageFlagBits::eUniformBuffer | vk::BufferUsageFlagBits::eTransferDst;
	m_buffer = utils::createBuffer(static_cast<vk::DeviceSize>(m_size), usage, vk::MemoryPropertyFlagBits::eDeviceLocal, false);
}

UniformBuffer::~UniformBuffer()
{
	vkc->waitIdle();

	vkc->m_device.freeMemory(m_buffer.memory);
	vkc->m_device.destroyBuffer(m_buffer.handle);
}

void UniformBuffer::updateData1f(const std::string& name, const float& value)
{
	updateData(name, (const void*)&value);
}

void UniformBuffer::updateDataVec2f(const std::string& name, const glm::vec2& value)
{
	updateData(name, (const void*)&value);
}

void UniformBuffer::updateDataVec4f(const std::string& name, const glm::vec4& value)
{
	updateData(name, (const void*)&value);
}

void UniformBuffer::updateDataMat4f(const std::string& name, const glm::mat4& value)
{
	updateData(name, (const void*)&value);
}

void UniformBuffer::updateData(const std::string& name, const void* data)
{
	if (!checkVariable(name))
		return;

	vk::DeviceSize offset = static_cast<vk::DeviceSize>(m_variable_data[name].offset);
	vk::DeviceSize size = static_cast<vk::DeviceSize>(m_variable_data[name].size);

	const auto command_buffer = vkc->beginCommandBuffer();
	command_buffer.updateBuffer(m_buffer.handle, offset, size, data);
	vkc->submitCommandBuffer();
}

void UniformBuffer::updateHash(const std::string& name, uint32_t hash)
{
	if (checkVariable(name))
		m_variable_data[name].hash = hash;
}

uint32_t UniformBuffer::getHash(const std::string& name)
{
	if (checkVariable(name))
		return m_variable_data[name].hash;
	return 0;
}

vk::DescriptorBufferInfo UniformBuffer::descriptorBufferInfo(uint32_t offset)
{
	return vk::DescriptorBufferInfo().setOffset(static_cast<vk::DeviceSize>(offset)).setRange(static_cast<vk::DeviceSize>(m_size)).setBuffer(m_buffer.handle);
}

bool UniformBuffer::checkVariable(const std::string& name)
{
	if (m_variable_data.find(name) == m_variable_data.end()) {
		error("Uniform variable (%s) not found!", name.c_str());
		return false;
	}
	return true;
}

}