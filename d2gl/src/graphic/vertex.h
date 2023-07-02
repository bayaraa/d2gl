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

struct Vertex {
	glm::vec<2, int16_t> position;
	glm::vec2 tex_coord;
	uint32_t color1;
	uint32_t color2;
	glm::vec<2, uint16_t> tex_ids;
	glm::vec<4, uint8_t> flags;

	static void enableAttribArray()
	{
		for (uint32_t i = 0; i <= 6; i++)
			glEnableVertexAttribArray(i);
	}

	static void bindingDescription()
	{
		glDisableVertexAttribArray(6);
		glVertexAttribPointer(0, 2, GL_HALF_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, position));
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, tex_coord));
		glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), (const void*)offsetof(Vertex, color1));
		glVertexAttribPointer(3, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), (const void*)offsetof(Vertex, color2));
		glVertexAttribIPointer(4, 2, GL_UNSIGNED_SHORT, sizeof(Vertex), (const void*)offsetof(Vertex, tex_ids));
		glVertexAttribIPointer(5, 4, GL_UNSIGNED_BYTE, sizeof(Vertex), (const void*)offsetof(Vertex, flags));
	}
};

struct VertexMod {
	glm::vec<2, int16_t> position;
	glm::vec2 tex_coord;
	uint32_t color1;
	uint32_t color2;
	glm::vec<2, uint16_t> tex_ids;
	glm::vec<4, uint8_t> flags;
	glm::vec<2, int16_t> extra;

	static void bindingDescription()
	{
		glEnableVertexAttribArray(6);
		glVertexAttribPointer(0, 2, GL_HALF_FLOAT, GL_FALSE, sizeof(VertexMod), (const void*)offsetof(VertexMod, position));
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(VertexMod), (const void*)offsetof(VertexMod, tex_coord));
		glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(VertexMod), (const void*)offsetof(VertexMod, color1));
		glVertexAttribPointer(3, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(VertexMod), (const void*)offsetof(VertexMod, color2));
		glVertexAttribIPointer(4, 2, GL_UNSIGNED_SHORT, sizeof(VertexMod), (const void*)offsetof(VertexMod, tex_ids));
		glVertexAttribIPointer(5, 4, GL_UNSIGNED_BYTE, sizeof(VertexMod), (const void*)offsetof(VertexMod, flags));
		glVertexAttribPointer(6, 2, GL_HALF_FLOAT, GL_FALSE, sizeof(VertexMod), (const void*)offsetof(VertexMod, extra));
	}
};

struct GlideVertex {
	float x, y;
	uint32_t pargb;
	float scale;
	float s, t;
	uint32_t unused;
};

}