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

#include "palette.h"
#include <ddraw.h>

namespace d2gl {

struct BitmapInfo256 {
	BITMAPINFOHEADER header = { 0 };
	RGBQUAD colors[256] = { 0 };
};

// clang-format off
class DirectDrawSurface : public IDirectDrawSurface {
	ULONG m_ref = 0;

	LONG m_width = 0, m_height = 0;
	DWORD m_bpp = 0, m_flags = 0, m_caps = 0;

	void* m_data = nullptr;
	DWORD m_xpitch = 0, m_ypitch = 0;

	BitmapInfo256* m_bmi = nullptr;
	HBITMAP m_bitmap = nullptr;
	HDC m_hdc = nullptr;

	DirectDrawPalette* m_palette = nullptr;
	DirectDrawSurface* m_back_buffer = nullptr;

public:
	DirectDrawSurface(LPDDSURFACEDESC surface_desc);
	~DirectDrawSurface();

	inline const DirectDrawPalette* getPalette() { return m_palette; }
	inline const uint8_t* getData() { return (uint8_t*)m_data; }

	/*** IUnknown methods ***/
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) { return DDERR_UNSUPPORTED; }
	STDMETHOD_(ULONG, AddRef)(THIS);
	STDMETHOD_(ULONG, Release)(THIS);
	/*** IDirectDrawSurface methods ***/
	STDMETHOD(AddAttachedSurface)(THIS_ LPDIRECTDRAWSURFACE) { return DDERR_UNSUPPORTED; }
	STDMETHOD(AddOverlayDirtyRect)(THIS_ LPRECT) { return DDERR_UNSUPPORTED; }
	STDMETHOD(Blt)(THIS_ LPRECT, LPDIRECTDRAWSURFACE, LPRECT, DWORD, LPDDBLTFX);
	STDMETHOD(BltBatch)(THIS_ LPDDBLTBATCH, DWORD, DWORD) { return DDERR_UNSUPPORTED; }
	STDMETHOD(BltFast)(THIS_ DWORD, DWORD, LPDIRECTDRAWSURFACE, LPRECT, DWORD);
	STDMETHOD(DeleteAttachedSurface)(THIS_ DWORD, LPDIRECTDRAWSURFACE) { return DDERR_UNSUPPORTED; }
	STDMETHOD(EnumAttachedSurfaces)(THIS_ LPVOID, LPDDENUMSURFACESCALLBACK) { return DDERR_UNSUPPORTED; }
	STDMETHOD(EnumOverlayZOrders)(THIS_ DWORD, LPVOID, LPDDENUMSURFACESCALLBACK) { return DDERR_UNSUPPORTED; }
	STDMETHOD(Flip)(THIS_ LPDIRECTDRAWSURFACE, DWORD);
	STDMETHOD(GetAttachedSurface)(THIS_ LPDDSCAPS, LPDIRECTDRAWSURFACE FAR*);
	STDMETHOD(GetBltStatus)(THIS_ DWORD) { return DD_OK; }
	STDMETHOD(GetCaps)(THIS_ LPDDSCAPS) { return DDERR_UNSUPPORTED; }
	STDMETHOD(GetClipper)(THIS_ LPDIRECTDRAWCLIPPER FAR*) { return DDERR_UNSUPPORTED; }
	STDMETHOD(GetColorKey)(THIS_ DWORD, LPDDCOLORKEY) { return DDERR_UNSUPPORTED; }
	STDMETHOD(GetDC)(THIS_ HDC FAR*) { return DDERR_UNSUPPORTED; }
	STDMETHOD(GetFlipStatus)(THIS_ DWORD) { return DD_OK; }
	STDMETHOD(GetOverlayPosition)(THIS_ LPLONG, LPLONG) { return DDERR_UNSUPPORTED; }
	STDMETHOD(GetPalette)(THIS_ LPDIRECTDRAWPALETTE FAR*) { return DDERR_UNSUPPORTED; }
	STDMETHOD(GetPixelFormat)(THIS_ LPDDPIXELFORMAT);
	STDMETHOD(GetSurfaceDesc)(THIS_ LPDDSURFACEDESC);
	STDMETHOD(Initialize)(THIS_ LPDIRECTDRAW, LPDDSURFACEDESC) { return DDERR_UNSUPPORTED; }
	STDMETHOD(IsLost)(THIS) { return DDERR_UNSUPPORTED; }
	STDMETHOD(Lock)(THIS_ LPRECT, LPDDSURFACEDESC, DWORD, HANDLE);
	STDMETHOD(ReleaseDC)(THIS_ HDC) { return DDERR_UNSUPPORTED; }
	STDMETHOD(Restore)(THIS) { return DDERR_UNSUPPORTED; }
	STDMETHOD(SetClipper)(THIS_ LPDIRECTDRAWCLIPPER) { return DDERR_UNSUPPORTED; }
	STDMETHOD(SetColorKey)(THIS_ DWORD, LPDDCOLORKEY) { return DDERR_UNSUPPORTED; }
	STDMETHOD(SetOverlayPosition)(THIS_ LONG, LONG) { return DDERR_UNSUPPORTED; }
	STDMETHOD(SetPalette)(THIS_ LPDIRECTDRAWPALETTE);
	STDMETHOD(Unlock)(THIS_ LPVOID);
	STDMETHOD(UpdateOverlay)(THIS_ LPRECT, LPDIRECTDRAWSURFACE, LPRECT, DWORD, LPDDOVERLAYFX) { return DDERR_UNSUPPORTED; }
	STDMETHOD(UpdateOverlayDisplay)(THIS_ DWORD) { return DDERR_UNSUPPORTED; }
	STDMETHOD(UpdateOverlayZOrder)(THIS_ DWORD, LPDIRECTDRAWSURFACE) { return DDERR_UNSUPPORTED; }
};
// clang-format on

extern DirectDrawSurface* DDrawSurface;

}