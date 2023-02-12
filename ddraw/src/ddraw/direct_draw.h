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

// extern DirectDraw* DDraw;

}