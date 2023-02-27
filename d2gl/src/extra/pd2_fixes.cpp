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
#include "pd2_fixes.h"
#include "d2/common.h"
#include "helpers.h"
#include "patch.h"

namespace d2gl {

int pd2_draw_ui = 0;
int pd2_draw_shifted_image = 0;

uintptr_t* PD2Fn1 = nullptr;
uintptr_t* FreeResFn1 = nullptr;

uintptr_t d2common_address = 0;
uint8_t* d2common_old_code = nullptr;
uint8_t* d2common_new_code = nullptr;

uintptr_t d2client_address = 0;
uint8_t* d2client_old_code = nullptr;
uint8_t* d2client_new_code = nullptr;

bool isPD2()
{
	static HANDLE handle = GetModuleHandleA("ProjectDiablo.dll");
	return (bool)handle;
}

void fixPD2drawImage(int x, int y)
{
	if (pd2_draw_ui) {
		if (App.hd_text)
			d2::drawSolidRectEx(x, y - 24, x + 24, y, 0, 5);

		pd2_draw_shifted_image = 1;
	}
}

void fixPD2drawSolidRectEx(glm::ivec2& offset, int draw_mode)
{
	if (pd2_draw_ui && draw_mode == 2)
		offset.y += 1;
}

bool fixPD2drawGroundTile(const d2::TileContext* tile)
{
	if (App.pd2_fix && App.api == Api::Glide && tile) {
		const auto len = strlen(tile->szTileName);
		if (len > 8) {
			const auto name = tile->szTileName + len - 8;
			if (strcmp(name, "Warp.dt1") == 0)
				return true;
		}
	}

	return false;
}

void fixPD2invItemActions()
{
	if (!App.pd2_fix || !d2common_new_code)
		return;

	static bool applied = true;
	bool apply = (App.game.screen != GameScreen::InGame || *d2::screen_shift == 0);

	if (!applied && apply) {
		Patch::setBytes(d2common_address, 5, d2common_new_code);
		Patch::setBytes(d2client_address, 76, d2client_new_code);
		applied = true;
	} else if (applied && !apply) {
		Patch::setBytes(d2common_address, 5, d2common_old_code);
		Patch::setBytes(d2client_address, 76, d2client_old_code);
		applied = false;
	}
}

__declspec(naked) void crossFnPD2()
{
	__asm
	{
		mov pd2_draw_ui, 0x1
		call PD2Fn1
		mov pd2_draw_ui, 0x0
		ret
	}
}

__declspec(naked) void crossFnPD2andFreeRes()
{
	__asm
	{
		mov pd2_draw_ui, 0x1
		call PD2Fn1
		mov pd2_draw_ui, 0x0
		call FreeResFn1
		ret
	}
}

__declspec(naked) void d2glideFix()
{
	static uintptr_t addr_back = 0x6F85A61A;
	static uintptr_t addr_skip = 0x6F85A67E;
	__asm
	{
		push eax
		xor eax, eax
		mov eax, pd2_draw_shifted_image
		test eax, eax
		mov pd2_draw_shifted_image, 0x0
		je skip
		pop eax
		lea edi, [eax + ebx * 8 + 04]
		jmp [addr_skip]
		
		skip:
		pop eax
		test ecx, ecx
		lea edi, [eax + ebx * 8 + 04]
		jmp [addr_back]
	}
}

void applyPD2fixes(int step)
{
	if (!App.pd2_fix)
		return;

	const auto offset = (uintptr_t*)helpers::getProcOffset(DLL_D2CLIENT, 0xC3AA6 + 1);

	if (step == 0) {
		PD2Fn1 = (uintptr_t*)((uintptr_t)offset + *offset + 4);

		d2common_address = helpers::getProcOffset(DLL_D2COMMON, -10760);
		Patch::getBytes(d2common_address, 5, &d2common_old_code);

		d2client_address = helpers::getProcOffset(DLL_D2CLIENT, 0x10DFD);
		Patch::getBytes(d2client_address, 76, &d2client_old_code);
	} else if (step == 1) {
		FreeResFn1 = (uintptr_t*)((uintptr_t)offset + *offset + 4);

		if (PD2Fn1 != FreeResFn1) {
			Patch patch = Patch();
			patch.add(PatchType::Call, { 0xC3AA6, 0, 0, DLL_D2CLIENT }, 5, (uintptr_t)crossFnPD2andFreeRes);
			patch.toggle(true);

			Patch::getBytes(d2common_address, 5, &d2common_new_code);
			Patch::getBytes(d2client_address, 76, &d2client_new_code);
		} else {
			Patch patch = Patch();
			patch.add(PatchType::Call, { 0xC3AA6, 0, 0, DLL_D2CLIENT }, 5, (uintptr_t)crossFnPD2);
			patch.toggle(true);
		}

		if (App.api == Api::Glide) {
			Patch patch2 = Patch();
			patch2.add(PatchType::Jump, { 0xA614, 0, 0, DLL_D2GLIDE }, 6, (uintptr_t)d2glideFix);
			patch2.toggle(true);
		}
	}
}

}