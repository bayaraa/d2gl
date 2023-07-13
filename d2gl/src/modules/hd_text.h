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

#include "d2/structs.h"
#include "hd_text/font.h"

namespace d2gl::modules {

enum class FramedTextType {
	Default,
	InvItem,
	Skill,
	Monster,
};

struct HoveredUnit {
	uint32_t id = 0;
	uint32_t hp[2] = { 0 };
	glm::ivec2 pos = { 0, 0 };
	wchar_t name[50] = { 0 };
	uint32_t color = 0;
};

class HDText {
	std::map<uint8_t, std::unique_ptr<Font>> m_fonts;
	std::unique_ptr<Object> m_object_bg;
	uint32_t m_lang_id = 0;
	uint32_t m_text_size = 1;
	uint32_t m_last_text_height = 0;
	uint32_t m_last_text_width = 0;
	uint32_t m_map_text_line = 1;
	bool m_map_names = false;

	bool m_bordered_rect = false;
	bool m_is_player_dead = false;

	glm::mat4 m_mvp = glm::mat4(0.0f);
	glm::vec4 m_text_mask = glm::vec4(0.0f);
	bool m_masking = false;

	bool m_entry_text = false;
	bool m_entry_text_draw = false;
	clock_t m_entry_timer = 0;
	int m_cur_level_no = 0;

	HoveredUnit m_hovered_unit;

	const uint32_t m_bg_color = 0x000000CC;
	const uint32_t m_border_color = 0x222222DD;
	const uint32_t m_alt_bg_color = 0x00000099;
	const uint32_t m_alt_bg_hovered = 0x213F98DD;
	const uint32_t m_monster_hp = 0x56110BDD;

	HDText();

public:
	static HDText& Instance()
	{
		static HDText instance;
		return instance;
	}

	inline bool isActive() { return App.hd_text; }
	inline void setMVP(const glm::mat4& mvp) { m_mvp = mvp; }

	void reset();
	void update();

	bool drawText(const wchar_t* str, int x, int y, uint32_t color, uint32_t centered);
	bool drawFramedText(const wchar_t* str, int x, int y, uint32_t color, uint32_t centered);
	bool drawRectangledText(const wchar_t* str, int x, int y, uint32_t rect_transparency, uint32_t color);
	bool drawSolidRect(int left, int top, int right, int bottom, uint32_t color, int draw_mode);

	uint32_t getNormalTextWidth(const wchar_t* str, const int n_chars);
	uint32_t getFramedTextSize(const wchar_t* str, uint32_t* width, uint32_t* height);
	uint16_t getFontHeight();

	inline void setTextSize(uint32_t size) { m_text_size = size; }
	inline uint32_t getTextSize() { return m_text_size; }
	inline void borderedRect(bool draw = true) { m_bordered_rect = draw; }
	inline Font* const getFont(uint32_t size) { return m_fonts[size].get(); }

	void drawSubText(uint8_t fn = 1);
	bool drawImage(d2::CellContext* cell, int x, int y, int draw_mode);
	bool drawShiftedImage(d2::CellContext* cell, int x, int y);
	void drawRectFrame();
	void loadUIImage();

	void drawUnitHealthBar();
	void startEntryText();
	void drawEntryText();

	static void drawFpsCounter();

private:
	void drawMonsterHealthBar(d2::UnitAny* unit);
	void drawPlayerHealthBar(d2::UnitAny* unit);

	inline wchar_t getColor(uint32_t color);

#ifdef _HDTEXT
public:
	static void showSampleText();
	const std::string& getAllFontMetricString();
#endif
};

}