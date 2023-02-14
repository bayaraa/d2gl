#pragma once

#include "font.h"

namespace d2gl::modules {

enum class FramedTextType {
	Default,
	InvItem,
	Skill,
	Monster,
};

struct D2FontInfo {
	uint8_t id;
	float size;
	float weight;
	wchar_t color = 0;
};

struct D2PopupInfo {
	glm::vec<2, uint16_t> size0;
	glm::vec<2, uint16_t> size1;
};

struct D2TextInfo {
	uint8_t cell_num;
	glm::vec<2, uint16_t> size;
	TextAlign align;
	const wchar_t* str;
};

class HDText {
private:
	std::map<uint8_t, std::unique_ptr<Font>> m_fonts;
	std::unique_ptr<Object> m_object_bg;
	uint32_t m_text_size = 1;

	bool m_bordered_rect = false;
	bool m_draw_sub_text = true;
	bool m_is_player_dead = false;

	glm::vec4 m_text_mask = glm::vec4(0.0f);

	const uint32_t m_bg_color = 0x000000CC;
	const uint32_t m_border_color = 0x222222DD;
	const uint32_t m_alt_bg_color = 0x00000099;
	const uint32_t m_alt_bg_hovered = 0x213F98DD;

	HDText();

public:
	static HDText& Instance()
	{
		static HDText instance;
		return instance;
	}

	inline bool isActive() { return App.hd_cursor && App.hd_text; }

	void reset();
	void update(std::unique_ptr<Pipeline>& shader);

	bool drawText(const wchar_t* str, int x, int y, uint32_t color, uint32_t centered);
	bool drawFramedText(const wchar_t* str, int x, int y, uint32_t color, uint32_t centered);
	bool drawRectangledText(const wchar_t* str, int x, int y, uint32_t rect_color, uint32_t rect_transparency, uint32_t color);
	bool drawSolidRect(int left, int top, int right, int bottom, uint32_t color, int draw_mode);

	uint32_t getNormalTextWidth(const wchar_t* str, const int n_chars);
	uint32_t getFramedTextSize(const wchar_t* str, uint32_t* width, uint32_t* height);
	uint16_t getFontHeight();

	inline void setTextSize(uint32_t size) { m_text_size = size; }
	inline void borderedRect(bool draw = true) { m_bordered_rect = draw; }

	void drawSubText(uint8_t fn = 1);
	// bool drawImage(d2::CellContext* pData, int nXpos, int nYpos, DWORD dwGamma, int nDrawMode);
	// bool drawShiftedImage(d2::CellContext* pData, int nXpos, int nYpos, DWORD dwGamma, int nDrawMode, int nGlobalPaletteShift);
	// void loadUIImage();

	void drawTest();

private:
	inline wchar_t getColor(uint32_t color);
	inline const D2FontInfo& getFont(uint32_t size);
};

void drawText(glm::vec2 position, uint32_t color);
void drawText2();
void drawSubText(uint8_t fn);

}