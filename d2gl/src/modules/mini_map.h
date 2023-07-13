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

class MiniMap {
	std::unique_ptr<Object> m_bg, m_map;
	glm::vec2 m_pos = { 0.0f, 0.0f };
	glm::vec2 m_size = { 0.0f, 0.0f };
	uint32_t m_time_width = 0;

	MiniMap();
	~MiniMap() = default;

public:
	static MiniMap& Instance()
	{
		static MiniMap instance;
		return instance;
	}

	inline bool isActive() { return ISGLIDE3X() && App.mini_map.active; }
	inline uint32_t getTimeWidth() const { return m_time_width; }

	void resize();
	void draw();
};

}