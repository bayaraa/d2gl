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
#include "font.h"
#include "helpers.h"

namespace d2gl {

Font::Font(GlyphSet* glyph_set, const FontCreateInfo& font_ci)
	: m_glyph_set(glyph_set), m_name(font_ci.name), m_size(font_ci.size), m_weight(font_ci.weight), m_letter_spacing(font_ci.letter_spacing), m_line_height(font_ci.line_height),
	  m_shadow_intensity(font_ci.shadow_intensity), m_offset(font_ci.offset), m_symbol_offset(font_ci.symbol_offset), m_color(font_ci.color), m_bordered(font_ci.bordered)
{
	setSize();
	m_object = std::make_unique<Object>();
}

glm::vec2 Font::getTextSize(const wchar_t* str, const int max_chars)
{
	m_text_size = { 0.0f, 0.0f };
	const float line_height = getLineHeight();
	const float letter_spacing = getLetterSpacing();

	float advance = 0.0f;
	int char_num = 0;
	int line_num = 0;

	while (str[char_num] != L'\0') {
		if (str[char_num] == L'ÿ' && str[char_num + 1] == L'c') {
			const auto color_code = str[char_num + 2];
			if (g_text_colors.find(color_code) != g_text_colors.end()) {
				char_num += 3;
				continue;
			}
		}
		if (str[char_num] == L'\n') {
			m_line_width[line_num] = advance;
			m_text_size.x = glm::max(m_text_size.x, advance);
			if (str[char_num + 1] != L'\0') {
				m_text_size.y += line_height;
				advance = 0.0f;
				line_num++;
			}
		} else if (auto glyph = m_glyph_set->getGlyph(str[char_num])) {
			advance += glyph->advance * m_scale;
			if (str[char_num + 1] != L'\n' && str[char_num + 1] != L'\0')
				advance += letter_spacing;
		}
		if (max_chars > 0 && max_chars < char_num)
			break;
		char_num++;
	}

	m_line_count = line_num + 1;
	m_line_width[line_num] = advance;
	m_text_size.x = glm::max(m_text_size.x, advance);
	m_text_size.y += line_height - (line_height - m_font_size);

	return m_text_size;
}

void Font::drawText(const wchar_t* str, glm::vec2 pos, uint32_t color, bool framed)
{
	const auto line_height = getLineHeight();
	const auto letter_spacing = getLetterSpacing();
	auto text_offset = getTextOffset();

	glm::vec2 offset = text_offset;
	if (framed) {
		offset.x = text_offset.x = 0.0f;
		offset.y += (float)m_line_count * line_height - line_height;
	} else
		offset.y += (m_font_size - m_size) / 2.0f;

	if (m_align == TextAlign::Right)
		offset.x += m_text_size.x - m_line_width[0];
	else if (m_align == TextAlign::Center)
		offset.x += (m_text_size.x - m_line_width[0]) / 2.0f;

	uint32_t char_color = color;
	if (m_bordered) {
		uint32_t border_color = 0x443B29FF;
		if (color == 0xC22121FF)
			border_color = 0x4A1515FF;
		else if (color == 0xDFB67966)
			border_color = 0x443B2966;
		m_object->setColor(border_color, 2);
	} else {
		uint32_t color2 = 0xFFFFFFFF;
		uint8_t* opacity = (uint8_t*)&color2;
		*opacity = (uint8_t)(255.0f * m_opacity);
		m_object->setColor(color2, 2);
	}

	int char_num = 0;
	int line_num = 0;
	while (str[char_num] != L'\0') {
		if (str[char_num] == L'ÿ' && str[char_num + 1] == L'c') {
			const auto color_code = str[char_num + 2];
			if (g_text_colors.find(color_code) != g_text_colors.end()) {
				char_color = g_text_colors.at(color_code);
				char_num += 3;
				continue;
			}
		}
		if (str[char_num] == L'\n') {
			offset.x = text_offset.x;
			offset.y -= line_height;

			if (str[char_num + 1] == L'\0')
				break;
			line_num++;

			if (m_align == TextAlign::Right)
				offset.x += m_text_size.x - m_line_width[line_num];
			else if (m_align == TextAlign::Center)
				offset.x += (m_text_size.x - m_line_width[line_num]) / 2.0f;
		} else
			offset.x += drawChar(str[char_num], pos + offset, char_color) + letter_spacing;
		char_num++;
	}
	m_line_count = 0;
}

float Font::drawChar(wchar_t c, glm::vec2 pos, uint32_t color)
{
	if (auto glyph = m_glyph_set->getGlyph(c)) {
		if (c == L' ')
			return glyph->advance * m_scale;

		glm::vec2 object_pos = pos + glyph->offset * m_scale;
		float weight = m_weight;
		if (m_glyph_set->isSymbol()) {
			object_pos.y += m_font_size * m_symbol_offset;
			weight = 1.0f + ((m_weight - 1.0f) * 0.5f);
		}

		m_object->setSize(glyph->size * m_scale);
		m_object->setTexIds({ glyph->tex_id, 0 });
		m_object->setTexCoord(glyph->tex_coord);
		m_object->setPosition(object_pos);
		m_object->setColor(color);

		if (m_shadow_level > 0) {
			m_object->setExtra({ m_shadow_intensity, 0.0f });
			m_object->setFlags(3, m_shadow_level, m_masking);
			App.context->pushObject(m_object);
		}
		m_object->setExtra({ m_smoothness, weight });
		m_object->setFlags(3, 0, m_masking, m_bordered);
		App.context->pushObject(m_object);

		return glyph->advance * m_scale;
	}

	return 0.0f;
}

#ifdef _HDTEXT
void Font::updateMetrics()
{
	m_size = App.hdt.size.value;
	m_weight = App.hdt.weight.value;
	m_letter_spacing = App.hdt.letter_spacing.value;
	m_line_height = App.hdt.line_height.value;
	m_shadow_intensity = App.hdt.shadow_intensity.value;
	m_offset.x = App.hdt.offset_x.value;
	m_offset.y = App.hdt.offset_y.value;
	m_symbol_offset = App.hdt.symbol_offset.value;
	setSize();
}

void Font::getMetrics()
{
	App.hdt.size.value = m_size;
	App.hdt.weight.value = m_weight;
	App.hdt.letter_spacing.value = m_letter_spacing;
	App.hdt.line_height.value = m_line_height;
	App.hdt.shadow_intensity.value = m_shadow_intensity;
	App.hdt.offset_x.value = m_offset.x;
	App.hdt.offset_y.value = m_offset.y;
	App.hdt.symbol_offset.value = m_symbol_offset;
}

char* Font::getMetricsString(uint8_t id)
{
	static char str[120] = "";
	sprintf_s(str, "%d | %s | %2.3f | %.3f | %.3f | %.3f | %.3f | %2.3f | %2.3f | %2.3f", id, m_name.c_str(), m_size, m_weight, m_letter_spacing, m_line_height, m_shadow_intensity, m_offset.x, m_offset.y, m_symbol_offset);
	return str;
}
#endif
}