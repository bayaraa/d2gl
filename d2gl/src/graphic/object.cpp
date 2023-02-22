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
#include "object.h"

namespace d2gl {

Object::Object(glm::vec2 position, glm::vec2 size)
	: m_size(size)
{
	for (size_t i = 0; i < 4; i++) {
		m_vertices[i].color1 = 0xFFFFFFFF;
		m_vertices[i].color2 = 0xFFFFFFFF;
		m_vertices[i].texture_ids = { 0, 0 };
		m_vertices[i].flags = { 0, 0, 0, 0 };
	}
	setPosition(position);
	setTexCoord({ 0.0f, 0.0f, 1.0f, 1.0f });
}

void Object::setPosition(glm::vec2 position)
{
	m_vertices[0].position = { position.x, position.y };
	m_vertices[1].position = { position.x + m_size.x, position.y };
	m_vertices[2].position = { position.x + m_size.x, position.y + m_size.y };
	m_vertices[3].position = { position.x, position.y + m_size.y };
}

void Object::setSize(glm::vec2 size)
{
	m_size = size;
	m_vertices[1].position.x = m_vertices[0].position.x + m_size.x;
	m_vertices[2].position = { m_vertices[0].position.x + m_size.x, m_vertices[0].position.y + m_size.y };
	m_vertices[3].position.y = m_vertices[0].position.y + m_size.y;
}

void Object::setTexCoord(glm::vec4 tex_coord)
{
	m_vertices[0].tex_coord = { tex_coord.x, tex_coord.w };
	m_vertices[1].tex_coord = { tex_coord.z, tex_coord.w };
	m_vertices[2].tex_coord = { tex_coord.z, tex_coord.y };
	m_vertices[3].tex_coord = { tex_coord.x, tex_coord.y };
}

void Object::setTexIds(glm::vec<2, int16_t> tex_ids)
{
	m_vertices[0].texture_ids = tex_ids;
	m_vertices[1].texture_ids = tex_ids;
	m_vertices[2].texture_ids = tex_ids;
	m_vertices[3].texture_ids = tex_ids;
}

void Object::setColor(uint32_t color, int num)
{
	if (num == 1) {
		m_vertices[0].color1 = color;
		m_vertices[1].color1 = color;
		m_vertices[2].color1 = color;
		m_vertices[3].color1 = color;
	} else {
		m_vertices[0].color2 = color;
		m_vertices[1].color2 = color;
		m_vertices[2].color2 = color;
		m_vertices[3].color2 = color;
	}
}

void Object::setFlags(glm::vec<4, int8_t> flags)
{
	m_vertices[0].flags = flags;
	m_vertices[1].flags = flags;
	m_vertices[2].flags = flags;
	m_vertices[3].flags = flags;
}

void Object::setExtra(glm::vec2 extra)
{
	m_vertices[0].extra = extra;
	m_vertices[1].extra = extra;
	m_vertices[2].extra = extra;
	m_vertices[3].extra = extra;
}

}