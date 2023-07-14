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

struct Glyph {
	glm::vec2 size = { 0.0f, 0.0f };
	glm::vec2 offset = { 0.0f, 0.0f };
	float advance = 0.0f;
	uint16_t tex_id = 0;
	glm::vec4 tex_coord = { 0.0f, 0.0f, 0.0f, 0.0f };
};

class GlyphSet {
	std::map<wchar_t, Glyph> m_glyphes;
	std::map<wchar_t, Glyph>* m_symbols = nullptr;
	bool m_is_symbol = false;

public:
	GlyphSet(Texture* texture, const std::string& name, GlyphSet* symbol_set = nullptr);
	~GlyphSet() = default;

	const Glyph* getGlyph(wchar_t c);
	inline bool isSymbol() { return m_is_symbol; }
	inline std::map<wchar_t, Glyph>* getGlyphes() { return &m_glyphes; }
};

}