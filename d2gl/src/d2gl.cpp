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
#include "d2gl.h"

#ifdef __cplusplus
extern "C" {
#endif

__declspec(dllexport) BOOL __stdcall d2glConfigQueryImpl(D2GLConfigId config_id)
{
	switch (config_id) {
		case D2GL_CONFIG_VSYNC: return d2gl::App.vsync;
		case D2GL_CONFIG_CURSOR_UNLOCKED: return d2gl::App.cursor.unlock;
		case D2GL_CONFIG_HD_CURSOR: return d2gl::App.hd_cursor;
		case D2GL_CONFIG_HD_TEXT: return d2gl::App.hd_text.active;
		case D2GL_CONFIG_MOTION_PREDICTION: return d2gl::App.motion_prediction;
		case D2GL_CONFIG_MINI_MAP: return d2gl::App.mini_map.active;
		case D2GL_CONFIG_SHOW_ITEM_QUANTITY: return d2gl::App.show_item_quantity;
		case D2GL_CONFIG_SHOW_MONSTER_RES: return d2gl::App.show_monster_res;
		case D2GL_CONFIG_SHOW_FPS: return d2gl::App.show_fps;
	}
	return FALSE;
}

__declspec(dllexport) void __stdcall setCustomScreenSize(uint32_t width, uint32_t height)
{
	d2gl::App.game.custom_size = { width, height };
}

#ifdef __cplusplus
}
#endif