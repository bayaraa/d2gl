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

#include <imgui/imgui.h>

namespace d2gl::option {

enum class Color {
	Default,
	Orange,
	White,
	Gray,
};

struct Options {
	bool vsync = false;
	D2GLApp::Window window;
	D2GLApp::ForegroundFPS foreground_fps;
	D2GLApp::BackgroundFPS background_fps;
	bool pos_changed = false;
};

class Menu {
	bool m_visible = false;
	bool m_closing = false;
	bool m_changed = false;
	bool m_opt_changed = false;
	std::unordered_map<int, ImFont*> m_fonts;
	std::unordered_map<Color, ImVec4> m_colors;
	Options m_options;
	bool m_ignore_font = false;

	Menu();
	~Menu() = default;

	bool tabBegin(const char* title, int tab_num, int* active_tab);
	void tabEnd();

	void childBegin(const char* id, bool half_width = false, bool with_nav = false);
	void childSeparator(const char* id, bool with_nav = false);
	void childEnd();
	bool drawNav(const char* btn_label);

	template <typename T>
	bool drawCombo(const char* title, Select<T>* select, const char* desc, bool have_btn, int* opt, int size = 17);
	bool drawCheckbox(const char* title, bool* option, const char* desc, bool* opt);
	template <typename T>
	bool drawSlider(const std::string& id, const char* title, Range<T>* range, const char* format, const char* desc, T* opt);
	void drawInput2(const std::string& id, const char* desc, glm::ivec2* input, glm::ivec2 min = { 0, 0 }, glm::ivec2 max = { 10000, 10000 });

	bool drawButton(const char* label, const ImVec2& btn_size = { 0.0f, 0.0f }, int size = 17);
	void drawSeparator(float y_padd = 5.0f, float alpha = 1.0f);
	void drawLabel(const char* title, const ImVec4& color, int size = 17);
	void drawDescription(const char* desc, const ImVec4& color, int size = 14);

public:
	static Menu& instance()
	{
		static Menu instance;
		return instance;
	}

	void toggle(bool force = false);
	void check();
	void draw();

	inline bool isVisible() { return m_visible; }
	inline Options* getOption() { return &m_options; }
};

}