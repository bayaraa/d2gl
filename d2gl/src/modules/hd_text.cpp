#include "pch.h"
#include "hd_text.h"
#include "d2/common.h"
#include "font.h"

namespace d2gl::modules {

// clang-format off
const std::map<uint32_t, D2FontInfo> g_font_sizes = {
	{  0, { 1, 10.8f, 1.14f } }, // Char name on stats panel
	{  1, { 0, 14.3f, 1.02f } }, // All Normal texts
	{  2, { 0, 27.0f, 1.06f, L'\x01' } }, // Char create screen title, Resolution text eg:800x600
	{  3, { 0, 36.0f, 1.06f, L'\x01' } }, // Char selection title (selected char name)
	{  4, { 1, 10.2f, 1.14f } }, // Skill tree level number 2digits
	{  5, { 1, 14.8f, 1.14f } }, // Menu screen copyright text
	{  6, { 1,  8.8f, 1.14f } }, // All Small texts
	{  7, { 0, 20.9f, 1.06f, L'\x01' } }, // Menu screen popup title
	{  8, { 1, 12.4f, 1.14f } }, // Talking Text
	{  9, { 0, 16.4f, 1.10f, L'\x02' } }, // Menu Button label
	{ 10, { 0, 12.0f, 1.14f, L'\x02' } }, // Menu Button label (smaller)
	{ 11, { 0, 12.2f, 1.06f } }, // ?
	{ 12, { 1,  8.8f, 1.14f } }, // ?
	{ 13, { 1, 11.6f, 1.14f } }, // Message, Shrine, Keybind config
	{ 99, { 0, 11.2f, 1.00f } }, // FramedText in small size
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

	FontCreateInfo font_ci1 = { "ExocetBlizzardMedium", 1.0f, 0.022f, -0.022f, { -0.08f, -0.16f } };
	FontCreateInfo font_ci2 = { "Formal436BT", 1.0f, 0.092f, 0.012f, { 0.01f, -0.25f } };
	m_fonts[0] = std::make_unique<Font>(font_ci1);
	m_fonts[1] = std::make_unique<Font>(font_ci2);
}

void HDText::reset()
{
	// IsMasking = false;

	// auto player = d2::GetPlayerUnit();
	// is_player_dead = player && (d2::GetUnitFlag(player) & 0x10000);
}

void HDText::update(std::unique_ptr<Pipeline>& shader)
{
	// static bool mask = false;
	// if (IsMasking != mask) {
	//	shader->SetUniformVec4f("u_TextMask", text_mask_);
	//	shader->SetUniform1i("u_IsMasking", IsMasking);
	//	mask = IsMasking;
	// }
}

bool HDText::drawText(const wchar_t* str, int x, int y, uint32_t color, uint32_t centered)
{
	if (!isActive() || !str)
		return false;

	glm::vec2 pos = { (float)x, (float)y };
	const auto font = getFont(m_text_size);
	uint32_t text_color = g_text_colors.at(!color && font.color ? font.color : getColor(color));
	// if (text_size_ == 11 || text_size_ == 12)
	//{
	//	trace("text: %S | %d", wStr, text_size_);
	//	color = 0xFF0000FF;
	// }
	if (App.game.screen == GameScreen::Menu) {
		switch (m_text_size) {
			case 3: pos.y -= font.size * 0.18f; break;
			case 7: pos.y -= font.size * 0.16f; break;
			case 9: pos.y -= font.size * 0.06f; break;
			case 10: pos.y += font.size * 0.06f; break;
		}
	}

	m_fonts[font.id]->setSize(font.size);
	m_fonts[font.id]->setWeight(font.weight);
	m_fonts[font.id]->setAlign(centered ? TextAlign::Center : TextAlign::Left);
	m_fonts[font.id]->setBoxed(false);
	m_fonts[font.id]->drawText(str, pos, text_color);

	return true;
}

bool HDText::drawFramedText(const wchar_t* str, int x, int y, uint32_t color, uint32_t centered)
{
	if (!isActive() || !str)
		return false;

	// auto hover_item = d2::GetHoveredInvItem();
	// if (hover_item && hover_item->dwType == d2::UnitType::Item)
	//{
	//	trace("item: %d", hover_item->pItemData->dwQuality);
	// }

	const float margin = 5.0f;
	const glm::vec2 padding = { 10.0f, 5.0f };

	auto font = getFont(1);
	const uint32_t text_color = g_text_colors.at(font.color ? font.color : getColor(color));

	m_fonts[font.id]->setSize(font.size);
	auto size = m_fonts[font.id]->getTextSize(str);
	if (size.y > (float)App.game.size.y - 40.0f) {
		font = getFont(99);
		m_fonts[font.id]->setSize(font.size);
		size = m_fonts[font.id]->getTextSize(str);
	}

	glm::vec2 pos = { (float)x - size.x / 2.0f - padding.x, (float)y - size.y - padding.y };
	glm::vec2 box_size = size + padding * 2.0f;

	if (pos.x + box_size.x > (float)App.game.size.x - margin)
		pos.x = (float)App.game.size.x - box_size.x - margin;
	else if (pos.x < margin)
		pos.x = margin;
	if (pos.y + box_size.y > (float)App.game.size.y - margin)
		pos.y = (float)App.game.size.y - box_size.y - margin;
	else if (pos.y < margin)
		pos.y = margin;

	// graphic::Renderer::Instance().ToggleDelayObject(true, true);
	m_object_bg->setPosition(pos);
	m_object_bg->setSize(box_size);
	m_object_bg->setColor(m_bg_color, 1);
	m_object_bg->setColor(m_border_color, 2);
	m_object_bg->setFlags({ 2, 2, 0, 0 });
	m_object_bg->setExtra(box_size);
	App.context->pushObject(m_object_bg);

	const auto line_height = m_fonts[font.id]->getLineHeight();
	pos.y += line_height - line_height * 0.1f;

	m_fonts[font.id]->setAlign(TextAlign::Center);
	m_fonts[font.id]->setWeight(font.weight);
	m_fonts[font.id]->setBoxed(true);
	m_fonts[font.id]->drawText(str, pos + padding, text_color);
	// graphic::Renderer::Instance().ToggleDelayObject(false);

	m_draw_sub_text = false;

	return true;
}

bool HDText::drawRectangledText(const wchar_t* str, int x, int y, uint32_t rect_color, uint32_t rect_transparency, uint32_t color)
{
	if (!isActive() || !str)
		return false;

	// rectColor = 0, rectTransparency = 2, dwColor = 0   stats panel popup
	// rectColor = 0, rectTransparency = 1, dwColor = 0   alt item
	// rectColor = 140, rectTransparency = 5, dwColor = 0   alt item hovered

	const auto font = getFont(m_text_size);
	const auto text_color = g_text_colors.at(getColor(font.color ? font.color : color));

	m_fonts[font.id]->setSize(font.size);
	const auto size = m_fonts[font.id]->getTextSize(str);
	const auto line_count = m_fonts[font.id]->getLineCount();

	uint32_t bg_color = m_bg_color;
	glm::vec2 padding = { 3.4f, font.size * 0.05f * line_count };

	glm::vec2 back_pos = { (float)x + 3.2f, (float)y - size.y + 1.6f / line_count };
	glm::vec2 text_pos = { back_pos.x, back_pos.y + font.size - font.size * 0.08f };

	if (rect_transparency == 2) {
		padding = { 10.0f, 5.0f };
		m_object_bg->setColor(m_border_color, 2);
		m_object_bg->setFlags({ 2, 2, 0, 0 });
		m_object_bg->setExtra(size + padding * 2.0f);
	} else {
		if (rect_transparency == 1)
			bg_color = m_alt_bg_color;
		else if (rect_transparency == 5)
			bg_color = m_alt_bg_hovered;

		// back_pos += glm::vec2(3.7f, 2.4f);
		m_object_bg->setFlags({ 2, 0, 0, 0 });
	}

	m_object_bg->setPosition(back_pos - padding);
	m_object_bg->setSize(size + padding * 2.0f);
	m_object_bg->setColor(bg_color, 1);
	App.context->pushObject(m_object_bg);

	m_fonts[font.id]->setAlign(TextAlign::Center);
	m_fonts[font.id]->setWeight(font.weight);
	m_fonts[font.id]->setBoxed(true);
	m_fonts[font.id]->drawText(str, text_pos, text_color);

	return true;
}

bool HDText::drawSolidRect(int left, int top, int right, int bottom, uint32_t color, int draw_mode)
{
	// trace("%dx%d <> %dx%d | %d | %d", nXStart, nYStart, nXEnd, nYEnd, draw_mode, dwColor);
	if (!isActive())
		return false;

	// if (nDrawMode == 2 && dwColor == 0)
	//	trace("%dx%d <> %dx%d | %d | %d", nXStart, nYStart, nXEnd, nYEnd, draw_mode, dwColor);

	if (((draw_mode == 2 && color == 0) || m_bordered_rect) && !*d2::esc_menu_open) {
		// const glm::vec2 padding = bordered_rect_ ? glm::vec2(5.0f, 5.0f) : glm::vec2(4.0f, 2.0f);
		const glm::vec2 padding = { 0.0f, 0.0f };
		const glm::vec2 pos = { (float)left - padding.x, (float)top - padding.y };
		const glm::vec2 size = glm::vec2((float)right + padding.x, (float)bottom + padding.y) - pos;

		m_object_bg->setPosition(pos);
		m_object_bg->setSize(size);
		m_object_bg->setColor(m_bg_color, 1);
		m_object_bg->setColor(m_border_color, 2);
		m_object_bg->setFlags({ 2, 2, 0, 0 });
		m_object_bg->setExtra(size);
		App.context->pushObject(m_object_bg);

		return true;
	}

	return false;
}

uint32_t HDText::getNormalTextWidth(const wchar_t* str, const int n_chars)
{
	const auto font = getFont(m_text_size);
	m_fonts[font.id]->setSize(font.size);
	const auto size = m_fonts[font.id]->getTextSize(str, n_chars);

	return (uint32_t)size.x;
}

uint32_t HDText::getFramedTextSize(const wchar_t* str, uint32_t* width, uint32_t* height)
{
	const auto font = getFont(m_text_size);
	m_fonts[font.id]->setSize(font.size);
	const auto size = m_fonts[font.id]->getTextSize(str);

	*width = (uint32_t)size.x;
	*height = (uint32_t)(size.y * 1.11888f);

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
	if (!d2::sub_text_ptr)
		return;

	// if (!draw_sub_text_) {
	//	draw_sub_text_ = true;
	//	return;
	// }

	static const wchar_t* str;
	static uint32_t* color;
	static int* lenth;
	static int* x;
	static int* y;

	if (fn == 1) {
		str = *(const wchar_t**)((uintptr_t)d2::sub_text_ptr + (isVerMax(V_110f) ? 0x20 : 0x68));
		color = (uint32_t*)((uintptr_t)d2::sub_text_ptr + (isVerMax(V_110f) ? 0x78 : 0x74));
		lenth = (int*)((uintptr_t)d2::sub_text_ptr + (isVerMax(V_110f) ? 0x1C : 0x10));
		x = (int*)((uintptr_t)d2::sub_text_ptr + (isVerMax(V_110f) ? 0x18 : 0x18));
		y = (int*)((uintptr_t)d2::sub_text_ptr + (isVerMax(V_110f) ? 0x70 : 0x6C));
	} else if (fn == 2) {
		str = (const wchar_t*)((uintptr_t)d2::sub_text_ptr + 0x390);
		color = (uint32_t*)((uintptr_t)d2::sub_text_ptr + 0x74);
		lenth = (int*)((uintptr_t)d2::sub_text_ptr + 0x14);
		x = (int*)((uintptr_t)d2::sub_text_ptr + 0x1C);
		y = (int*)((uintptr_t)d2::sub_text_ptr + 0x4);
	} else {
		str = (const wchar_t*)((uintptr_t)d2::sub_text_ptr + 0x25C);
		color = (uint32_t*)((uintptr_t)d2::sub_text_ptr + 0x10);
		lenth = (int*)((uintptr_t)d2::sub_text_ptr - 0xC);
		x = (int*)((uintptr_t)d2::sub_text_ptr - 0x14);
		y = (int*)((uintptr_t)d2::sub_text_ptr + 0x8);
	}

	if (!str)
		return;

	drawText(str, *x, *y, *color, 1);
	*lenth = 0;
}

inline const D2FontInfo& HDText::getFont(uint32_t size)
{
	return g_font_sizes.find(size) != g_font_sizes.end() ? g_font_sizes.at(size) : g_font_sizes.at(0);
}

inline wchar_t HDText::getColor(uint32_t color)
{
	return (color < g_default_colors.size()) ? g_default_colors[color] : g_default_colors[0];
}

void drawText(glm::vec2 position, uint32_t color)
{
	static std::unique_ptr<Object> text_obj = std::make_unique<Object>(position, glm::vec2(10.0f, 10.0f));
	text_obj->setPosition(position);
	text_obj->setFlags({ 4, 0, 0, 0 });
	text_obj->setColor(color);
	App.context->pushObject(text_obj);
}

void drawText2()
{
	// static Font font1 = Font({ "ExocetBlizzardMedium", 1.0f, 0.022f, -0.022f, { -0.08f, -0.16f } });
	// static Font font2 = Font({ "Formal436BT", 1.0f, 0.092f, 0.012f, { 0.01f, -0.25f } });
	//// static Font font1 = Font({ "ExocetBlizzardMedium", 1.0f, -0.022f, { -0.08f, -0.16f } });

	// font1.setSize(14.0f);
	// font1.drawText(L"Exocet Blizzard Medium", { 30.0f, 30.0f }, 0xFF0000FF);

	// font2.setSize(14.0f);
	// font2.drawText(L"Exocet Blizzard Medium", { 30.0f, 50.0f }, 0xFF0000FF);
}

void drawSubText(uint8_t fn)
{
}

}