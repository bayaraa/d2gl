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

#pragma once

namespace d2gl {

struct UboVariable {
	std::string name;
	uint32_t size;
};

struct UboVariableData {
	uint32_t size;
	uint32_t offset;
	uint32_t hash = 0;
};

struct UniformBufferCreateInfo {
	std::vector<UboVariable> variables;
};

class UniformBuffer {
	GLuint m_id = 0;
	uint32_t m_size = 0;
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
	const uint32_t getHash(const std::string& name);

	inline const uint32_t& getBinding() const { return m_binding; }

private:
	bool checkVariable(const std::string& name);
};

}