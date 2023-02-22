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
#include "stubs.h"
#include "common.h"
#include "funcs.h"

namespace d2gl::d2 {

// clang-format off
__declspec(naked) uint32_t __stdcall mpqLoadStub(const char* dll, const char* mpqfile, const char* mpqname, int v4, int v5)
{
	__asm jmp mpqLoad_Fn
}

__declspec(naked) void gameDrawBeginStub()
{
	__asm
	{
		call d2SoundFn1_O
		pushad
		call gameDrawBegin
		popad
		ret
	}
}

__declspec(naked) void automapDrawBeginStub()
{
	__asm
	{
		call d2ClientFn1_O
		pushad
		call automapDrawBegin
		popad
		ret
	}
}

__declspec(naked) void automapDrawEndStub()
{
	__asm
	{
		pushad
		call automapDrawEnd
		popad
		call d2ClientFn2_O
		ret
	}
}

__declspec(naked) void uiDrawBeginStub()
{
	__asm
	{
		pushad
		call uiDrawBegin
		popad
		ret
	}
}

__declspec(naked) void uiDrawEndStub()
{
	__asm
	{
		pushad
		call uiDrawEnd
		popad
		ret
	}
}

__declspec(naked) void drawUnitStubStack()
{
	static void* return_addr = nullptr;

	__asm
	{
		push eax
		push edx
		lea edx, return_addr
		mov eax, dword ptr[esp + 0x08]
		mov dword ptr[edx], eax
		lea eax, patch_return_addr
		mov dword ptr[esp + 0x08], eax
		lea edx, currently_drawing_unit
		mov eax, dword ptr[esp + 0x0c]
		mov dword ptr[edx], eax
		pop edx
		pop eax
	}

	__asm jmp drawUnit

	patch_return_addr :
	__asm
	{
		push eax
		push eax
		push edx
		lea edx, currently_drawing_unit
		xor eax, eax
		mov dword ptr[edx], eax
		lea edx, return_addr
		mov eax, dword ptr[edx]
		mov dword ptr[esp + 0x08], eax
		pop edx
		pop eax
		ret
	}
}

__declspec(naked) void drawUnitStubESI()
{
	static void* return_addr = nullptr;

	__asm
	{
		push eax
		push edx
		lea edx, return_addr
		mov eax, dword ptr[esp + 0x08]
		mov dword ptr[edx], eax
		lea eax, patch_return_addr
		mov dword ptr[esp + 0x08], eax
		lea edx, currently_drawing_unit
		mov dword ptr[edx], esi
		pop edx
		pop eax
	}

	__asm jmp drawUnit

	patch_return_addr :
	__asm
	{
		push eax
		push eax
		push edx
		lea edx, currently_drawing_unit
		xor eax, eax
		mov dword ptr[edx], eax
		lea edx, return_addr
		mov eax, dword ptr[edx]
		mov dword ptr[esp + 0x08], eax
		pop edx
		pop eax
		ret
	}
}

__declspec(naked) void drawMissileStub()
{
	static void* return_addr = nullptr;

	__asm
	{
		push eax
		push edx
		lea edx, return_addr
		mov eax, dword ptr[esp + 0x08]
		mov dword ptr[edx], eax
		lea eax, patch_return_addr
		mov dword ptr[esp + 0x08], eax
		lea edx, currently_drawing_unit
		mov dword ptr[edx], esi
		pop edx
		pop eax
	}

	__asm jmp drawMissile

	patch_return_addr :
	__asm
	{
		push eax
		push eax
		push edx
		lea edx, currently_drawing_unit
		xor eax, eax
		mov dword ptr[edx], eax
		lea edx, return_addr
		mov eax, dword ptr[edx]
		mov dword ptr[esp + 0x08], eax
		pop edx
		pop eax
		ret
	}
}

__declspec(naked) void drawWeatherParticlesStub()
{
	static void* return_addr = nullptr;

	__asm
	{
		push eax
		push edx
		lea edx, return_addr
		mov eax, dword ptr[esp + 0x08]
		mov dword ptr[edx], eax
		lea eax, patch_return_addr
		mov dword ptr[esp + 0x08], eax
		mov eax, 1
		lea edx, currently_drawing_weather_particles
		mov dword ptr[edx], eax
		lea edx, currently_drawing_weather_particle_index_ptr
		mov eax, esp
		add eax, 4
		mov dword ptr[edx], eax
		pop edx
		pop eax
	}

	__asm jmp drawWeatherParticles

	patch_return_addr :
	__asm
	{
		push eax
		push eax
		push edx
		lea edx, currently_drawing_weather_particles
		xor eax, eax
		mov dword ptr[edx], eax
		lea edx, return_addr
		mov eax, dword ptr[edx]
		mov dword ptr[esp + 0x08], eax
		pop edx
		pop eax
		ret
	}
}

__declspec(naked) void drawWeatherParticlesStub114d()
{
	static void* return_addr = nullptr;

	__asm
	{
		push eax
		push edx
		lea edx, return_addr
		mov eax, dword ptr[esp + 0x08]
		mov dword ptr[edx], eax
		lea eax, patch_return_addr
		mov dword ptr[esp + 0x08], eax
		mov eax, 1
		lea edx, currently_drawing_weather_particles
		mov dword ptr[edx], eax
		lea edx, currently_drawing_weather_particle_index_ptr
		mov eax, esp
		sub eax, 10h
		mov dword ptr[edx], eax
		pop edx
		pop eax
	}

	__asm jmp drawWeatherParticles

	patch_return_addr :
	__asm
	{
		push eax
		push eax
		push edx
		lea edx, currently_drawing_weather_particles
		xor eax, eax
		mov dword ptr[edx], eax
		lea edx, return_addr
		mov eax, dword ptr[edx]
		mov dword ptr[esp + 0x08], eax
		pop edx
		pop eax
		ret
	}
}

__declspec(naked) void rectangledTextBeginStub()
{
	__asm
	{
		pushad
		call rectangledTextBegin
		popad
		ret
	}
}

__declspec(naked) void rectangledTextEndStub()
{
	__asm
	{
		pushad
		call rectangledTextEnd
		popad
		ret
	}
}

__declspec(naked) void drawRectFrameStub()
{
	__asm
	{
		push edx
		lea edx, currently_drawing_rect
		mov dword ptr[edx], eax
		pop edx
		pushad
		call drawRectFrame
		popad
		ret
	}
}

__declspec(naked) void drawRectFrameStubECX()
{
	__asm
	{
		push edx
		lea edx, currently_drawing_rect
		mov dword ptr[edx], ecx
		pop edx
		pushad
		call drawRectFrame
		popad
		ret
	}
}

__declspec(naked) void unitHoverTextStub()
{
	__asm
	{
		pushad
		call unitHoverText
		popad
		ret
	}
}

__declspec(naked) void altItemsTextStub()
{
	__asm
	{
		push eax
		mov eax, esp
		add eax, 0x0C
		mov alt_item_pos, eax
		pop eax
		pushad
		call altItemsText
		popad
		ret
	}
}
__declspec(naked) void altItemsTextStub110f()
{
	__asm
	{
		mov[esp + 0x34], eax
		xor edi, edi
		push eax
		mov eax, esp
		add eax, 0x0C
		mov alt_item_pos, eax
		pop eax
		pushad
		call altItemsText
		popad
		ret
	}
}
__declspec(naked) void altItemsTextStub109d()
{
	__asm
	{
		xor edi, edi
		mov[esp + 0x20], eax
		push eax
		mov eax, esp
		add eax, 0x0C
		mov alt_item_pos, eax
		pop eax
		pushad
		call altItemsText
		popad
		ret
	}
}

__declspec(naked) void loadUIImageStub()
{
	__asm
	{
		mov ui_image_path, eax
		pushad
		call loadUIImage
		popad
		ret
	}
}
__declspec(naked) void loadUIImageStubECX()
{
	__asm
	{
		mov ui_image_path, ecx
		pushad
		call loadUIImage
		popad
		ret
	}
}

__declspec(naked) void drawSubTextAStub()
{
	__asm
	{
		mov ecx, 0x12
		push eax
		mov eax, esp
		add eax, 0x8
		mov sub_text_ptr, eax
		pop eax
		pushad
		call drawSubTextA
		popad
		ret
	}
}

__declspec(naked) void drawSubTextBStub()
{
	__asm
	{
		mov ecx, 0x12
		push eax
		mov eax, esp
		add eax, 0x8
		mov sub_text_ptr, eax
		pop eax
		pushad
		call drawSubTextB
		popad
		ret
	}
}

__declspec(naked) void drawSubTextCStub()
{
	__asm
	{
		push eax
		mov eax, ebp
		// add eax, 0x8
		mov sub_text_ptr, eax
		pop eax
		pushad
		call drawSubTextC
		popad
		ret
	}
}

__declspec(naked) void levelEntryTextStub()
{
	__asm
	{
		pushad
		call levelEntryText
		popad
		ret
	}
}

__declspec(naked) wchar_t* __fastcall getLevelName(int Level_no)
{
	__asm
	{
		push esi
		mov  esi,ecx
		call getLevelName_Fn
		pop esi
		ret
	}
}
// clang-format on

}