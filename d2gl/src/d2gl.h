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

#ifndef INCLUDE_D2GL_H
#define INCLUDE_D2GL_H

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum D2GLConfigId {
	D2GL_CONFIG_VSYNC,
	D2GL_CONFIG_CURSOR_UNLOCKED,
	D2GL_CONFIG_HD_CURSOR,
	D2GL_CONFIG_HD_TEXT,
	D2GL_CONFIG_MOTION_PREDICTION,
	D2GL_CONFIG_MINI_MAP,
	D2GL_CONFIG_SHOW_ITEM_QUANTITY,
	D2GL_CONFIG_SHOW_MONSTER_RES,
	D2GL_CONFIG_SHOW_FPS,
} D2GLConfigId;

extern BOOL d2glConfigQuery(D2GLConfigId config_id)
{
	typedef BOOL(__stdcall * d2glConfigQueryImpl_t)(D2GLConfigId);
	static d2glConfigQueryImpl_t d2glConfigQueryImpl = NULL;

	if (!d2glConfigQueryImpl) {
		HMODULE handle = GetModuleHandleA("glide3x.dll");
		handle = !handle ? GetModuleHandleA("ddraw.dll") : handle;
		if (handle) {
			FARPROC proc = GetProcAddress(handle, "_d2glConfigQueryImpl@4");
			d2glConfigQueryImpl = proc ? (d2glConfigQueryImpl_t)proc : NULL;
		}
	}
	if (d2glConfigQueryImpl)
		return d2glConfigQueryImpl(config_id);

	return FALSE;
}

#ifdef __cplusplus
}
#endif

#endif