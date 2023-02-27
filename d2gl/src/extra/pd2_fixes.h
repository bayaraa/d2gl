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

#include "d2/structs.h"

namespace d2gl {

extern int pd2_draw_ui;

bool isPD2();
void fixPD2drawImage(int x, int y);
void fixPD2drawSolidRectEx(glm::ivec2& offset, int draw_mode);
bool fixPD2drawGroundTile(const d2::TileContext* tile);
void fixPD2invItemActions();
void applyPD2fixes(int step = 0);

}