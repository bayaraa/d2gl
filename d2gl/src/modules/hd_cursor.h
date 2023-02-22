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

namespace d2gl::modules {

enum class CursorType {
	None,
	Identify,
	Buy,
	Sell,
	Repair,
	Hand,
	Press,
	Idle,
};

class CursorObject {
	std::unique_ptr<Object> m_object;
	uint32_t m_start_layer;
	glm::vec2 m_offset;

public:
	CursorObject(const std::unique_ptr<Texture>& texture, const std::string& file_name, uint32_t frames, glm::vec2 offset);
	~CursorObject() = default;

	void draw(uint8_t frame);
};

class HDCursor {
	std::unique_ptr<Texture> m_texture;
	std::unique_ptr<CursorObject> m_hand_cursor;
	std::unique_ptr<CursorObject> m_other_cursor;

	uint16_t m_cursor_state = 0x0105;
	uint32_t m_hand_frame = 1;
	uint32_t m_frame_pos = 0;
	bool m_pressed = false;

	HDCursor();
	~HDCursor() = default;

public:
	static HDCursor& Instance()
	{
		static HDCursor instance;
		return instance;
	}

	void draw();
	CursorType getCursorType();
	void mouseProc(UINT uMsg);
};

}