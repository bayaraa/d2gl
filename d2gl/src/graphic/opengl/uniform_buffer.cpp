#include "pch.h"
#include "context.h"
#include "utils.h"

namespace d2gl::opengl {

uint32_t g_ubo_bindings = 0;

UniformBuffer::UniformBuffer(const UniformBufferCreateInfo& info)
{
	uint32_t offset = 0;
	for (auto& variable : info.variables) {
		m_variable_data.insert({ variable.name, { variable.size, offset } });
		m_size += variable.size;
		offset = m_size;
	}

	glGenBuffers(1, &m_id);

	glBindBuffer(GL_UNIFORM_BUFFER, m_id);
	glBufferData(GL_UNIFORM_BUFFER, m_size, NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	m_binding = g_ubo_bindings;
	glBindBufferRange(GL_UNIFORM_BUFFER, m_binding, m_id, 0, m_size);
	g_ubo_bindings++;
}

UniformBuffer::~UniformBuffer()
{
	glDeleteBuffers(1, &m_id);
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

	const uintptr_t offset = m_variable_data[name].offset;
	const size_t size = m_variable_data[name].size;

	glBindBuffer(GL_UNIFORM_BUFFER, m_id);
	glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
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

bool UniformBuffer::checkVariable(const std::string& name)
{
	if (m_variable_data.find(name) == m_variable_data.end()) {
		error("Uniform variable (%s) not found!", name.c_str());
		return false;
	}
	return true;
}

}