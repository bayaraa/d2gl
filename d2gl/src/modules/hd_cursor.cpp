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
#include "hd_cursor.h"
#include "d2/common.h"
#include "d2/funcs.h"
#include "helpers.h"

namespace d2gl::modules {

CursorObject::CursorObject(const std::unique_ptr<Texture>& texture, const std::string& file_name, uint32_t frames, glm::vec2 offset)
	: m_offset(offset)
{
	auto image_data = helpers::loadImage(file_name);
	auto tex_data = texture->fillImage(image_data, frames);
	helpers::clearImage(image_data);

	m_start_layer = tex_data.start_layer;
	m_object = std::make_unique<Object>(glm::vec2(0.0f, 0.0f), glm::vec2(40.0f, 40.0f));
	m_object->setFlags(1);
}

void CursorObject::draw(uint8_t frame)
{
	glm::ivec2 pos = d2::getCursorPos();
	m_object->setPosition({ (float)pos.x - m_offset.x, (float)pos.y - m_offset.y });
	m_object->setTexIds(m_start_layer + frame - 1);

	App.context->pushObject(m_object);
}

HDCursor::HDCursor()
{
	TextureCreateInfo texture_ci;
	texture_ci.size = { 128, 128 };
	texture_ci.layer_count = 50;
	texture_ci.slot = TEXTURE_SLOT_CURSOR;
	texture_ci.min_filter = GL_LINEAR;
	texture_ci.mag_filter = GL_LINEAR;
	m_texture = Context::createTexture(texture_ci);

	m_hand_cursor = std::make_unique<CursorObject>(m_texture, "assets\\textures\\cursor\\hand.png", 19, glm::vec2(1.0f, 6.0f));
	m_other_cursor = std::make_unique<CursorObject>(m_texture, "assets\\textures\\cursor\\other.png", 10, glm::vec2(1.0f, 6.0f));
}

void HDCursor::draw()
{
	if (!App.hd_cursor)
		return;

	// clang-format off
	static std::array<uint8_t, 162> hand_frames = {
		3,4,5,6,6,6,6,6,6,7,8,9,10,11,12,13,14,15,15,14,13,14,15,15,15,15,16,17,18,19,19,19,19,19,
		19,19,19,19,18,17,17,18,19,19,19,19,19,19,19,19,19,19,19,19,18,17,16,15,14,13,13,13,13,13,
		13,13,13,13,13,12,11,10,9,8,7,7,8,9,10,11,12,13,14,15,15,14,13,14,15,15,15,15,16,17,18,19,
		19,19,19,19,19,19,19,19,18,17,17,18,19,19,19,19,19,19,19,19,19,19,19,19,18,17,16,15,14,
		13,13,13,13,13,13,13,13,13,13,12,11,10,9,8,7,6,5,4,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
	};
	// clang-format on

	static CursorType cur_cursor = CursorType::None;
	cur_cursor = getCursorType();

	if (cur_cursor == CursorType::None)
		return;

	if (cur_cursor >= CursorType::Hand) {
		if (cur_cursor == CursorType::Idle) {
			static int cur_time = 0;
			static int last_time = std::clock();
			cur_time = std::clock();
			m_hand_frame = hand_frames[m_frame_pos];

			if (last_time + 150 < cur_time) {
				m_frame_pos++;
				if (m_frame_pos >= hand_frames.size())
					m_frame_pos = 0;
				last_time = cur_time;
			}
		}

		if (cur_cursor == CursorType::Hand || cur_cursor == CursorType::Press) {
			m_frame_pos = 0;
			if (m_hand_frame > 2) {
				if (m_hand_frame > 10)
					m_hand_frame = 10;

				m_hand_frame--;
				if (m_hand_frame == 2 && cur_cursor != CursorType::Press)
					m_hand_frame = 1;
			} else
				m_hand_frame = (cur_cursor == CursorType::Press) ? 2 : 1;
		}

		m_hand_cursor->draw(m_hand_frame);
	} else
		m_other_cursor->draw((uint8_t)cur_cursor);
}

CursorType HDCursor::getCursorType()
{
	static uint16_t prev_state = m_cursor_state;
	m_cursor_state = (*d2::cursor_state1 << 8) | *d2::cursor_state2;

	switch (*d2::cursor_state3) {
		case 2: return CursorType::Buy;
		case 3: return CursorType::Sell;
		case 4: return CursorType::Repair;
	}

	CursorType cursor_type = CursorType::Hand;
	switch (m_cursor_state) {
		case 0x0606: cursor_type = CursorType::Identify; break;
		case 0x0504: cursor_type = CursorType::Press; break;
		case 0x0403:
		case 0x0202: cursor_type = CursorType::Idle; break;
		case 0x0302: cursor_type = (prev_state == 0x0105 ? CursorType::None : CursorType::Idle); break;
		default: cursor_type = (m_pressed ? CursorType::Press : CursorType::Hand);
	}

	prev_state = m_cursor_state;
	return cursor_type;
}

void HDCursor::mouseProc(UINT uMsg)
{
	switch (uMsg) {
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_MBUTTONDOWN: m_pressed = true; break;
		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
		case WM_MBUTTONUP: m_pressed = false; break;
	}
}

}