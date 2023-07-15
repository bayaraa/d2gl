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
#include "glyph_set.h"
#include "helpers.h"

namespace d2gl {

GlyphSet::GlyphSet(Texture* texture, const std::string& name, GlyphSet* symbol_set)
	: m_symbols(symbol_set ? symbol_set->getGlyphes() : nullptr)
{
	auto buffer = helpers::loadFile("assets\\atlases\\" + name + "\\data.csv");
	if (!buffer.size)
		return;

	int atlas_index = -1;
	uint32_t start_layer = 0;
	std::string data((const char*)buffer.data, buffer.size);
	auto lines = helpers::strToLines(data);
	for (auto& line : lines) {
		auto cols = helpers::splitToVector(line);

		auto index = std::atoi(cols[0].c_str());
		if (atlas_index < index) {
			auto image_data = helpers::loadImage("assets\\atlases\\" + name + "\\" + cols[0] + ".png");
			auto tex_data = texture->fillImage(image_data);
			helpers::clearImage(image_data);
			start_layer = tex_data.start_layer;
			atlas_index = index;
		}

		wchar_t cc = (wchar_t)std::atoi(cols[1].c_str());
		glm::vec4 coords = { std::stof(cols[7]), std::stof(cols[8]), std::stof(cols[9]), std::stof(cols[10]) };
		glm::vec4 bounds = { std::stof(cols[3]), std::stof(cols[4]), std::stof(cols[5]), std::stof(cols[6]) };

		m_glyphes[cc].advance = std::stof(cols[2].c_str()) * 32.0f;
		m_glyphes[cc].size = { coords.z - coords.x, coords.w - coords.y };
		m_glyphes[cc].offset = { bounds.x * 32.0f, -bounds.w * 32.0f };
		m_glyphes[cc].tex_id = start_layer;
		m_glyphes[cc].tex_coord = coords / 1024.0f;
	}

	delete[] buffer.data;
}

const Glyph* GlyphSet::getGlyph(wchar_t c)
{
	m_is_symbol = false;
	if (m_glyphes.find(c) != m_glyphes.end())
		return &m_glyphes[c];

	if (c == L'\xa0')
		return &m_glyphes[L' '];

	if (m_symbols) {
		m_is_symbol = true;
		if (m_symbols->find(c) != m_symbols->end())
			return &m_symbols->at(c);

		if (c > 0x20)
			return &m_symbols->at(L'☹');
	}

	if (c > 0x20)
		return &m_glyphes[L'?'];

	return nullptr;
}

}