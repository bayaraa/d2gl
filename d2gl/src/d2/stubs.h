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

namespace d2gl::d2 {

uint32_t __stdcall mpqLoadStub(const char* dll, const char* mpqfile, const char* mpqname, int v4, int v5);

void gameDrawBeginStub();

void automapDrawBeginStub();
void automapDrawEndStub();

void uiDrawBeginStub();
void uiDrawCursorItemStub();
void uiDrawEndStub();

void drawUnitStubStack();
void drawUnitStubESI();
void drawMissileStub();
void drawWeatherParticlesStub();
void drawWeatherParticlesStub114d();

void rectangledTextBeginStub();
void rectangledTextBeginStub110f();
void rectangledTextBeginStub109d();
void rectangledTextEndStub();
void drawRectFrameStub();
void drawRectFrameStubECX();

void unitHoverTextStub();
void altItemsTextStub();
void altItemsTextStub110f();
void altItemsTextStub109d();

void loadUIImageStub();
void loadUIImageStubECX();
void drawSubTextAStub();
void drawSubTextBStub();
void drawSubTextCStub();
void levelEntryTextStub();

wchar_t* __fastcall getLevelName(int Level_no);

}