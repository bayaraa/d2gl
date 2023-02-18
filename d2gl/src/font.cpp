#include "pch.h"
#include "font.h"
#include "helpers.h"

namespace d2gl {

// clang-format off
const std::map<wchar_t, uint32_t> g_text_colors = {
	{ L'\x01', 0xDFB679FF }, { L'\x02', 0x000000FF }, { L'\x03', 0xDFB67966 }, { L'\x04', 0xC22121FF },
	{ L'\x05', 0x959595FF }, { L'\x06', 0xE09595FF }, { L'\x07', 0xB6E12EFF }, { L'\x08', 0x9EA0A0FF },
	{ L'\x09', 0x5CA6A4FF }, { L'\x0A', 0xA2AAA5FF }, { L'\x0B', 0xA6E0A7FF }, { L'\x0C', 0xA9A9B0FF },
	{ L'\x0D', 0xA9B1B1FF }, { L'\x0E', 0xE2ABE2FF }, { L'\x0F', 0xEB2EAEFF }, { L'\x10', 0xB0B2E2FF },
	{ L'\x11', 0xEBEBE1FF }, { L'\x12', 0xE22EB8FF }, { L'\x13', 0xEBEBEBFF }, { L'\x14', 0x21C7CAFF },
	{ L'\x15', 0xCB21CDFF }, { L'\x16', 0xCED126FF }, { L'\x17', 0xD2D328FF }, { L'\x18', 0xD5D6D8FF },
	{ L'\x19', 0xD9D965FF }, { L'\x1A', 0xDA29DAFF }, { L'\x1B', 0xDC68DBFF }, { L'\x1C', 0xDCAD2BFF },
	{ L'\x1D', 0x2BDFDFFF }, { L'\x1E', 0xDFADDFFF }, { L'\x1F', 0x2CE02CFF }, { L'\x21', 0xE0B3B3FF },
	{ L'\x22', 0x2C2CE1FF }, { L'\x23', 0xE1E1E2FF }, { L'\x24', 0xE2EBEBFF }, { L'\x25', 0xEBEB77FF },
	{ L'\x26', 0x898D8BFF }, { L'\x27', 0x8D8D8BFF }, { L'\x28', 0x8EA1A0FF }, { L'\x29', 0x0DCCE9FF },
	{ L'\x2A', 0x2325CEFF }, { L'\x2B', 0x8DEAD3FF }, { L'\x2C', 0xD5D3EAFF }, { L'\x2D', 0xEAD8D9FF },
	{ L'\x2E', 0x29DCDCFF }, { L'\x2F', 0xDFDFEBFF }, { L'\x30', 0xFEFEFEFF }, { L'\x31', 0xFE6950FF },
	{ L'\x32', 0x25FE00FF }, { L'\x33', 0x7B7BFEFF }, { L'\x34', 0xDFB679FF }, { L'\x35', 0x7B7B7BFF },
	{ L'\x36', 0x00000000 }, { L'\x37', 0xD9CD8EFF }, { L'\x38', 0xFFB200FF }, { L'\x39', 0xEBEB75FF },
	{ L'\x3A', 0x008C00FF }, { L'\x3B', 0xB700FFFF }, { L'\x3C', 0x00D200FF }, { L'\x3D', 0xEBEBEBFF },
};

const std::array<wchar_t, 18> g_default_colors = {
	L'\x30', // White (Item Descriptions)
	L'\x31', // Red
	L'\x32', // Bright Green (Set Items)
	L'\x33', // Blue (Magic Items)
	L'\x34', // Gold (Unique Items)
	L'\x35', // Dark Gray (Socketed / Ethereal Items)
	L'\x36', // Black
	L'\x37', // Tan
	L'\x38', // Orange (Crafted Items)
	L'\x39', // Yellow (Rare Items)
	L'\x3A', // Dark Green
	L'\x3B', // Purple
	L'\x2F', // White (Brighter than Light Gray)
	L'\x2E', // Messed Up White (Same as above but text is messed up)
	L'\x09', // Teal
	L'\x0C', // Light Gray
	L'\x03', // Options (Half transparent)
	L'\x04', // Red (Death texts)
};

// clang-format on

Font::Font(const FontCreateInfo& info)
	: m_rtrim_size(info.rtrim_size), m_offset(info.offset)
{
	TextureCreateInfo texture_ci;
	texture_ci.layer_count = 3;
	texture_ci.size = { 860, 860 };
	texture_ci.slot = TEXTURE_SLOT_FONTS;
	texture_ci.min_filter = GL_LINEAR;
	texture_ci.mag_filter = GL_LINEAR;
	static std::unique_ptr<Texture> texture = Context::createTexture(texture_ci);

	auto data_buffer = helpers::loadFile("assets\\fonts\\" + info.path + "\\data.csv");
	if (!data_buffer.size)
		return;

	auto image_data = helpers::loadImage("assets\\fonts\\" + info.path + "\\atlas.png");
	auto tex_data = texture->fillImage(image_data);
	helpers::clearImage(image_data);

	m_sub_tex_id = tex_data.start_layer;

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
		m_glyphes[cc].tex_coord = coords / 860.0f;
		m_glyphes[cc].offset = { bounds.x * m_glyph_size, -bounds.w * m_glyph_size };
	}

	m_object = std::make_unique<Object>();
	m_object->setTexIds({ TEXTURE_SLOT_FONTS, m_sub_tex_id });
	m_object->setFlags({ 3, 0, 0, 0 });
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
	m_shadow_color = 0x000000CC;

	if (color == 0xC22121FF)
		border_color = 0x4A1515FF;
	else if (color == 0xDFB67966) {
		border_color = 0x443B2966;
		m_shadow_color = 0x00000055;
	} else if (color == 0xFF)
		m_shadow_color = 0x00000055;

	m_object->setColor(border_color, 2);
	m_object->setExtra({ m_smoothness, m_weight });

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
		m_object->setTexCoord(glyph->tex_coord);

		m_object->setPosition(object_pos + glm::min(4.0f * m_scale, 1.2f));
		m_object->setColor(m_shadow_color);
		m_object->setFlags({ 3, 0, m_masking, 0 });
		App.context->pushObject(m_object);

		m_object->setPosition(object_pos);
		m_object->setColor(color);
		m_object->setFlags({ 3, 0, m_masking, m_scale > 0.3f ? 1 : 0 });
		App.context->pushObject(m_object);

		return glyph->advance * m_scale;
	}

	return 0.0f;
}

}