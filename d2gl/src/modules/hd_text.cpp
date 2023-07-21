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
#include "hd_text.h"
#include "d2/common.h"
#include "d2/stubs.h"
#include "modules/mini_map.h"
#include "modules/motion_prediction.h"
#include "option/menu.h"

namespace d2gl::modules {

HDText::HDText()
{
	m_lang_id = d2::getLangId();
	m_object_bg = std::make_unique<Object>();

	std::string lang_file = helpers::getLangString(true);
	auto buffer = helpers::loadFile("assets\\atlases\\" + lang_file + ".txt");
	if (!buffer.size && m_lang_id == LANG_SIN)
		buffer = helpers::loadFile("assets\\atlases\\chi.txt");
	if (!buffer.size)
		buffer = helpers::loadFile("assets\\atlases\\default.txt");

	if (buffer.size) {
		std::string data((const char*)buffer.data, buffer.size);
		auto lines = helpers::strToLines(data);
		delete[] buffer.data;

		TextureCreateInfo texture_ci;
		texture_ci.layer_count = 1;
		texture_ci.size = { 1024, 1024 };
		texture_ci.slot = TEXTURE_SLOT_FONTS;
		texture_ci.filter = { GL_LINEAR, GL_LINEAR };

		static std::unordered_map<std::string, GlyphSet*> glyph_sets;
		std::vector<std::vector<std::string>> info_list;
		for (auto& line : lines) {
			helpers::replaceAll(line, " ", "");
			helpers::replaceAll(line, "\r", "");

			auto info = helpers::splitToVector(line, '|');
			if (info.size() > 9) {
				if (glyph_sets.find(info[1]) == glyph_sets.end()) {
					auto buffer2 = helpers::loadFile("assets\\atlases\\" + info[1] + "\\data.csv");
					if (buffer2.size) {
						auto pos = (buffer2.data + (buffer2.size - 5));
						while (*pos != '\n' || pos == buffer2.data)
							pos--;
						std::string num = std::string((const char*)(pos + 1), 2);
						helpers::replaceAll(num, ",", "");
						texture_ci.layer_count += std::atoi(num.c_str()) + 1;
						delete[] buffer2.data;
					}
					glyph_sets.insert({ info[1], nullptr });
				}
				info_list.push_back(info);
			}
		}

		static std::unique_ptr<Texture> texture = Context::createTexture(texture_ci);
		static auto symbol_set = new GlyphSet(texture.get(), "NotoSymbol");

		for (auto& info : info_list) {
			const auto name = info[1];
			if (!glyph_sets[name])
				glyph_sets[name] = new GlyphSet(texture.get(), name, symbol_set);

			uint8_t id = (uint8_t)std::atoi(info[0].c_str());
			bool bordered = (id == 2 || id == 3 || id == 7 || id == 18);
			wchar_t color = g_initial_colors.find(id) != g_initial_colors.end() ? g_initial_colors.at(id) : 0;
			const auto offset = glm::vec2(std::stof(info[7]), std::stof(info[8]));

			FontCreateInfo font_ci = { name, std::stof(info[2]), std::stof(info[3]), std::stof(info[4]), std::stof(info[5]), std::stof(info[6]), offset, std::stof(info[9]), color, bordered };
			m_fonts[id] = std::make_unique<Font>(glyph_sets[name], font_ci);
		}

		if (m_lang_id != LANG_ENG && m_lang_id != LANG_DEF) {
			if (m_lang_id != LANG_POR && m_lang_id != LANG_SIN && m_lang_id != LANG_RUS) {
				for (size_t i = 0; i < g_options_texts.size(); i++)
					g_options_text_others.at(m_lang_id)[i].align = g_options_texts[i].align;
			}
			g_options_texts = g_options_text_others.at(m_lang_id);
			g_death_texts = g_death_text_others.at(m_lang_id);
		}
	}

#ifdef _HDTEXT
	App.hdt.fonts.items.push_back({ " 0: Character life/mana/stamina texts on stats panel", 0 });
	App.hdt.fonts.items.push_back({ " 1: All Normal texts", 1 });
	App.hdt.fonts.items.push_back({ " 2: Character create screen title / Esc menu smaller texts", 2 });
	App.hdt.fonts.items.push_back({ " 3: Character selection title / Esc menu bigger texts", 3 });
	App.hdt.fonts.items.push_back({ " 4: Skill Tree level number (2 digits)", 4 });
	App.hdt.fonts.items.push_back({ " 5: Menu screen copyright text", 5 });
	App.hdt.fonts.items.push_back({ " 6: All Small texts (smallest)", 6 });
	App.hdt.fonts.items.push_back({ " 7: Menu screen popup titles", 7 });
	App.hdt.fonts.items.push_back({ " 8: NPC talking Text", 8 });
	App.hdt.fonts.items.push_back({ " 9: Menu Button label", 9 });
	App.hdt.fonts.items.push_back({ "10: Menu Button label (smaller)", 10 });
	App.hdt.fonts.items.push_back({ "11: ? (Seems unused)", 11 });
	App.hdt.fonts.items.push_back({ "12: ? (Seems unused)", 12 });
	App.hdt.fonts.items.push_back({ "13: Messages / Heads up bubble text / Keybind configs", 13 });
	App.hdt.fonts.items.push_back({ "14: Tooltip text (items/skills/units etc) (Custom)", 14 });
	App.hdt.fonts.items.push_back({ "15: Tooltip text smaller (when text is bigger than screen) (Custom)", 15 });
	App.hdt.fonts.items.push_back({ "16: Dropped items label (Custom)", 16 });
	App.hdt.fonts.items.push_back({ "17: Level/Class text on char selection screen (Custom)", 17 });
	App.hdt.fonts.items.push_back({ "18: Level Entry text (Custom)", 18 });
	App.hdt.fonts.items.push_back({ "19: Minimap first line text/clock/fps counter (Custom)", 19 });
	App.hdt.fonts.items.push_back({ "20: Unused: For future reference (Custom)", 20 });
	App.hdt.fonts.items.push_back({ "21: Unused: For future reference (Custom)", 21 });
	App.hdt.fonts.items.push_back({ "22: Unused: For future reference (Custom)", 22 });
	getFont(0)->getMetrics();
#endif
}

void HDText::reset()
{
	m_masking = false;
	m_map_text_line = 1;
	m_is_player_dead = d2::isUnitDead(d2::getPlayerUnit());
}

void HDText::update()
{
	static bool mask = false;
	if (App.game.screen == GameScreen::Menu) {
		static glm::vec4 text_mask = glm::vec4(0.0f);

		if (m_masking != mask || text_mask != m_text_mask) {
			App.context->getCommandBuffer()->setHDTextMasking(m_masking, m_text_mask);
			mask = m_masking;
			text_mask = m_text_mask;
		}
	} else {
		if (mask) {
			App.context->getCommandBuffer()->setHDTextMasking(false, m_text_mask);
			mask = false;
		}
	}
	m_cur_level_no = App.game.screen == GameScreen::InGame ? *d2::level_no : 0;
}

bool HDText::drawText(const wchar_t* str, int x, int y, uint32_t color, uint32_t centered)
{
	if (!isActive() || !str)
		return false;

	auto font = getFont(m_text_size);
	font->setShadow(1);

	glm::vec2 pos = { (float)x, (float)y };
	const auto def_color = font->getColor();
	uint32_t text_color = g_text_colors.at(!color && def_color ? def_color : getColor(color));

	if (App.game.screen == GameScreen::Menu) {
		if (m_text_size == 1) {
			if (x == 113 || x == 385) {
				pos.y += isVer(V_109d) ? -5.0f : 8.0f;
				if (color == 0 || color == 2 || y == 155 || y == 248 || y == 341 || y == 434)
					font = getFont(color == 0 ? 17 : 6);
			}
			if (m_lang_id == LANG_JPN)
				pos.y -= font->getFontSize() * 0.38f;
			else if (m_lang_id == LANG_KOR)
				pos.y -= font->getFontSize() * 0.22f;
			else if (m_lang_id == LANG_CHI)
				pos.y += font->getFontSize() * 0.28f;
			else if (m_lang_id == LANG_POL)
				pos.y -= font->getFontSize() * 0.28f;
		} else if (m_text_size == 3) {
			if (m_lang_id == LANG_JPN)
				pos.y -= font->getFontSize() * 0.41f;
			else if (m_lang_id == LANG_KOR)
				pos.y -= font->getFontSize() * 0.22f;
			else if (m_lang_id == LANG_CHI)
				pos.y -= font->getFontSize() * 0.45f;
			else if (m_lang_id == LANG_POL)
				pos.y -= font->getFontSize() * 0.32f;
			else if (m_lang_id == LANG_RUS)
				pos.y -= font->getFontSize() * 0.14f;
		} else if (m_text_size == 7) {
			if (m_lang_id == LANG_POL)
				pos.y -= font->getFontSize() * 0.42f;
		} else if (m_text_size == 9) {
			if ((m_lang_id == LANG_JPN || m_lang_id == LANG_KOR) && (y == 504 || y == 506))
				pos.y += 6.9f;
		}
	}
	if (!ISGLIDE3X())
		pos.y += font->getFontSize() * 0.08f;

	static bool map_text = false;
	if (App.game.draw_stage == DrawStage::Map && modules::MiniMap::Instance().isActive()) {
		if (m_text_size == 6 && !*d2::automap_on) {
			m_map_names = true;
			return false;
		} else if (m_text_size != 6) {
			if (*d2::screen_shift != SCREENPANEL_NONE)
				return true;

			map_text = true;
			App.context->toggleDelayPush(true);

			bool hidden = *d2::automap_on || d2::isEscMenuOpen();
			font = getFont(m_map_text_line == 1 || !App.mini_map.text_over || hidden ? 19 : 6);
			const auto size = font->getTextSize(str);

			if (m_map_text_line == 1) {
				pos.x = App.game.size.x - modules::MiniMap::Instance().getTimeWidth() - size.x - 5.0f;
				pos.y = 13.0f;
			} else {
				pos.x = App.game.size.x - 12.0f - size.x;
				pos.y = 10.0f + m_map_text_line * font->getFontSize() * 1.34f;
				if (hidden) {
					pos.x += 7.0f;
					pos.y -= 10.0f;
				} else if (!App.mini_map.text_over) {
					pos.x += 7.0f;
					pos.y += (float)App.mini_map.height.value - 5.0f;
				}
			}
			m_map_text_line++;
		}
	}

	if (m_text_size == 13 && (x == 15 || (*d2::screen_shift == SCREENPANEL_LEFT && x == App.game.size.x / 2 + 15))) {
		auto size = font->getTextSize(str);
		const glm::vec2 padding = { 4.0f, 2.0f };
		glm::vec2 back_pos = { pos.x, pos.y - size.y };

		m_object_bg->setFlags(2);
		m_object_bg->setPosition(back_pos - padding);
		m_object_bg->setSize(size + padding * 2.0f);
		m_object_bg->setColor(m_alt_bg_color, 1);
		App.context->pushObject(m_object_bg);
		font->setShadow(2);
	}

	font->setMasking(m_masking);
	font->setAlign(TextAlign::Left);
	font->drawText(str, pos, text_color);

	if (map_text) {
		App.context->toggleDelayPush(false);
		map_text = false;
	}

	return true;
}

bool HDText::drawFramedText(const wchar_t* str, int x, int y, uint32_t color, uint32_t centered)
{
	if (!str || !isActive())
		return false;

	if (*d2::is_alt_clicked)
		return true;

	const auto unit = d2::getSelectedUnit();
	if (unit) {
		const bool is_monster = (unit->dwType == d2::UnitType::Monster && y == 32);
		if (unit->dwType == d2::UnitType::Player || d2::isMercUnit(unit) || is_monster) {
			m_hovered_unit.id = d2::getUnitID(unit) | ((uint8_t)unit->dwType << 24);
			m_hovered_unit.color = color;
			if (!is_monster) {
				wcscpy_s(m_hovered_unit.name, str);
				m_hovered_unit.pos = { x, y };
			}
			return false;
		}
	}

	float font_size;
	int line_count = 0;
	auto font = getFont(1);
	const auto text_color = g_text_colors.at(getColor(color));
	glm::vec2 pos, padding, box_size, size;

	if (unit && unit->dwType == d2::UnitType::Item) {
		font = getFont(16);
		size = font->getTextSize(str);
		line_count = font->getLineCount();
		font_size = font->getFontSize();

		padding = { 3.4f, glm::max(1.4f, (18.0f - font_size) / 2.0f) };
		pos = { (float)x - padding.x, (float)y - size.y };
		pos.y -= ((float)(line_count * 18 + 2) - size.y) / 2.0f;
		box_size = size + padding * 2.0f;
		m_object_bg->setFlags(2);
	} else {
		font = getFont(14);
		size = font->getTextSize(str);
		line_count = font->getLineCount();
		font_size = font->getFontSize();
		if (size.y > (float)App.game.size.y - 40.0f) {
			font = getFont(15);
			size = font->getTextSize(str);
			font_size = font->getFontSize();
		}

		padding = { 10.0f, 5.0f };
		pos = { (float)x - padding.x, (float)y - size.y - padding.y };
		box_size = size + padding * 2.0f;
		m_object_bg->setFlags(7, 2);

		const int mid_y = (int)(pos.y + box_size.y / 2);
		const int cursor_y = d2::getCursorPos().y;

		const auto item = d2::getSelectedItem();
		if (item && item->dwType == d2::UnitType::Item) {
			const auto quality = d2::getItemQuality(item);
			uint32_t border_color = m_border_color;
			switch (quality) {
				case d2::ItemQuality::Unique: border_color = 0x2B2215DD; break;
				case d2::ItemQuality::Set: border_color = 0x1C3418DD; break;
				case d2::ItemQuality::Rare: border_color = 0x31311EDD; break;
				case d2::ItemQuality::Craft: border_color = 0x2F2102DD; break;
				case d2::ItemQuality::Magic: border_color = 0x1D1D31DD; break;
			}

			pos.y = mid_y > cursor_y ? (float)(y - m_last_text_height) + 3.0f : (float)y - box_size.y;
			m_object_bg->setColor(border_color, 2);
		} else {
			// TODO: center skill hover text
			if (line_count > 2)
				pos.y = mid_y > cursor_y ? (float)(y - m_last_text_height) : (float)y - box_size.y;
			m_object_bg->setColor(m_border_color, 2);
		}
	}

	if (centered || line_count > 1)
		pos.x -= size.x / 2.0f;

	const float margin = 5.0f;
	if (pos.x + box_size.x > (float)App.game.size.x - margin)
		pos.x = (float)App.game.size.x - box_size.x - margin;
	else if (pos.x < margin)
		pos.x = margin;
	if (pos.y + box_size.y > (float)App.game.size.y - margin)
		pos.y = (float)App.game.size.y - box_size.y - margin;
	else if (pos.y < margin)
		pos.y = margin;

	App.context->toggleDelayPush(true);
	m_object_bg->setPosition(pos);
	m_object_bg->setSize(box_size);
	m_object_bg->setColor(m_bg_color, 1);
	m_object_bg->setExtra(box_size);
	App.context->pushObject(m_object_bg);

	pos.y += font_size;
	font->setShadow(0);
	font->setMasking(false);
	font->setAlign(TextAlign::Center);
	font->drawText(str, pos + padding, text_color, true);
	App.context->toggleDelayPush(false);

	return true;
}

bool HDText::drawRectangledText(const wchar_t* str, int x, int y, uint32_t rect_transparency, uint32_t color)
{
	if (!isActive() || !str)
		return false;

	// rectColor = 0, rectTransparency = 2, dwColor = 0   stats panel popup
	// rectColor = 0, rectTransparency = 1, dwColor = 0   alt item
	// rectColor = 140, rectTransparency = 5, dwColor = 0   alt item hovered

	float font_size;
	int line_count = 0;
	auto font = getFont(m_text_size);
	const auto text_color = g_text_colors.at(getColor(color));
	uint32_t bg_color = m_bg_color;
	glm::vec2 padding, back_pos, text_pos, size;

	if (rect_transparency == 2) {
		size = font->getTextSize(str);
		line_count = font->getLineCount();
		font_size = font->getFontSize();

		padding = { 10.0f, 5.0f };
		back_pos = { (float)x, (float)y - size.y };
		text_pos = { back_pos.x, back_pos.y + font_size };

		m_object_bg->setColor(m_border_color, 2);
		m_object_bg->setFlags(2, 2);
		m_object_bg->setExtra(size + padding * 2.0f);
	} else {
		font = getFont(16);
		size = font->getTextSize(str);
		line_count = font->getLineCount();
		font_size = font->getFontSize();

		padding = { 3.4f, glm::max(1.4f, (18.0f - font_size) / 2.0f) };
		back_pos = { (float)x + 5.0f, (float)y - size.y };
		back_pos.y -= ((float)(line_count * 18 + 2) - size.y) / 2.0f;
		text_pos = { back_pos.x, back_pos.y + font_size };

		if (rect_transparency == 1)
			bg_color = m_alt_bg_color;
		else if (rect_transparency == 5)
			bg_color = m_alt_bg_hovered;

		const float ext = glm::max(glm::min((float)(size.x * 0.1), 15.0f), 10.0f);
		padding.x += ext;
		back_pos.x -= ext;
		text_pos.x -= ext;
		const float ext_normal = ext / (size.x + 10.0f);

		m_object_bg->setFlags(6);
		m_object_bg->setExtra({ ext_normal, 1.0f - ext_normal });
	}

	m_object_bg->setPosition(back_pos);
	m_object_bg->setSize(size + padding * 2.0f);
	m_object_bg->setColor(bg_color, 1);
	App.context->pushObject(m_object_bg);

	font->setShadow(1);
	font->setMasking(false);
	font->setAlign(TextAlign::Center);
	font->drawText(str, text_pos + padding, text_color, true);

	return true;
}

bool HDText::drawSolidRect(int left, int top, int right, int bottom, uint32_t color, int draw_mode)
{
	if (App.game.screen != GameScreen::InGame || !isActive())
		return false;

	const int width = right - left;
	const int height = bottom - top;

	if (d2::is_unit_hovered) {
		const auto unit = d2::getSelectedUnit();
		if (unit && (unit->dwType == d2::UnitType::Player || d2::isMercUnit(unit))) {
			if (d2::is_unit_hovered == 1) {
				m_hovered_unit.hp[0] = width;
				m_hovered_unit.pos = { left, top };
			} else
				m_hovered_unit.hp[1] = width;
		}
		return true;
	}

	// if (draw_mode == 1 && top < 200) {
	//	trace("[] | %d | %d | %dx%d | %dx%d", color, draw_mode, left, top, width, height);
	// }

	if (color != 0) // skip drawing except black color
		return false;

	if (draw_mode == 5 && height == 5 && top == 14) // hireling & summon hp
		return false;

	if (width == App.game.size.x || height == App.game.size.y) // FreeRes black bars
		return false;

	if ((*d2::screen_shift == SCREENPANEL_LEFT || *d2::screen_shift == SCREENPANEL_BOTH) && width == 320 && (height == 432 || height == 236)) // Plugy stats panel bg
		return false;

	if (draw_mode == 2 && width == 24 && height <= 24) // PD2 buff timer bg
		return false;

	if (*d2::esc_menu_open && height == 30) // In-game option sliders
		return false;

	if (draw_mode == 1 && height == 16 && (left == 11 || left == 10 || (*d2::screen_shift == SCREENPANEL_LEFT && left == App.game.size.x / 2 + 10))) // message bg
		return true;

	uint32_t bg_color = 0x000000FF;
	switch (draw_mode) {
		case 0: bg_color = 0x00000066; break;
		case 1: bg_color = 0x00000099; break;
		case 2: bg_color = 0x000000CC; break;
		case 3: bg_color = 0x000000DD; break;
	}
	// draw_mode == 1 : NPC/Hire/Talk dialog bg & Key binding bg & option slider bg && BH settings btn/panel bg & message input bg & shrine text bg && message panel bg & message bg
	// draw_mode == 2 : PD2 buff timer bg & stat panel popup(hit chance) bg & small button label
	// draw_mode == 4 : PD2 stat panel bg
	// draw_mode == 5 : BH setting header bg
	// draw_mode == 5 : skill tab 1,2,3 etc & D2HD bottom panel bg(full width)/left(half width)/right(half width) & Plugy stats panel(320x432 left panel open)
	// draw_mode == 7 : BH setting tab bg
	// draw_mode == 8 : BH setting checkbox bg

	m_object_bg->setPosition({ (float)left, (float)top });
	m_object_bg->setSize({ (float)width, (float)height });
	m_object_bg->setColor(bg_color, 1);

	if (draw_mode == 1 && top == App.game.size.y - 103 && height == 47) { // message input bg
		m_object_bg->setColor(m_border_color, 2);
		m_object_bg->setFlags(2, 4);
		m_object_bg->setExtra({ (float)width, (float)height });
	} else
		m_object_bg->setFlags(2);

	App.context->pushObject(m_object_bg);

	return true;
}

uint32_t HDText::getNormalTextWidth(const wchar_t* str, const int n_chars)
{
	if (App.game.draw_stage == DrawStage::Map && modules::MiniMap::Instance().isActive() && m_text_size == 6 && !*d2::automap_on)
		return n_chars > 0 ? d2::getNormalTextNWidth(str, n_chars) : d2::getNormalTextWidth(str);

	const auto size = getFont(m_text_size)->getTextSize(str, n_chars);
	m_last_text_width = (uint32_t)size.x;

	return (uint32_t)size.x;
}

uint32_t HDText::getFramedTextSize(const wchar_t* str, uint32_t* width, uint32_t* height)
{
	const auto font = getFont(m_text_size);
	const auto size = font->getTextSize(str);

	*width = (uint32_t)(size.x + (m_text_size == 1 ? 10 : 0));
	*height = m_text_size == 1 ? (font->getLineCount() * 18 + 2) : (uint32_t)size.y;
	m_last_text_width = *width;
	m_last_text_height = *height;

	return *height;
}

uint16_t HDText::getFontHeight()
{
	if (App.game.screen != GameScreen::Menu)
		return (uint16_t)getFont(m_text_size)->getFontSize();

	return d2::getFontHeight();
}

void HDText::drawSubText(uint8_t fn)
{
	if (m_map_names) {
		m_map_names = false;
		return;
	}

	if (!isActive() || !d2::sub_text_ptr)
		return;

	const uintptr_t ptr = (uintptr_t)d2::sub_text_ptr;
	static const wchar_t* str;
	static uint32_t* color;
	static int *length, *x, *y;

	if (fn == 1) {
		if (isVerMax(V_110)) {
			str = *(const wchar_t**)(ptr + 0x20);
			color = (uint32_t*)(ptr + 0x78);
			length = (int*)(ptr + 0x1C);
			x = (int*)(ptr + 0x18);
			y = (int*)(ptr + 0x70);
		} else {
			str = *(const wchar_t**)(ptr + 0x68);
			color = (uint32_t*)(ptr + 0x74);
			length = (int*)(ptr + 0x10);
			x = (int*)(ptr + 0x18);
			y = (int*)(ptr + 0x6C);
		}
	} else if (fn == 2) {
		str = (const wchar_t*)(ptr + 0x390);
		color = (uint32_t*)(ptr + 0x74);
		length = (int*)(ptr + 0x14);
		x = (int*)(ptr + 0x1C);
		y = (int*)(ptr + 0x4);
	} else {
		str = (const wchar_t*)(ptr + 0x25C);
		color = (uint32_t*)(ptr + 0x10);
		length = (int*)(ptr - 0xC);
		x = (int*)(ptr - 0x14);
		y = (int*)(ptr + 0x8);
	}

	if (m_hovered_unit.id) {
		m_hovered_unit.id = 0;
		*length = 0;
		return;
	}

	if (str) {
		drawText(str, *x, *y, *color, 1);
		*length = 0;
	}
}

bool HDText::drawImage(d2::CellContext* cell, int x, int y, int draw_mode)
{
	if (!isActive() || !cell)
		return true;

	const auto cell_file = d2::getCellFile(cell);
	if (!cell_file)
		return true;

	if (App.game.screen == GameScreen::Menu && (cell_file->numcells == 1 || cell_file->numcells == 2 || cell_file->numcells == 4) && d2::getCellNo(cell) == 0) {
		const auto cell0 = cell_file->cells[0];
		if (cell0->width != 256)
			return true;

		const auto cell1 = cell_file->cells[cell_file->numcells - 1];
		for (auto& p : g_popups) {
			if (p.size0.y == cell0->height && (cell_file->numcells == 1 || (p.size1.x == cell1->width && p.size1.y == cell1->height))) {
				const glm::vec2 pos = { (float)(x - 1), (float)(y - p.size0.y - 1) };
				const glm::vec2 size = { (float)(p.size0.x + p.size1.x + 2), (float)(p.size0.y + (cell_file->numcells > 2 ? p.size1.y : 0) + 2) };

				const auto m1 = m_mvp * glm::vec4(pos, 0.0f, 1.0f);
				const auto m2 = m_mvp * glm::vec4((pos + size), 0.0f, 1.0f);
				m_text_mask = glm::vec4(m1.x, m1.y, m2.x, m2.y);
				m_masking = true;

				return true;
			}
		}
	} else if (App.game.draw_stage == DrawStage::UI && *d2::esc_menu_open) {
		const auto cell_no = d2::getCellNo(cell);
		if (cell_file->cells[0]->height != g_text_size[0].large_text.x && cell_file->cells[0]->height != g_text_size[0].large_text.y &&
			cell_file->cells[0]->height != g_text_size[m_lang_id].large_text.x && cell_file->cells[0]->height != g_text_size[m_lang_id].large_text.y)
			return true;

		for (auto& p : g_options_texts) {
			const auto cell = cell_file->cells[p.cell_num];
			if (p.cell_num == cell_file->numcells - 1 && cell->width == p.size.x && cell->height == p.size.y) {
				if (cell_no == 0) {
					auto image_width = cell->width + (p.cell_num > 0 ? cell_file->cells[0]->width : 0);
					if (p.cell_num == 2)
						image_width += cell_file->cells[1]->width;
					glm::ivec2 pos = { x, y };
					const auto old_size = m_text_size;
					setTextSize(cell->height == g_text_size[0].large_text.x || cell->height == g_text_size[m_lang_id].large_text.x ? 3 : 2);

					if (p.align == TextAlign::Center) {
						auto text_width = getNormalTextWidth(p.str, 0);
						pos.x = pos.x + image_width / 2 - text_width / 2;
					} else if (p.align == TextAlign::Right) {
						auto text_width = getNormalTextWidth(p.str, 0);
						pos.x = pos.x + image_width - text_width;
					}
					drawText(p.str, pos.x, pos.y, draw_mode == 1 ? 16 : 0, 0);
					setTextSize(old_size);
				}
				return false;
			}
		}
	}

	if (cell_file->numcells == 22 && cell_file->cells[0]->width == 14 && cell_file->cells[0]->height == 15) // Box Frames
		return false;

	return true;
}

bool HDText::drawShiftedImage(d2::CellContext* cell, int x, int y)
{
	if (!isActive())
		return true;

	if (App.game.draw_stage == DrawStage::HUD && m_is_player_dead) {
		const auto cell_file = d2::getCellFile(cell);
		if (!cell_file)
			return true;

		const auto cell_no = d2::getCellNo(cell);
		if (cell_file->cells[0]->height != g_text_size[0].large_text.x && cell_file->cells[0]->height != g_text_size[m_lang_id].large_text.x)
			return true;

		for (auto& p : g_death_texts) {
			const auto cell = cell_file->cells[p.cell_num];
			if (p.cell_num == cell_file->numcells - 1 && cell->width == p.size.x && cell->height == p.size.y) {
				if (cell_no == 0) {
					auto image_width = cell->width + (p.cell_num > 0 ? cell_file->cells[0]->width : 0);
					if (p.cell_num == 2)
						image_width += cell_file->cells[1]->width;
					glm::ivec2 pos = { x, y };
					const auto old_size = m_text_size;
					setTextSize(3);

					auto text_width = getNormalTextWidth(p.str, 0);
					pos.x = pos.x + image_width / 2 - text_width / 2;
					drawText(p.str, pos.x, pos.y, 17, 0);
					setTextSize(old_size);
				}
				return false;
			}
		}
	}

	if (m_entry_text) {
		const auto cell_file = d2::getCellFile(cell);
		if (cell_file->cells[0]->height == g_text_size[m_lang_id].entry_text.x || cell_file->cells[0]->height == g_text_size[m_lang_id].entry_text.y)
			return false;
	}
	m_entry_text = false;

	return true;
}

void HDText::drawRectFrame()
{
	if (!isActive() || !d2::currently_drawing_rect)
		return;

	auto rect = d2::currently_drawing_rect;
	glm::vec2 pos = { (float)rect->left, (float)rect->top };
	glm::vec2 size = { (float)(rect->right - rect->left + 1), (float)(rect->bottom - rect->top + 1) };

	if (rect->top == 6 && rect->right - rect->left == 327) // talking text
		size.x -= 1.0f;

	if (*(bool*)(d2::esc_menu_open + 8)) {
		pos.y -= 3.0f;
		size += glm::vec2(2.0f, 3.0f);
	}

	m_object_bg->setPosition(pos);
	m_object_bg->setSize(size);
	m_object_bg->setColor(0x000000CC, 1);
	m_object_bg->setColor(0x333333FF, 2);
	m_object_bg->setFlags(4, 3);
	m_object_bg->setExtra(size);
	App.context->pushObject(m_object_bg);
}

void HDText::loadUIImage()
{
	if (App.ready && d2::ui_image_path && _strnicmp(d2::ui_image_path + 12, "ui", 2)) {
		std::string file_name = (d2::ui_image_path + 18);
		helpers::strToLower(file_name);

		const auto vec = &g_option_overwrites[m_lang_id];
		if (std::find(vec->begin(), vec->end(), file_name) != vec->end()) {
			const auto len = strlen(d2::ui_image_path);
			((char*)d2::ui_image_path)[len - 2] = '_';
			((char*)d2::ui_image_path)[len - 1] = 'n';
		}
	}
}

void HDText::startEntryText()
{
	if (!isActive() || App.game.screen != GameScreen::InGame)
		return;

	m_entry_text = true;
	if (isVerMax(V_110)) {
		static int level = 0;
		if (level != *d2::level_no) {
			m_entry_text_draw = true;
			m_entry_timer = std::clock() + 3000;
			level = *d2::level_no;
		}
	} else {
		if (m_cur_level_no != *d2::level_no) {
			m_entry_text_draw = true;
			m_entry_timer = std::clock() + 3000;
		}
	}
}

void HDText::drawEntryText()
{
	if (App.game.screen != GameScreen::InGame || !m_entry_text_draw)
		return;

	if (m_entry_timer < std::clock())
		m_entry_text_draw = false;

	if (App.game.screen != GameScreen::InGame)
		return;

	const auto old_size = m_text_size;
	setTextSize(18);
	const auto level_name = d2::getLevelName(*d2::level_no);
	const auto text_width = getNormalTextWidth(level_name, 0);
	drawText(level_name, *d2::screen_width / 2 - text_width / 2, (uint32_t)((float)*d2::screen_height / 4.5f), 17, 0);
	setTextSize(old_size);
}

void HDText::drawUnitHealthBar()
{
	d2::is_unit_hovered = 0;
	if (!isActive())
		return;

	if (const auto unit = d2::getSelectedUnit()) {
		if (unit->dwType == d2::UnitType::Player || d2::isMercUnit(unit))
			drawPlayerHealthBar(unit);
		else if (unit->dwType == d2::UnitType::Monster)
			drawMonsterHealthBar(unit);
	}
}

void HDText::drawMonsterHealthBar(d2::UnitAny* unit)
{
	const auto name = d2::getMonsterName(unit);
	if (!name)
		return;

	const auto hp = d2::getUnitStat(unit, 6);
	const auto max_hp = d2::getUnitStat(unit, 7);
	const auto type = d2::getMonsterType(unit);

	const auto font = getFont(1);
	font->setShadow(2);
	font->setMasking(false);
	font->setAlign(TextAlign::Center);

	float center = (float)(*d2::screen_width / 2);
	if (*d2::screen_shift == SCREENPANEL_RIGHT)
		center = (float)(*d2::screen_width / 4);
	else if (*d2::screen_shift == SCREENPANEL_LEFT)
		center = (float)(*d2::screen_width / 4 * 3);

	const auto text_size = font->getTextSize(name);
	float hp_percent = (float)hp / (float)max_hp;

	glm::vec2 bar_size = { 140.0f, 18.0f };
	if (text_size.x + 40.0f > bar_size.x)
		bar_size.x = text_size.x + 40.0f;

	glm::vec2 bar_pos = { center - bar_size.x / 2, 18.0f };

	m_object_bg->setFlags(2);
	m_object_bg->setPosition(bar_pos);
	m_object_bg->setSize({ bar_size.x * hp_percent, bar_size.y });
	m_object_bg->setColor(m_monster_hp, 1);
	App.context->pushObject(m_object_bg);

	if (hp_percent < 1.0f) {
		m_object_bg->setPosition({ bar_pos.x + bar_size.x * hp_percent, bar_pos.y });
		m_object_bg->setSize({ bar_size.x * (1.0f - hp_percent), bar_size.y });
		m_object_bg->setColor(m_bg_color, 1);
		App.context->pushObject(m_object_bg);
	}

	wchar_t text_color = L'\x30';
	if (type == d2::MonsterType::Boss || type == d2::MonsterType::SuperUnique)
		text_color = L'\x34';
	else if (type == d2::MonsterType::Champion)
		text_color = L'\x33';
	else
		text_color = getColor(m_hovered_unit.color);
	if (hp == 0)
		text_color = L'\x31';

	glm::vec2 text_pos = { center - text_size.x / 2, 19.3f + 14.5f };
	font->drawText(name, text_pos, g_text_colors.at(text_color));
	m_hovered_unit.color = 0;
}

void HDText::drawPlayerHealthBar(d2::UnitAny* unit)
{
	const uint32_t hp = m_hovered_unit.hp[0];
	const uint32_t max_hp = m_hovered_unit.hp[0] + m_hovered_unit.hp[1];

	const auto font = getFont(1);
	font->setShadow(2);
	font->setMasking(false);
	font->setAlign(TextAlign::Center);

	if (isVerMax(V_110) && unit->dwType == d2::UnitType::Player)
		mbstowcs_s(nullptr, m_hovered_unit.name, d2::getPlayerName(unit), 16);
	const wchar_t* name = unit->dwType == d2::UnitType::Player ? m_hovered_unit.name : d2::getMonsterName(unit);
	const auto text_size = font->getTextSize(name);
	const float hp_percent = (float)hp / (float)max_hp;

	glm::vec2 bar_size = { 60.0f, 16.0f };
	if (text_size.x + 20.0f > bar_size.x)
		bar_size.x = text_size.x + 20.0f;

	auto offset = modules::MotionPrediction::Instance().getUnitOffset(m_hovered_unit.id);
	const float center = (float)(m_hovered_unit.pos.x + max_hp / 2) + (float)offset.x;
	glm::vec2 bar_pos = { center - bar_size.x / 2, (float)m_hovered_unit.pos.y + (float)offset.y };

	m_object_bg->setFlags(2);
	m_object_bg->setPosition(bar_pos);
	m_object_bg->setSize({ bar_size.x * hp_percent, bar_size.y });
	m_object_bg->setColor(m_monster_hp, 1);
	App.context->pushObject(m_object_bg);

	if (hp_percent < 1.0f) {
		m_object_bg->setPosition({ bar_pos.x + bar_size.x * hp_percent, bar_pos.y });
		m_object_bg->setSize({ bar_size.x * (1.0f - hp_percent), bar_size.y });
		m_object_bg->setColor(m_bg_color, 1);
		App.context->pushObject(m_object_bg);
	}

	glm::vec2 text_pos = { center - text_size.x / 2, bar_pos.y + 14.5f };
	font->drawText(name, text_pos, g_text_colors.at(getColor(m_hovered_unit.color)));

	m_hovered_unit.hp[0] = m_hovered_unit.hp[1] = 0;
	m_hovered_unit.color = 0;
}

inline wchar_t HDText::getColor(uint32_t color)
{
	return (color < g_default_colors.size()) ? g_default_colors[color] : g_default_colors[0];
}

void HDText::drawFpsCounter()
{
	if (!App.show_fps || App.game.screen != GameScreen::InGame)
		return;

	static wchar_t str[20] = { 0 };
	const float fps = (float)round(1000.0 / App.context->getAvgFrameTime());
	swprintf_s(str, L"FPS: %.0f", fps);

	const auto old_size = HDText::Instance().getTextSize();
	d2::setTextSizeHooked(19);
	const auto width = d2::getNormalTextWidthHooked(str);
	d2::drawNormalTextHooked(str, App.game.size.x / 2 - width / 2, App.game.size.y - 58, 4, 0);
	d2::setTextSizeHooked(old_size);
}

void HDText::drawItemQuantity(bool draw, int x, int y)
{
	if (!App.show_item_quantity || App.game.screen != GameScreen::InGame || !d2::currently_drawing_item)
		return;

	static glm::ivec2 item_pos = { 0, 0 };
	if (!draw) {
		item_pos = { x, y };
		return;
	}

	const auto item = d2::currently_drawing_item;
	if (item->dwType == d2::UnitType::Item && d2::getItemLocation(item) != 0xFF) {
		if (const auto quantity = d2::getUnitStat(item, 70)) {
			static wchar_t str[10] = { 0 };
			swprintf_s(str, L"%d", quantity);

			const auto old_size = modules::HDText::Instance().getTextSize();
			d2::setTextSizeHooked(6);
			if (App.hd_text.active) {
				static auto bg = std::make_unique<Object>();
				uint32_t width, height;

				d2::getFramedTextSizeHooked(str, &width, &height);
				glm::vec2 size = { (float)(width + 10), (float)(height + 2) };
				glm::vec2 pos = { (float)(item_pos.x - 5 + 3), (float)(item_pos.y - 1 - height - 4) };

				bg->setFlags(6, 0, 0, 1);
				bg->setPosition(pos);
				bg->setSize(size);
				bg->setColor(0x00000099, 1);
				bg->setExtra({ 0.4f, 0.6f });
				App.context->pushObject(bg);
			}
			d2::drawNormalTextHooked(str, item_pos.x + 3, item_pos.y - 4, 0, 0);
			d2::setTextSizeHooked(old_size);
		}
	}
	d2::currently_drawing_item = nullptr;
}

void HDText::updateFontSize()
{
	for (auto& p : m_fonts)
		p.second->setSize();
}

#ifdef _HDTEXT
void HDText::showSampleText()
{
	if (App.game.screen == GameScreen::InGame && App.hdt.show_sample && option::Menu::instance().isVisible()) {
		auto id = (uint32_t)App.hdt.fonts.items[App.hdt.fonts.selected].value;
		auto lang_id = d2::getLangId();
		auto texts = g_sample_text.at((d2::isLangCJK(lang_id) || lang_id == LANG_RUS) ? lang_id : LANG_ENG);

		const auto old_size = HDText::Instance().getTextSize();
		d2::drawSolidRectEx(40, 40, App.game.size.x - 40, App.game.size.y - 100, 1, 2);
		if (id <= 13) {
			bool old_val = App.hd_text.active;
			App.hd_text.active = false;
			d2::setTextSize(id);
			d2::drawNormalText(texts.txt1, 60, 140, 0, 0);
			d2::setTextSize(id);
			d2::drawNormalText(texts.txt2, 60, 260, 0, 0);
			d2::setTextSize(id);
			d2::drawNormalText(texts.txt3, 60, 460, 0, 0);
			App.hd_text.active = old_val;
		}

		d2::drawSolidRectEx(60, 60, App.game.size.x - 60, App.game.size.y - 140, 1, 1);
		d2::setTextSizeHooked(id);
		d2::drawNormalTextHooked(texts.txt1, 60, 100, 0, 0);
		d2::setTextSizeHooked(id);
		d2::drawNormalTextHooked(texts.txt1, 60, 140, 0, 0);
		d2::setTextSizeHooked(id);
		d2::drawNormalTextHooked(texts.txt2, 60, 260, 0, 0);
		d2::setTextSizeHooked(id);
		d2::drawNormalTextHooked(texts.txt3, 60, 460, 0, 0);
		d2::setTextSizeHooked(old_size);
	}
}

const std::string& HDText::getAllFontMetricString()
{
	static std::string metric_text;
	metric_text = "";

	const auto& last_key = m_fonts.rbegin()->first;
	for (auto& font : m_fonts) {
		metric_text += std::string(font.second->getMetricsString(font.first));
		if (font.first != last_key)
			metric_text += "\n";
	}
	return metric_text;
}
#endif
}