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

#include "glyph_set.h"
#include "variables.h"

namespace d2gl {

struct FontCreateInfo {
	std::string name;
	float size = 0.0f;
	float weight = 0.0f;
	float letter_spacing = 0.0f;
	float line_height = 0.0f;
	float shadow_intensity = 0.0f;
	glm::vec2 offset = { 0.0f, 0.0f };
	float symbol_offset = 0.0f;
	wchar_t color = 0;
	bool bordered = false;
};

class Font {
	std::unique_ptr<Object> m_object;
	GlyphSet* m_glyph_set = nullptr;
	std::string m_name;
	float m_scale = 1.0f;
	wchar_t m_color = 0;
	bool m_bordered = false;

	float m_size = 10.0f;
	float m_font_size = 10.0f;
	float m_weight = 1.0f;
	float m_line_height = 1.0f;
	float m_letter_spacing = 0.0f;
	float m_shadow_intensity = 0.9f;
	glm::vec2 m_offset = { 0.0f, 0.0f };
	float m_symbol_offset = 0.0f;

	float m_smoothness = 5.0f;
	uint8_t m_shadow_level = 0;

	TextAlign m_align = TextAlign::Left;
	float m_line_width[100] = { 0.0f };
	glm::vec2 m_text_size = { 0.0f, 0.0f };

	uint32_t m_line_count = 0;
	bool m_masking = false;

public:
	Font(GlyphSet* glyph_set, const FontCreateInfo& font_ci);
	~Font() = default;

	inline void setSize() { m_font_size = m_size * App.hd_text.scale.value, m_scale = m_font_size / 32.0f, m_smoothness = m_font_size; }
	inline void setAlign(TextAlign align) { m_align = align; }
	inline void setShadow(uint8_t level = 0) { m_shadow_level = level; }
	inline void setMasking(bool masking) { m_masking = masking; }

	inline wchar_t getColor() { return m_color; }
	inline float getFontSize() { return m_font_size; }
	inline float getWeight() { return m_weight; }
	inline float getLineHeight() { return m_font_size * m_line_height; }
	inline float getLetterSpacing() { return m_font_size * m_letter_spacing; }
	inline glm::vec2 getTextOffset() { return m_font_size * m_offset; }
	inline int getLineCount() { return m_line_count; }

	glm::vec2 getTextSize(const wchar_t* str, const int max_chars = 0);
	void drawText(const wchar_t* str, glm::vec2 pos, uint32_t color, bool framed = false);

private:
	float drawChar(wchar_t c, glm::vec2 pos, uint32_t color);

#ifdef _HDTEXT
public:
	void updateMetrics();
	void getMetrics();
	char* getMetricsString(uint8_t id);
#endif
};

}