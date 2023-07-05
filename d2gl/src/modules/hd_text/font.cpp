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

Font::Font(const FontCreateInfo& info)
	: m_glyph_size(info.glyph_size), m_rtrim_size(info.rtrim_size), m_offset(info.offset)
{
	TextureCreateInfo texture_ci;
	texture_ci.layer_count = 20;
	texture_ci.size = { 1024, 1024 };
	texture_ci.slot = TEXTURE_SLOT_FONTS;
	texture_ci.min_filter = GL_LINEAR;
	texture_ci.mag_filter = GL_LINEAR;
	static std::unique_ptr<Texture> texture = Context::createTexture(texture_ci);

	for (uint32_t index = 0; index < info.atlas_count; index++) {
		char atlas_index[2] = { 0 };
		sprintf_s(atlas_index, "%d", index);

		auto data_buffer = helpers::loadFile("assets\\fonts\\" + info.path + "\\data_" + atlas_index + ".csv");
		if (!data_buffer.size)
			continue;

		auto image_data = helpers::loadImage("assets\\fonts\\" + info.path + "\\atlas_" + atlas_index + ".png");
		auto tex_data = texture->fillImage(image_data);
		helpers::clearImage(image_data);

		std::string csv_data((const char*)data_buffer.data, data_buffer.size);
		auto lines = helpers::strToLines(csv_data);

		for (auto& line : lines) {
			uint32_t col_index = 0;
			std::vector<std::string> cols = { "" };
			for (auto& c : line) {
				if (c == ',') {
					cols.push_back("");
					col_index++;
				} else
					cols[col_index].push_back(c);
			}

			wchar_t cc = (wchar_t)std::atoi(cols[0].c_str());
			glm::vec4 coords = { std::stof(cols[6].c_str()), std::stof(cols[7].c_str()), std::stof(cols[8].c_str()), std::stof(cols[9].c_str()) };
			glm::vec4 bounds = { std::stof(cols[2].c_str()), std::stof(cols[3].c_str()), std::stof(cols[4].c_str()), std::stof(cols[5].c_str()) };

			m_glyphes[cc].advance = std::stof(cols[1].c_str()) * m_glyph_size;
			m_glyphes[cc].size = { coords.z - coords.x, coords.w - coords.y };
			m_glyphes[cc].offset = { bounds.x * m_glyph_size, -bounds.w * m_glyph_size };
			m_glyphes[cc].tex_id = tex_data.start_layer;
			m_glyphes[cc].tex_coord = coords / 1024.0f;
		}
	}

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
			m_line_width[line_num] = advance - letter_spacing;
			m_text_size.x = m_text_size.x < m_line_width[line_num] ? m_line_width[line_num] : m_text_size.x;
			m_text_size.y += line_height;
			advance = 0.0f;

			if (str[char_num + 1] == L'\0')
				break;
			line_num++;
		} else if (m_glyphes.find(str[char_num]) != m_glyphes.end()) {
			const Glyph* glyph = &m_glyphes[str[char_num]];
			advance += glyph->advance * m_scale + letter_spacing;
		}
		if (max_chars > 0 && max_chars < char_num)
			break;
		char_num++;
	}

	m_line_count = line_num;
	m_line_width[line_num] = advance - letter_spacing;
	m_text_size.x = m_text_size.x < m_line_width[line_num] ? m_line_width[line_num] : m_text_size.x;
	m_text_size.y = m_text_size.x > 0.0f ? m_text_size.y + line_height : m_text_size.y;

	return m_text_size;
}

void Font::drawText(const wchar_t* str, glm::vec2 pos, uint32_t color)
{
	uint32_t char_color = color;
	uint32_t border_color = 0x443B29FF;

	if (color == 0xC22121FF)
		border_color = 0x4A1515FF;
	else if (color == 0xDFB67966)
		border_color = 0x443B2966;

	m_object->setColor(border_color, 2);
	m_object->setExtra({ m_smoothness * (m_glyph_size < 33.0f ? 1.8f : 1.0f), m_weight });

	const float line_height = getLineHeight();
	const float letter_spacing = getLetterSpacing();
	const auto padding = getTextOffset();
	const float paddind_x = m_boxed ? m_rtrim_size * m_size : padding.x;

	glm::vec2 offset = { paddind_x, padding.y + (float)m_line_count * line_height };
	if (m_align == TextAlign::Right)
		offset.x += m_text_size.x - m_line_width[0];
	else if (m_align == TextAlign::Center)
		offset.x += (m_text_size.x - m_line_width[0]) / 2.0f;

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
			offset.x = paddind_x;
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
	if (m_glyphes.find(c) != m_glyphes.end()) {
		const Glyph* glyph = &m_glyphes[c];
		if (c == L' ')
			return glyph->advance * m_scale;

		glm::vec2 object_pos = pos + glyph->offset * m_scale;
		m_object->setSize(glyph->size * m_scale);
		m_object->setTexIds({ glyph->tex_id, m_glyph_size < 33.0f });
		m_object->setTexCoord(glyph->tex_coord);
		m_object->setPosition(object_pos);
		m_object->setColor(color);

		if (m_shadow_level > 0) {
			m_object->setFlags(3, m_shadow_level, m_masking);
			App.context->pushObject(m_object);
		}
		m_object->setFlags(3, 0, m_masking, m_size > 18.0f);
		App.context->pushObject(m_object);

		return glyph->advance * m_scale;
	}

	return 0.0f;
}

}