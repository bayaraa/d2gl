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

#include "types.h"
#include "vertex.h"

namespace d2gl {

class Object {
	VertexMod m_vertices[4];
	glm::vec2 m_position;
	glm::vec2 m_size;

public:
	Object(glm::vec2 position = { 0.0f, 0.0f }, glm::vec2 size = { 0.0f, 0.0f });
	~Object() = default;

	void setPosition(glm::vec2 position);
	void setSize(glm::vec2 size);
	void setTexCoord(glm::vec4 tex_coord);
	void setTexIds(int16_t tex_num);
	void setColor(uint32_t color, int num = 1);
	void setFlags(uint8_t x = 0, uint8_t y = 0, uint8_t z = 0, uint8_t w = 0);
	void setExtra(glm::vec2 extra);

	inline const VertexMod* getVertices() { return m_vertices; };
};

}