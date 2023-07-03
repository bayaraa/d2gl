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

namespace d2gl::modules {

HDText::HDText()
{
	m_lang_id = d2::getLangId();
	initLangVariables(m_lang_id);
	m_object_bg = std::make_unique<Object>();

	if (m_lang_id == LANG_JPN) {
		m_fonts[0] = std::make_unique<Font>(FontCreateInfo{ "NotoSansJP", 5, 0.022f, { 0.00f, App.api == Api::Glide ? -0.20f : -0.12f }, 32.0f });
		m_fonts[1] = std::make_unique<Font>(FontCreateInfo{ "NotoSerifJP", 5, 0.012f, { 0.01f, App.api == Api::Glide ? -0.31f : -0.18f }, 32.0f });
	} else if (m_lang_id == LANG_KOR) {
		m_fonts[0] = std::make_unique<Font>(FontCreateInfo{ "NotoSansKR", 3, 0.022f, { 0.00f, App.api == Api::Glide ? -0.20f : -0.12f }, 32.0f });
		m_fonts[1] = std::make_unique<Font>(FontCreateInfo{ "NotoSerifKR", 3, 0.012f, { 0.01f, App.api == Api::Glide ? -0.31f : -0.18f }, 32.0f });
	} else if (m_lang_id == LANG_CHI) {
		m_fonts[0] = std::make_unique<Font>(FontCreateInfo{ "NotoSansSC", 9, 0.022f, { 0.00f, App.api == Api::Glide ? -0.20f : -0.12f }, 32.0f });
		m_fonts[1] = std::make_unique<Font>(FontCreateInfo{ "NotoSerifSC", 9, 0.012f, { 0.01f, App.api == Api::Glide ? -0.31f : -0.18f }, 32.0f });
	} else {
		m_fonts[0] = std::make_unique<Font>(FontCreateInfo{ "ExocetBlizzard", 2, 0.022f, { 0.00f, App.api == Api::Glide ? -0.20f : -0.12f } });
		m_fonts[1] = std::make_unique<Font>(FontCreateInfo{ "AlegreyaSans", 2, 0.012f, { 0.01f, App.api == Api::Glide ? -0.28f : -0.15f } });
		if (m_lang_id == LANG_RUS)
			m_fonts[2] = std::make_unique<Font>(FontCreateInfo{ "Macho", 2, 0.012f, { 0.01f, App.api == Api::Glide ? -0.28f : -0.15f } });
		else
			m_fonts[2] = std::make_unique<Font>(FontCreateInfo{ "Formal436BT", 1, 0.012f, { 0.01f, App.api == Api::Glide ? -0.31f : -0.18f } });
		m_fonts[3] = std::make_unique<Font>(FontCreateInfo{ "ExocetReaper", 2, 0.0f, { 0.0f, App.api == Api::Glide ? -0.14f : -0.10f } });
	}
}

void HDText::reset()
{
	m_masking = false;
	m_is_player_dead = d2::isUnitDead(d2::getPlayerUnit());
	m_map_text_line = 1;
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
	m_fonts[font.id]->setSize(font.size);
	m_fonts[font.id]->setMetrics(font);
	m_fonts[font.id]->setShadow(1);

	glm::vec2 pos = { (float)x, (float)y };
	uint32_t text_color = g_text_colors.at(!color && font.color ? font.color : getColor(color));

	if (App.game.screen == GameScreen::Menu) {
		if (m_text_size == 1 && (x == 113 || x == 385)) {
			pos.y += isVer(V_109d) ? -5.0f : 8.0f;
			if (color == 0 || color == 2 || y == 155 || y == 248 || y == 341 || y == 434) {
				font = getFont(color == 0 ? 98 : 6);
				m_fonts[font.id]->setSize(font.size);
				m_fonts[font.id]->setMetrics(font);
				pos.x += 2.0f;
			}
		}
		if (d2::isLangCJK(m_lang_id)) {
			switch (m_text_size) {
				case 1: pos.y -= font.size * 0.26f; break;
				case 7: pos.y -= font.size * 0.32f; break;
				case 9: pos.y -= font.size * 0.27f; break;
				case 10: pos.y -= font.size * 0.22f; break;
			}
			if (m_lang_id == LANG_JPN || m_lang_id == LANG_KOR) {
				if (m_text_size == 3)
					pos.y -= font.size * 0.08f;
				else if (m_text_size == 9 && (y == 504 || y == 506))
					pos.y += 7.0f;
			}
			if (m_text_size == 3) {
				if (m_lang_id == LANG_JPN)
					pos.y -= font.size * 0.20f;
				else if (m_lang_id == LANG_CHI)
					pos.y -= font.size * 0.29f;
			}
		} else if (m_lang_id == LANG_POL) {
			switch (m_text_size) {
				case 1: pos.y -= font.size * 0.28f; break;
				case 3: pos.y -= font.size * 0.34f; break;
				case 7: pos.y -= font.size * 0.42f; break;
				case 9: pos.y -= font.size * 0.27f; break;
				case 10: pos.y -= font.size * 0.27f; break;
			}
		} else {
			switch (m_text_size) {
				case 3: pos.y -= font.size * 0.14f; break;
				case 7: pos.y -= font.size * 0.16f; break;
				case 9: pos.y -= font.size * 0.06f; break;
				case 10: pos.y += font.size * 0.06f; break;
			}
		}
	} else {
		if (App.api == Api::Glide && (m_text_size == 13 || m_text_size == 0))
			pos.y += font.size * 0.08f;
	}

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

			font = getFont(m_map_text_line == 1 || !App.mini_map.text_over ? 99 : 6);
			m_fonts[font.id]->setSize(font.size);
			m_fonts[font.id]->setMetrics(font);
			const auto size = m_fonts[font.id]->getTextSize(str);

			if (m_map_text_line == 1) {
				pos.x = App.game.size.x - 50.0f - size.x;
				pos.y = 15.2f;
			} else {
				pos.x = App.game.size.x - 12.0f - size.x;
				pos.y = 10.0f + m_map_text_line * font.size * 1.34f;
				if (!App.mini_map.text_over) {
					pos.x += 7.0f;
					pos.y += (float)App.mini_map.height.value - 5.0f;
				}
			}
			m_map_text_line++;
		}
	}

	m_fonts[font.id]->setBoxed(false);
	m_fonts[font.id]->setMasking(m_masking);
	m_fonts[font.id]->setAlign(TextAlign::Left);
	m_fonts[font.id]->drawText(str, pos, text_color);

	if (map_text) {
		App.context->toggleDelayPush(false);
		map_text = false;
	}

	return true;
}

bool HDText::drawFramedText(const wchar_t* str, int x, int y, uint32_t color, uint32_t centered)
{
	if (!isActive() || !str)
		return false;

	const auto unit = d2::getSelectedUnit();
	if (unit) {
		if (unit->dwType == d2::UnitType::Monster && isVerMin(V_111) && y == 32 && !centered) {
			drawMonsterHealthBar(unit);
			return true;
		}

		if (unit->dwType == d2::UnitType::Player || d2::isMercUnit(unit)) {
			m_hovered_player_id = d2::getUnitID(unit) | ((uint8_t)unit->dwType << 24);
			if (isVer(V_114d))
				wcscpy_s(m_hovered_player_str, str);
			return false;
		}
	}

	auto font = getFont(1);
	m_fonts[font.id]->setSize(font.size);
	m_fonts[font.id]->setMetrics(font);
	m_fonts[font.id]->setShadow(0);

	auto size = m_fonts[font.id]->getTextSize(str);
	if (size.y > (float)App.game.size.y - 40.0f) {
		font = getFont(99);
		m_fonts[font.id]->setSize(font.size);
		m_fonts[font.id]->setMetrics(font);
		size = m_fonts[font.id]->getTextSize(str);
	}

	const uint32_t text_color = g_text_colors.at(font.color ? font.color : getColor(color));
	const int line_count = m_fonts[font.id]->getLineCount();

	glm::vec2 pos;
	glm::vec2 padding = { 10.0f, 5.0f };

	if (unit && unit->dwType == d2::UnitType::Item) {
		padding = { 3.4f, font.size * 0.05f * line_count };
		pos = { (float)x - padding.x - 1.0f, (float)y - size.y - padding.y + 1.0f };
		m_object_bg->setFlags(2);
	} else {
		pos = { (float)x - padding.x, (float)y - size.y - padding.y };
		m_object_bg->setFlags(2, 2);
	}

	glm::vec2 box_size = size + padding * 2.0f;

	if (centered)
		pos.x -= size.x / 2.0f;
	else if (line_count > 1)
		pos.x -= size.x / 2.0f - 24.0f;

	App.context->toggleDelayPush(true);

	const auto item = d2::getSelectedItem();
	if (item && item->dwType == d2::UnitType::Item) {
		const int mid_y = (int)(pos.y + box_size.y / 2);
		const int cursor_y = d2::getCursorPos().y;
		if (mid_y > cursor_y)
			pos.y = (float)(y - m_last_text_height);
		else
			pos.y = (float)y - box_size.y;

		const auto quality = d2::getItemQuality(item);
		uint32_t border_color = m_border_color;
		switch (quality) {
			case d2::ItemQuality::Unique: border_color = 0x2B2215DD; break;
			case d2::ItemQuality::Set: border_color = 0x1C3418DD; break;
			case d2::ItemQuality::Rare: border_color = 0x31311EDD; break;
			case d2::ItemQuality::Craft: border_color = 0x2F2102DD; break;
			case d2::ItemQuality::Magic: border_color = 0x1D1D31DD; break;
		}
		m_object_bg->setColor(border_color, 2);
	} else
		m_object_bg->setColor(m_border_color, 2);

	const float margin = 5.0f;
	if (pos.x + box_size.x > (float)App.game.size.x - margin)
		pos.x = (float)App.game.size.x - box_size.x - margin;
	else if (pos.x < margin)
		pos.x = margin;
	if (pos.y + box_size.y > (float)App.game.size.y - margin)
		pos.y = (float)App.game.size.y - box_size.y - margin;
	else if (pos.y < margin)
		pos.y = margin;

	m_object_bg->setPosition(pos);
	m_object_bg->setSize(box_size);
	m_object_bg->setColor(m_bg_color, 1);
	m_object_bg->setExtra(box_size);
	App.context->pushObject(m_object_bg);

	static float lh_diff = App.api == Api::Glide ? 0.06f : 0.12f;
	const auto line_height = m_fonts[font.id]->getLineHeight();
	pos.y += line_height - line_height * lh_diff;

	m_fonts[font.id]->setBoxed(true);
	m_fonts[font.id]->setMasking(false);
	m_fonts[font.id]->setAlign(TextAlign::Center);
	m_fonts[font.id]->drawText(str, pos + padding, text_color);

	App.context->toggleDelayPush(false);

	m_draw_sub_text = false;

	return true;
}

bool HDText::drawRectangledText(const wchar_t* str, int x, int y, uint32_t rect_transparency, uint32_t color)
{
	if (!isActive() || !str)
		return false;

	// rectColor = 0, rectTransparency = 2, dwColor = 0   stats panel popup
	// rectColor = 0, rectTransparency = 1, dwColor = 0   alt item
	// rectColor = 140, rectTransparency = 5, dwColor = 0   alt item hovered

	const auto font = getFont(m_text_size);
	m_fonts[font.id]->setSize(font.size);
	m_fonts[font.id]->setMetrics(font);
	m_fonts[font.id]->setShadow(1);

	const auto size = m_fonts[font.id]->getTextSize(str);
	const auto line_count = m_fonts[font.id]->getLineCount();

	const auto text_color = g_text_colors.at(getColor(font.color ? font.color : color));

	uint32_t bg_color = m_bg_color;
	glm::vec2 padding = { 3.4f, font.size * 0.05f * line_count };

	static float lh_diff = App.api == Api::Glide ? 0.0f : 0.08f;
	glm::vec2 back_pos = { (float)x + 3.2f, (float)y - size.y + 1.6f / line_count };
	glm::vec2 text_pos = { back_pos.x, back_pos.y + font.size - font.size * lh_diff };

	if (rect_transparency == 2) {
		padding = { 10.0f, 6.0f };
		m_object_bg->setColor(m_border_color, 2);
		m_object_bg->setFlags(2, 2);
		m_object_bg->setExtra(size + padding * 2.0f);
	} else {
		if (rect_transparency == 1)
			bg_color = m_alt_bg_color;
		else if (rect_transparency == 5)
			bg_color = m_alt_bg_hovered;

		m_object_bg->setFlags(2);
		if (d2::isLangCJK(m_lang_id))
			text_pos.y += 3.0f;
	}

	m_object_bg->setPosition(back_pos - padding);
	m_object_bg->setSize(size + padding * 2.0f);
	m_object_bg->setColor(bg_color, 1);
	App.context->pushObject(m_object_bg);

	m_fonts[font.id]->setBoxed(true);
	m_fonts[font.id]->setMasking(false);
	m_fonts[font.id]->setAlign(TextAlign::Center);
	m_fonts[font.id]->drawText(str, text_pos, text_color);

	return true;
}

bool HDText::drawSolidRect(int left, int top, int right, int bottom, uint32_t color, int draw_mode)
{
	if (App.game.screen != GameScreen::InGame || !isActive())
		return false;

	if (isVerMax(V_110)) {
		static bool monster_hp = false;
		const auto unit = d2::getSelectedUnit();
		if (unit && unit->dwType == d2::UnitType::Monster && top == 18 && bottom == 34) {
			if (!monster_hp) {
				drawMonsterHealthBar(unit);
				monster_hp = true;
			}
			return true;
		}
		monster_hp = false;
	}

	const int width = right - left;
	const int height = bottom - top;

	// if (draw_mode == 1)
	// trace("[] | %d | %d | %dx%d | %dx%d", color, draw_mode, left, top, width, height);

	if (height == g_text_size[m_lang_id].hp_bar && m_hovered_player_id && m_text_size == 1 && (color == 0 || color == 261)) {
		if (m_hovered_player_hp1 == 0) {
			m_hovered_player_hp1 = width;
			m_hovered_player_pos = { left, top };
		} else
			m_hovered_player_hp2 = width;
		return true;
	}

	if (color != 0) // skip drawing except black color
		return false;

	if (draw_mode == 5 && height == 5 && top == 14) // hireling & summon hp
		return false;

	if (right - left == App.game.size.x || bottom - top == App.game.size.y) // FreeRes black bars
		return false;

	if ((*d2::screen_shift == SCREENPANEL_LEFT || *d2::screen_shift == SCREENPANEL_BOTH) && width == 320 && (height == 432 || height == 236)) // Plugy stats panel bg
		return false;

	if (draw_mode == 2 && width == 24 && height <= 24) // PD2 buff timer bg
		return false;

	if (*d2::esc_menu_open && height == 30) // In-game option sliders
		return false;

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

	const auto font = getFont(m_text_size);
	m_fonts[font.id]->setSize(font.size);
	m_fonts[font.id]->setMetrics(font);

	const auto size = m_fonts[font.id]->getTextSize(str, n_chars);
	m_last_text_width = (uint32_t)size.x;

	return (uint32_t)size.x;
}

uint32_t HDText::getFramedTextSize(const wchar_t* str, uint32_t* width, uint32_t* height)
{
	const auto font = getFont(m_text_size);
	m_fonts[font.id]->setSize(font.size);
	m_fonts[font.id]->setMetrics(font);

	const auto size = m_fonts[font.id]->getTextSize(str);
	*width = (uint32_t)size.x;
	*height = (uint32_t)(size.y * 1.11888f);
	m_last_text_width = *width;
	m_last_text_height = *height;

	return *height;
}

uint16_t HDText::getFontHeight()
{
	if (App.game.screen != GameScreen::Menu) {
		const auto font = getFont(m_text_size);
		return (uint16_t)font.size;
	}

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
		str = *(const wchar_t**)(ptr + (isVerMax(V_110) ? 0x20 : 0x68));
		color = (uint32_t*)(ptr + (isVerMax(V_110) ? 0x78 : 0x74));
		length = (int*)(ptr + (isVerMax(V_110) ? 0x1C : 0x10));
		x = (int*)(ptr + (isVerMax(V_110) ? 0x18 : 0x18));
		y = (int*)(ptr + (isVerMax(V_110) ? 0x70 : 0x6C));
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

	if (m_hovered_player_id) {
		drawPlayerHealthBar(isVer(V_114d) ? m_hovered_player_str : str, *color);
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

	if (App.game.screen == GameScreen::Menu) {
		const auto cell_file = d2::getCellFile(cell);
		if (!cell_file)
			return true;

		if ((cell_file->numcells == 1 || cell_file->numcells == 2 || cell_file->numcells == 4) && d2::getCellNo(cell) == 0) {
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
		}
	} else if (App.game.draw_stage == DrawStage::UI && d2::isEscMenuOpen()) {
		const auto cell_file = d2::getCellFile(cell);
		if (!cell_file)
			return true;

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
					setTextSize(cell->height == g_text_size[0].large_text.x || cell->height == g_text_size[m_lang_id].large_text.x ? 3 : 2);

					if (p.align == TextAlign::Center) {
						auto text_width = getNormalTextWidth(p.str, 0);
						pos.x = pos.x + image_width / 2 - text_width / 2;
					} else if (p.align == TextAlign::Right) {
						auto text_width = getNormalTextWidth(p.str, 0);
						pos.x = pos.x + image_width - text_width;
					}
					drawText(p.str, pos.x, pos.y, draw_mode == 1 ? 16 : 0, 0);
				}
				return false;
			}
		}
	}

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
					setTextSize(3);

					auto text_width = getNormalTextWidth(p.str, 0);
					pos.x = pos.x + image_width / 2 - text_width / 2;
					drawText(p.str, pos.x, pos.y, 17, 0);
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

	rect->left = rect->top = rect->right = rect->bottom = -10;
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
	if (!m_entry_text_draw)
		return;

	if (m_entry_timer < std::clock())
		m_entry_text_draw = false;

	if (App.game.screen != GameScreen::InGame)
		return;

	setTextSize(2);
	const auto level_name = d2::getLevelName(*d2::level_no);
	const auto text_width = getNormalTextWidth(level_name, 0);
	drawText(level_name, *d2::screen_width / 2 - text_width / 2, (uint32_t)((float)*d2::screen_height / 4.5f), 17, 0);
}

void HDText::drawMonsterHealthBar(d2::UnitAny* unit)
{
	const auto name = d2::getMonsterName(unit);
	if (!name)
		return;

	const auto hp = d2::getUnitStat(unit, 6);
	const auto max_hp = d2::getUnitStat(unit, 7);
	const auto type = d2::getMonsterType(unit);

	auto font = getFont(1);
	m_fonts[font.id]->setSize(font.size);
	m_fonts[font.id]->setMetrics(font);
	m_fonts[font.id]->setShadow(2);
	m_fonts[font.id]->setBoxed(false);
	m_fonts[font.id]->setMasking(false);
	m_fonts[font.id]->setAlign(TextAlign::Center);

	float center = (float)(*d2::screen_width / 2);
	if (*d2::screen_shift == SCREENPANEL_RIGHT)
		center = (float)(*d2::screen_width / 4);
	else if (*d2::screen_shift == SCREENPANEL_LEFT)
		center = (float)(*d2::screen_width / 4 * 3);

	const auto text_size = m_fonts[font.id]->getTextSize(name);
	float hp_percent = (float)hp / (float)max_hp;

	glm::vec2 bar_size = { 120.0f, 18.0f };
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
	if (hp == 0)
		text_color = L'\x31';

	glm::vec2 text_pos = { center - text_size.x / 2, App.api == Api::Glide ? 33.2f : 32.5f };
	if (d2::isLangCJK(m_lang_id))
		text_pos.y += 1.0f;
	m_fonts[font.id]->drawText(name, text_pos, g_text_colors.at(text_color));
}

void HDText::drawPlayerHealthBar(const wchar_t* name, uint32_t color)
{
	const uint32_t hp = m_hovered_player_hp1;
	const uint32_t max_hp = m_hovered_player_hp1 + m_hovered_player_hp2;

	auto font = getFont(1);
	m_fonts[font.id]->setSize(font.size);
	m_fonts[font.id]->setMetrics(font);
	m_fonts[font.id]->setShadow(2);
	m_fonts[font.id]->setBoxed(false);
	m_fonts[font.id]->setMasking(false);
	m_fonts[font.id]->setAlign(TextAlign::Center);

	const auto text_size = m_fonts[font.id]->getTextSize(name);
	float hp_percent = (float)hp / (float)max_hp;

	glm::vec2 bar_size = { 60.0f, 16.0f };
	if (text_size.x + 20.0f > bar_size.x)
		bar_size.x = text_size.x + 20.0f;

	auto offset = modules::MotionPrediction::Instance().getUnitOffset(m_hovered_player_id);
	const float center = (float)(m_hovered_player_pos.x + max_hp / 2) + (float)offset.x;
	glm::vec2 bar_pos = { center - bar_size.x / 2, (float)m_hovered_player_pos.y + (float)offset.y };

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

	glm::vec2 text_pos = { center - text_size.x / 2, bar_pos.y + (App.api == Api::Glide ? 14.4f : 13.8f) };
	m_fonts[font.id]->drawText(name, text_pos, g_text_colors.at(getColor(color)));

	m_hovered_player_id = 0;
	m_hovered_player_hp1 = m_hovered_player_hp2 = 0;
}

inline const D2FontInfo& HDText::getFont(uint32_t size)
{
	return g_font_sizes.find(size) != g_font_sizes.end() ? g_font_sizes.at(size) : g_font_sizes.at(0);
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

	d2::setTextSizeHooked(99);
	const auto width = d2::getNormalTextWidthHooked(str);
	d2::drawNormalTextHooked(str, App.game.size.x / 2 - width / 2, App.game.size.y - 54, 4, 0);
	d2::setTextSizeHooked(1);
}

}