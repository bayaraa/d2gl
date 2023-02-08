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
};

class Menu {
	bool m_visible = false;
	std::unordered_map<int, ImFont*> m_fonts;
	std::unordered_map<Color, ImVec4> m_colors;
	Options m_options;

	Menu();
	~Menu();

	bool tabBegin(const char* title, int tab_num, int* active_tab);
	void tabEnd();

	void childBegin(const char* id, bool half_width = false, bool with_nav = false);
	void childSeparator(const char* id, bool with_nav = false);
	void childEnd();
	bool drawNav(const char* btn_label);

	template <typename T>
	bool drawCombo(const char* title, Select<T>* select, const char* desc, const char* btn_label, int* opt);
	bool drawCheckbox(const char* title, bool* option, const char* desc, bool* opt);
	template <typename T>
	bool drawSlider(const std::string& id, const char* title, Range<T>* range, const char* format, const char* desc, T* opt);
	void drawInput2(const std::string& id, const char* desc, glm::ivec2* input, glm::ivec2 min = { 0, 0 }, glm::ivec2 max = { 10000, 10000 });

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
	void draw();

	inline bool isVisible() { return m_visible; }
	inline Options* getOption() { return &m_options; }
};

}