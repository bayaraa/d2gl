#pragma once

#include "surface.h"

#include <ddraw.h>

namespace d2gl {

struct DirectDraw : public IDirectDraw {
	ULONG m_ref = 0;

	HRESULT __stdcall QueryInterface(REFIID riid, LPVOID FAR* object);
	ULONG __stdcall AddRef();
	ULONG __stdcall Release();

	HRESULT __stdcall Compact();
	HRESULT __stdcall CreateClipper(DWORD flags, LPDIRECTDRAWCLIPPER FAR* clipper, IUnknown FAR* unknown);
	HRESULT __stdcall CreatePalette(DWORD flags, LPPALETTEENTRY palette_entry, LPDIRECTDRAWPALETTE FAR* palette, IUnknown FAR* unknown);
	HRESULT __stdcall CreateSurface(LPDDSURFACEDESC surface_desc, LPDIRECTDRAWSURFACE FAR* surface, IUnknown FAR* unknown);
	HRESULT __stdcall DuplicateSurface(LPDIRECTDRAWSURFACE src_surface, LPDIRECTDRAWSURFACE FAR* dst_surface);
	HRESULT __stdcall EnumDisplayModes(DWORD flags, LPDDSURFACEDESC surface_desc, LPVOID context, LPDDENUMMODESCALLBACK enum_modes_callback);
	HRESULT __stdcall EnumSurfaces(DWORD flags, LPDDSURFACEDESC surface_desc, LPVOID context, LPDDENUMSURFACESCALLBACK enum_surfaces_callback);
	HRESULT __stdcall FlipToGDISurface();
	HRESULT __stdcall GetCaps(LPDDCAPS driver_caps, LPDDCAPS emul_caps);
	HRESULT __stdcall GetDisplayMode(LPDDSURFACEDESC surface_desc);
	HRESULT __stdcall GetFourCCCodes(LPDWORD num_codes, LPDWORD codes);
	HRESULT __stdcall GetGDISurface(LPDIRECTDRAWSURFACE FAR* gdi_surface);
	HRESULT __stdcall GetMonitorFrequency(LPDWORD frequency);
	HRESULT __stdcall GetScanLine(LPDWORD scan_line);
	HRESULT __stdcall GetVerticalBlankStatus(LPBOOL in_vertical_blank);
	HRESULT __stdcall Initialize(GUID* guid);
	HRESULT __stdcall RestoreDisplayMode();
	HRESULT __stdcall SetCooperativeLevel(HWND hwnd, DWORD flags);
	HRESULT __stdcall SetDisplayMode(DWORD width, DWORD height, DWORD bpp);
	HRESULT __stdcall WaitForVerticalBlank(DWORD flags, HANDLE event);
};

extern DirectDraw* DDraw;

}