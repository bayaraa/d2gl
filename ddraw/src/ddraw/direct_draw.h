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

#include "surface.h"

#include <ddraw.h>

namespace d2gl {

// clang-format off
class DirectDraw : public IDirectDraw {
	ULONG m_ref = 0;

public:
	DirectDraw() = default;
	~DirectDraw() = default;

	/*** IUnknown methods ***/
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) { return DDERR_UNSUPPORTED; }
	STDMETHOD_(ULONG, AddRef)(THIS);
	STDMETHOD_(ULONG, Release)(THIS);
	/*** IDirectDraw methods ***/
	STDMETHOD(Compact)(THIS) { return DDERR_UNSUPPORTED; }
	STDMETHOD(CreateClipper)(THIS_ DWORD, LPDIRECTDRAWCLIPPER FAR*, IUnknown FAR*) { return DDERR_UNSUPPORTED; }
	STDMETHOD(CreatePalette)(THIS_ DWORD, LPPALETTEENTRY, LPDIRECTDRAWPALETTE FAR*, IUnknown FAR*);
	STDMETHOD(CreateSurface)(THIS_ LPDDSURFACEDESC, LPDIRECTDRAWSURFACE FAR*, IUnknown FAR*);
	STDMETHOD(DuplicateSurface)(THIS_ LPDIRECTDRAWSURFACE, LPDIRECTDRAWSURFACE FAR*) { return DDERR_UNSUPPORTED; }
	STDMETHOD(EnumDisplayModes)(THIS_ DWORD, LPDDSURFACEDESC, LPVOID, LPDDENUMMODESCALLBACK) { return DDERR_UNSUPPORTED; }
	STDMETHOD(EnumSurfaces)(THIS_ DWORD, LPDDSURFACEDESC, LPVOID, LPDDENUMSURFACESCALLBACK) { return DDERR_UNSUPPORTED; }
	STDMETHOD(FlipToGDISurface)(THIS) { return DDERR_UNSUPPORTED; }
	STDMETHOD(GetCaps)(THIS_ LPDDCAPS, LPDDCAPS) { return DDERR_UNSUPPORTED; }
	STDMETHOD(GetDisplayMode)(THIS_ LPDDSURFACEDESC) { return DDERR_UNSUPPORTED; }
	STDMETHOD(GetFourCCCodes)(THIS_ LPDWORD, LPDWORD) { return DDERR_UNSUPPORTED; }
	STDMETHOD(GetGDISurface)(THIS_ LPDIRECTDRAWSURFACE FAR*) { return DDERR_UNSUPPORTED; }
	STDMETHOD(GetMonitorFrequency)(THIS_ LPDWORD) { return DDERR_UNSUPPORTED; }
	STDMETHOD(GetScanLine)(THIS_ LPDWORD) { return DDERR_UNSUPPORTED; }
	STDMETHOD(GetVerticalBlankStatus)(THIS_ LPBOOL) { return DDERR_UNSUPPORTED; }
	STDMETHOD(Initialize)(THIS_ GUID FAR*) { return DDERR_UNSUPPORTED; }
	STDMETHOD(RestoreDisplayMode)(THIS) { return DDERR_UNSUPPORTED; }
	STDMETHOD(SetCooperativeLevel)(THIS_ HWND, DWORD);
	STDMETHOD(SetDisplayMode)(THIS_ DWORD, DWORD, DWORD);
	STDMETHOD(WaitForVerticalBlank)(THIS_ DWORD, HANDLE) { return DD_OK; }
};
// clang-format on

}