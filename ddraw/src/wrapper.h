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

#include <ddraw.h>

namespace d2gl {

class Wrapper;

extern const char* g_shader_game;
extern std::unique_ptr<Wrapper> DDrawWrapper;

class Wrapper {
	Context* ctx;
	bool m_swapped = true;

public:
	Wrapper();
	~Wrapper() = default;

	void onBufferClear();
	void onBufferSwap(bool flip = false);

	void updatePalette(const glm::vec4* data);

	static HRESULT setCooperativeLevel(HWND hwnd, DWORD flags);
	static HRESULT setDisplayMode(DWORD width, DWORD height, DWORD bpp);
};

}