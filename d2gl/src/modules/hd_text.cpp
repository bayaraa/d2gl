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
#include "font.h"

namespace d2gl::modules {

// clang-format off
const std::map<uint32_t, D2FontInfo> g_font_sizes = {
	{  1, { 0, 14.3f, 1.02f, 1.1f, -0.022f, } }, // All Normal texts
	{  9, { 0, 16.4f, 1.10f, 1.1f, -0.022f, L'\x02' } }, // Menu Button label
	{ 10, { 0, 12.0f, 1.14f, 1.1f, -0.022f, L'\x02' } }, // Menu Button label (smaller)
	{ 11, { 0, 12.2f, 1.06f, 1.1f, -0.022f, } }, // ?
	{ 99, { 0, 11.2f, 1.00f, 1.1f, -0.022f, } }, // FramedText in small size

	{  0, { 1, 10.8f, 1.14f, 1.0f,  0.115f } }, // Char name on stats panel
	{  4, { 1, 10.2f, 1.14f, 1.0f,  0.115f } }, // Skill tree level number 2digits
	{  5, { 1, 14.8f, 1.14f, 1.0f,  0.115f } }, // Menu screen copyright text
	{  6, { 1,  8.8f, 1.14f, 1.0f,  0.115f } }, // All Small texts
	{  8, { 1, 12.4f, 1.14f, 1.0f,  0.115f } }, // Talking Text
	{ 12, { 1,  8.8f, 1.14f, 1.0f,  0.115f } }, // ?
	{ 13, { 1, 11.6f, 1.14f, 1.0f,  0.090f } }, // Message, Shrine, Keybind config

	{  2, { 2, 27.0f, 1.10f, 1.0f, -0.022f, L'\x01' } }, // Char create screen title, Resolution text eg:800x600
	{  3, { 2, 36.0f, 1.10f, 1.0f, -0.022f, L'\x01' } }, // Char selection title (selected char name)
	{  7, { 2, 20.9f, 1.10f, 1.0f, -0.022f, L'\x01' } }, // Menu screen popup title
};

const std::vector<D2PopupInfo> g_popups = {
	{ { 256, 256 }, {  70,  85 } }, // CinematicsSelection.dc6
	{ { 256, 256 }, {  70, 171 } }, // CinematicsSelectionEXP.dc6
	{ { 256, 256 }, {  84,  84 } }, // GWListBox.dc6 // PopUpLarge.dc6
	{ { 256, 224 }, {  84, 224 } }, // PopUp_340x224.dc6
	{ { 256, 256 }, { 229,  34 } }, // PopUpLargest.dc6 // PopUpLargest2.dc6
	{ { 256, 245 }, { 154, 245 } }, // PopupWide.dc6
	{ { 256, 176 }, {   8, 176 } }, // popupok.dc6 // passwordbox.dc6 // PopUpOKCancel.DC6
	{ { 256, 200 }, {  70, 200 } }, // DifficultyLevels.dc6
};

const std::vector<D2TextInfo> g_options_texts = {
	{ 0, { 160, 36 }, TextAlign::Center, L"OPTIONS" },
	{ 1, { 178, 36 }, TextAlign::Center, L"SAVE AND EXIT GAME" },
	{ 1, { 100, 36 }, TextAlign::Center, L"RETURN TO GAME" },
	{ 1, {  57, 36 }, TextAlign::Center, L"PREVIOUS MENU" },
	{ 0, { 231, 26 }, TextAlign::Center, L"PREVIOUS MENU" },
	{ 0, {  41, 26 }, TextAlign::Right,  L"ON" },
	{ 0, {  52, 26 }, TextAlign::Right,  L"OFF" },
	{ 0, {  51, 26 }, TextAlign::Right,  L"YES" },
	{ 0, {  40, 26 }, TextAlign::Right,  L"NO" },
	{ 1, {  53, 36 }, TextAlign::Center, L"SOUND OPTIONS" },
	{ 0, {  95, 26 }, TextAlign::Left,   L"SOUND" },
	{ 0, {  84, 26 }, TextAlign::Left,   L"MUSIC" },
	{ 0, { 144, 26 }, TextAlign::Left,   L"3D SOUND" },
	{ 1, { 106, 26 }, TextAlign::Left,   L"ENVIRONMENTAL EFFECTS" },
	{ 0, { 109, 26 }, TextAlign::Right,  L"3D BIAS" },
	{ 0, { 157, 26 }, TextAlign::Left,   L"NPC SPEECH" },
	{ 1, {   4, 26 }, TextAlign::Right,  L"AUDIO AND TEXT" },
	{ 0, { 184, 26 }, TextAlign::Right,  L"AUDIO ONLY" },
	{ 0, { 169, 26 }, TextAlign::Right,  L"TEXT ONLY" },
	{ 1, {  39, 36 }, TextAlign::Center, L"VIDEO OPTIONS" },
	{ 0, { 175, 26 }, TextAlign::Left,   L"RESOUTION" },
	{ 0, { 135, 26 }, TextAlign::Right,  L"800x600" },
	{ 0, { 123, 26 }, TextAlign::Right,  L"640x480" },
	{ 1, {   9, 26 }, TextAlign::Left,   L"LIGHTING QUALITY" },
	{ 0, {  57, 26 }, TextAlign::Right,  L"HIGH" },
	{ 0, { 108, 26 }, TextAlign::Right,  L"MEDIUM" },
	{ 0, {  66, 26 }, TextAlign::Right,  L"LOW" },
	{ 1, {  10, 26 }, TextAlign::Left,   L"BLENDED SHADOWS" },
	{ 0, { 174, 26 }, TextAlign::Left,   L"PERSPECTIVE" },
	{ 0, { 105, 26 }, TextAlign::Left,   L"GAMMA" },
	{ 0, { 151, 26 }, TextAlign::Left,   L"CONTRAST" },
	{ 1, { 122, 36 }, TextAlign::Center, L"AUTOMAP OPTIONS" },
	{ 0, { 213, 26 }, TextAlign::Left,   L"AUTOMAP SIZE" },
	{ 0, { 180, 26 }, TextAlign::Right,  L"FULL SCREEN" },
	{ 0, { 131, 26 }, TextAlign::Right,  L"MINI MAP" },
	{ 0, {  70, 26 }, TextAlign::Left,   L"FADE" },
	{ 0, { 173, 26 }, TextAlign::Right,  L"EVERYTHING" },
	{ 0, { 104, 26 }, TextAlign::Right,  L"CENTER" },
	{ 0, {  86, 26 }, TextAlign::Right,  L"AUTO" },
	{ 1, {  80, 26 }, TextAlign::Left,   L"CENTER WHEN CLEARED" },
	{ 0, { 194, 26 }, TextAlign::Left,   L"SHOW PARTY" },
	{ 0, { 190, 26 }, TextAlign::Left,   L"SHOW NAMES" },
	{ 1, { 167, 36 }, TextAlign::Center, L"CONFIGURE CONTROLS" },
};

const std::vector<D2TextInfo> g_death_texts = {
	{ 1, {  20, 36 }, TextAlign::Center, L"You have died" },
	{ 1, { 175, 36 }, TextAlign::Center, L"Press ESC to continue." },
	{ 1, {  23, 36 }, TextAlign::Center, L"You lost gold" },
};
// clang-format on

HDText::HDText()
{
	m_object_bg = std::make_unique<Object>();

	FontCreateInfo font_ci1 = { "ExocetBlizzardMedium", 0.022f, { 0.00f, App.api == Api::Glide ? -0.20f : -0.12f } };
	FontCreateInfo font_ci2 = { "Formal436BT", 0.012f, { 0.01f, App.api == Api::Glide ? -0.28f : -0.15f } };
	FontCreateInfo font_ci3 = { "ExocetReaperMedium", 0.0f, { 0.0f, App.api == Api::Glide ? -0.14f : -0.10f } };

	m_fonts[0] = std::make_unique<Font>(font_ci1);
	m_fonts[1] = std::make_unique<Font>(font_ci2);
	m_fonts[2] = std::make_unique<Font>(font_ci3);
}

void HDText::reset()
{
	m_masking = false;
	m_is_player_dead = d2::isUnitDead(d2::getPlayerUnit());
	m_map_text_line = 1;
}

void HDText::update(const std::unique_ptr<Pipeline>& pipeline)
{
	static bool mask = false;
	if (m_masking != mask) {
		pipeline->setUniformVec4f("u_TextMask", m_text_mask);
		pipeline->setUniform1i("u_IsMasking", m_masking);
		mask = m_masking;
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

	glm::vec2 pos = { (float)x, (float)y };
	uint32_t text_color = g_text_colors.at(!color && font.color ? font.color : getColor(color));

	if (App.game.screen == GameScreen::Menu) {
		switch (m_text_size) {
			case 1:
				if (x == 113 || x == 385) {
					pos.y += isVer(V_109d) ? -5.0f : 8.0f;
					if (color == 0 || color == 2) {
						font = getFont(6);
						m_fonts[font.id]->setSize(font.size);
						m_fonts[font.id]->setMetrics(font);
						pos.x += 2.0f;
					}
				}
				break;
			case 3: pos.y -= font.size * 0.18f; break;
			case 7: pos.y -= font.size * 0.16f; break;
			case 9: pos.y -= font.size * 0.06f; break;
			case 10: pos.y += font.size * 0.06f; break;
		}
	} else {
		if (App.api == Api::Glide && (m_text_size == 13 || m_text_size == 0))
			pos.y += font.size * 0.08f;
	}

	static bool map_text = false;
	if (App.mini_map.active && App.hd_cursor && App.game.draw_stage == DrawStage::Map) {
		if (m_text_size == 6 && !*d2::automap_on)
			return true;
		else if (m_text_size != 6) {
			if (*d2::screen_shift != 0)
				return true;

			map_text = true;
			App.context->toggleDelayPush(true);

			font = getFont(6);
			m_fonts[font.id]->setSize(font.size);
			m_fonts[font.id]->setMetrics(font);
			const auto size = m_fonts[font.id]->getTextSize(str);
			pos.x = App.game.size.x - 12.0f - size.x;
			pos.y = 9.0f + m_map_text_line * font.size * 1.3f;
			m_map_text_line++;
		}
	}

	m_fonts[font.id]->setBoxed(false);
	m_fonts[font.id]->setMasking(m_masking);
	m_fonts[font.id]->setAlign(centered ? TextAlign::Center : TextAlign::Left);
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

	if (isVerMin(V_111)) {
		const auto unit = d2::getSelectedUnit();
		if (unit && unit->dwType == d2::UnitType::Monster && y == 32 && !centered) {
			drawMonsterHealthBar(unit);
			return true;
		}
	}

	auto font = getFont(1);
	m_fonts[font.id]->setSize(font.size);
	auto size = m_fonts[font.id]->getTextSize(str);
	if (size.y > (float)App.game.size.y - 40.0f) {
		font = getFont(99);
		m_fonts[font.id]->setSize(font.size);
		size = m_fonts[font.id]->getTextSize(str);
	}

	m_fonts[font.id]->setMetrics(font);
	const uint32_t text_color = g_text_colors.at(font.color ? font.color : getColor(color));
	const int line_count = m_fonts[font.id]->getLineCount();

	const glm::vec2 padding = { 10.0f, 5.0f };
	glm::vec2 pos = { (float)x - padding.x, (float)y - size.y - padding.y };
	glm::vec2 box_size = size + padding * 2.0f;

	if (centered)
		pos.x -= size.x / 2.0f;
	else if (line_count > 1)
		pos.x -= size.x / 2.0f - 24.0f;

	const int mid_y = (int)(pos.y + box_size.y / 2);
	const int cursor_y = d2::getCursorPos().y;
	if (mid_y > cursor_y)
		pos.y = (float)(y - m_last_text_height);
	else
		pos.y = (float)y - box_size.y;

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

	const auto item = d2::getSelectedItem();
	if (item && item->dwType == d2::UnitType::Item) {
		const auto quality = d2::getItemQuality(item);
		uint32_t border_color = m_border_color;
		switch (quality) {
			case d2::ItemQuality::Unique: border_color = 0x2b2215DD; break;
			case d2::ItemQuality::Set: border_color = 0x1c3418DD; break;
			case d2::ItemQuality::Rare: border_color = 0x31311eDD; break;
			case d2::ItemQuality::Craft: border_color = 0x2f2102DD; break;
			case d2::ItemQuality::Magic: border_color = 0x1d1d31DD; break;
		}
		m_object_bg->setColor(border_color, 2);
	} else
		m_object_bg->setColor(m_border_color, 2);

	m_object_bg->setPosition(pos);
	m_object_bg->setSize(box_size);
	m_object_bg->setColor(m_bg_color, 1);
	m_object_bg->setFlags({ 2, 2, 0, 0 });
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

bool HDText::drawRectangledText(const wchar_t* str, int x, int y, uint32_t rect_color, uint32_t rect_transparency, uint32_t color) // TODO: remove rect_color
{
	if (!isActive() || !str)
		return false;

	// rectColor = 0, rectTransparency = 2, dwColor = 0   stats panel popup
	// rectColor = 0, rectTransparency = 1, dwColor = 0   alt item
	// rectColor = 140, rectTransparency = 5, dwColor = 0   alt item hovered

	const auto font = getFont(m_text_size);
	m_fonts[font.id]->setSize(font.size);
	m_fonts[font.id]->setMetrics(font);

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
		m_object_bg->setFlags({ 2, 2, 0, 0 });
		m_object_bg->setExtra(size + padding * 2.0f);
	} else {
		if (rect_transparency == 1)
			bg_color = m_alt_bg_color;
		else if (rect_transparency == 5)
			bg_color = m_alt_bg_hovered;

		m_object_bg->setFlags({ 2, 0, 0, 0 });
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
	if (!isActive())
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

	if (color != 0)
		return false;

	const int width = right - left;
	const int height = bottom - top;

	if (draw_mode == 5 && height == 5 && top == 14) // hireling & summon hp
		return false;

	if (right - left == App.game.size.x || bottom - top == App.game.size.y) // FreeRes black bars
		return false;

	if ((*d2::screen_shift == 2 || *d2::screen_shift == 3) && width == 320 && height == 432) // Plugy stats panel bg
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
	m_object_bg->setFlags({ 2, 0, 0, 0 });
	App.context->pushObject(m_object_bg);

	return true;
}

uint32_t HDText::getNormalTextWidth(const wchar_t* str, const int n_chars)
{
	const auto font = getFont(m_text_size);
	m_fonts[font.id]->setSize(font.size);
	m_fonts[font.id]->setMetrics(font);

	const auto size = m_fonts[font.id]->getTextSize(str, n_chars);

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

	if (str) {
		drawText(str, *x, *y, *color, 1);
		*length = 0;
	}
}

bool HDText::drawImage(d2::CellContext* cell, int x, int y, uint32_t gamma, int draw_mode) // TODO: remove gamma
{
	if (!isActive() || !cell)
		return true;

	if (App.game.screen == GameScreen::Menu) {
		const auto cell_file = d2::getCellFile(cell);
		if (!cell_file)
			return true;

		if ((cell_file->numcells == 2 || cell_file->numcells == 4) && d2::getCellNo(cell) == 0) {
			const auto cell0 = cell_file->cells[0];
			if (cell0->width != 256)
				return true;

			const auto cell1 = cell_file->cells[cell_file->numcells - 1];
			for (auto& p : g_popups) {
				if (p.size0.y == cell0->height && p.size1.x == cell1->width && p.size1.y == cell1->height) {
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
		if (cell_file->cells[0]->height != 26 && cell_file->cells[0]->height != 36)
			return true;

		for (auto& p : g_options_texts) {
			const auto cell = cell_file->cells[p.cell_num];
			if (p.cell_num == cell_file->numcells - 1 && cell->width == p.size.x && cell->height == p.size.y) {
				if (cell_no == 0) {
					auto image_width = cell->width + (p.cell_num > 0 ? cell_file->cells[0]->width : 0);
					glm::ivec2 pos = { x, y };
					setTextSize(cell->height == 26 ? 2 : 3);

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

bool HDText::drawShiftedImage(d2::CellContext* cell, int x, int y, uint32_t gamma, int draw_mode) // TODO: remove gamma, draw_mode
{
	if (!isActive())
		return true;

	if (App.game.draw_stage == DrawStage::HUD && m_is_player_dead) {
		const auto cell_file = d2::getCellFile(cell);
		if (!cell_file)
			return true;

		const auto cell_no = d2::getCellNo(cell);
		if (cell_file->cells[0]->height != 36)
			return true;

		for (auto& p : g_death_texts) {
			const auto cell = cell_file->cells[p.cell_num];
			if (p.cell_num == cell_file->numcells - 1 && cell->width == p.size.x && cell->height == p.size.y) {
				if (cell_no == 0) {
					auto image_width = cell->width + (p.cell_num > 0 ? cell_file->cells[0]->width : 0);
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

	if (m_entry_text && y == 160)
		return false;
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
	m_object_bg->setFlags({ 4, 3, 0, 0 });
	m_object_bg->setExtra(size);
	App.context->pushObject(m_object_bg);

	rect->left = rect->top = rect->right = rect->bottom = -10;
}

void HDText::loadUIImage()
{
	if (d2::ui_image_path) {
		size_t len = strlen(d2::ui_image_path);
		std::string path(d2::ui_image_path);
		helpers::strToLower(path);

		if (path.find("ui\\eng") != std::string::npos) {
			if (path.find("3dbias") != std::string::npos ||
			  path.find("perspective") != std::string::npos ||
			  path.find("resolution") != std::string::npos ||
			  path.find("smalloff") != std::string::npos ||
			  path.find("smallon") != std::string::npos) {
				((char*)d2::ui_image_path)[len - 2] = '_';
				((char*)d2::ui_image_path)[len - 1] = 'n';
			}
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

	const auto level_name = d2::getLevelName(*d2::level_no);
	std::wstring text = L"Entering ";
	if (wcsncmp(level_name, L"The ", 4) != 0)
		text += L"The ";
	text += level_name;

	setTextSize(2);
	auto text_width = getNormalTextWidth(text.c_str(), 0);
	drawText(text.c_str(), *d2::screen_width / 2 - text_width / 2, 160, 17, 0);
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
	m_fonts[font.id]->setBoxed(false);
	m_fonts[font.id]->setMasking(false);
	m_fonts[font.id]->setAlign(TextAlign::Center);

	float center = (float)(*d2::screen_width / 2);
	if (*d2::screen_shift == 1)
		center = (float)(*d2::screen_width / 4);
	else if (*d2::screen_shift == 2)
		center = (float)(*d2::screen_width / 4 * 3);

	const auto text_size = m_fonts[font.id]->getTextSize(name);
	float hp_percent = (float)hp / (float)max_hp;

	glm::vec2 bar_size = { 120.0f, 18.0f };
	if (text_size.x + 40.0f > bar_size.x)
		bar_size.x = text_size.x + 40.0f;

	m_object_bg->setFlags({ 2, 0, 0, 0 });
	glm::vec2 bar_pos = { center - bar_size.x / 2, 18.0f };

	m_object_bg->setPosition(bar_pos);
	m_object_bg->setSize({ bar_size.x * hp_percent, bar_size.y });
	m_object_bg->setColor(m_monster_hp, 1);
	App.context->pushObject(m_object_bg);

	m_object_bg->setPosition({ bar_pos.x + bar_size.x * hp_percent, bar_pos.y });
	m_object_bg->setSize({ bar_size.x * (1.0f - hp_percent), bar_size.y });
	m_object_bg->setColor(m_bg_color, 1);
	App.context->pushObject(m_object_bg);

	wchar_t text_color = L'\x30';
	if (type == d2::MonsterType::Boss || type == d2::MonsterType::SuperUnique)
		text_color = L'\x34';
	else if (type == d2::MonsterType::Champion)
		text_color = L'\x33';
	if (hp == 0)
		text_color = L'\x31';

	glm::vec2 text_pos = { center - text_size.x / 2, App.api == Api::Glide ? 33.2f : 32.5f };
	m_fonts[font.id]->drawText(name, text_pos, g_text_colors.at(text_color));
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
	const float fps = round(1000.0f / App.context->getAvgFrameTime());
	swprintf_s(str, L"FPS: %.0f", fps);

	d2::setTextSizeHooked(App.hd_text ? 99 : 11);
	const auto width = d2::getNormalTextWidthHooked(str);
	d2::drawNormalTextHooked(str, App.game.size.x / 2 - width / 2, App.game.size.y - 54, 4, 0);
	d2::setTextSizeHooked(1);
}

}