/*
	D2GL: Diablo 2 LoD Glide/DDraw to OpenGL Wrapper.
	Copyright (C) 2023  Bayaraa

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "pch.h"
#include "uniform_buffer.h"

namespace d2gl {

UniformBuffer::UniformBuffer(const UniformBufferCreateInfo& info)
{
	static uint32_t ubo_bindings = 0;

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

	m_binding = ubo_bindings;
	glBindBufferRange(GL_UNIFORM_BUFFER, m_binding, m_id, 0, m_size);
	ubo_bindings++;
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

const uint32_t UniformBuffer::getHash(const std::string& name)
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