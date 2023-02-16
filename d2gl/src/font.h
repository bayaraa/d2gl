#pragma once

namespace d2gl {

enum class TextAlign {
	Left,
	Right,
	Center,
};

struct D2FontInfo {
	uint8_t id;
	float size;
	float weight;
	float line_height;
	float letter_spacing;
	wchar_t color = 0;
};

struct Glyph {
	glm::vec2 size = { 0.0f, 0.0f };
	glm::vec2 offset = { 0.0f, 0.0f };
	glm::vec4 tex_coord = { 0.0f, 0.0f, 0.0f, 0.0f };
	float advance = 0.0f;
};

struct FontCreateInfo {
	std::string path;
	float rtrim_size = 0.0f;
	glm::vec2 offset = { 0.0f, 0.0f };
};

extern const std::map<wchar_t, uint32_t> g_text_colors;
extern const std::array<wchar_t, 18> g_default_colors;

class Font {
	uint16_t m_sub_tex_id = 0;
	const float m_glyph_size = 64.0f;
	std::map<wchar_t, Glyph> m_glyphes;
	std::unique_ptr<Object> m_object;

	float m_rtrim_size;
	glm::vec2 m_offset;

	float m_size = 10.0f;
	float m_scale = 1.0f;
	float m_weight = 1.0f;
	float m_line_height = 1.0f;
	float m_letter_spacing = 0.0f;
	float m_smoothness = 5.0f;
	TextAlign m_align = TextAlign::Left;

	bool m_boxed = false;
	bool m_masking = false;

	uint32_t m_line_count = 0;
	float m_line_width[100] = { 0.0f };
	glm::vec2 m_text_size = { 0.0f, 0.0f };

public:
	Font(const FontCreateInfo& info);
	~Font() = default;

	inline void setSize(float size) { m_size = size, m_scale = size / m_glyph_size, m_smoothness = size / 2.0f; }
	inline void setLineHeight(float line_height) { m_line_height = line_height; }
	inline void setLetterSpacing(float letter_spacing) { m_letter_spacing = letter_spacing; }
	inline void setWeight(float weight) { m_weight = weight; }
	inline void setMetrics(const D2FontInfo& font) { m_weight = font.weight, m_line_height = font.line_height, m_letter_spacing = font.letter_spacing; }

	inline void setAlign(TextAlign align) { m_align = align; }
	inline void setBoxed(bool boxed) { m_boxed = boxed; }
	inline void setMasking(bool masking) { m_masking = masking; }

	inline float getLineHeight() { return m_size * m_line_height; }
	inline float getLetterSpacing() { return m_size * m_letter_spacing; }
	inline glm::vec2 getTextOffset() { return m_size * m_offset; }
	inline int getLineCount() { return m_line_count + 1; }

	glm::vec2 getTextSize(const wchar_t* str, const int max_chars = 0);
	void drawText(const wchar_t* str, glm::vec2 pos, uint32_t color);

private:
	float drawChar(wchar_t c, glm::vec2 pos, uint32_t color);
};

void initFonts();
void destroyFonts();

}