#pragma once

#include "palette.h"
#include <ddraw.h>

namespace d2gl {

struct BitmapInfo256 {
	BITMAPINFOHEADER header = { 0 };
	RGBQUAD colors[256] = { 0 };
};

struct DirectDrawSurface : public IDirectDrawSurface {
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

	HRESULT __stdcall QueryInterface(REFIID riid, LPVOID FAR* object);
	ULONG __stdcall AddRef();
	ULONG __stdcall Release();

	HRESULT __stdcall AddAttachedSurface(LPDIRECTDRAWSURFACE surface);
	HRESULT __stdcall AddOverlayDirtyRect(LPRECT rect);
	HRESULT __stdcall Blt(LPRECT in_dst_rect, LPDIRECTDRAWSURFACE surface, LPRECT in_src_rect, DWORD flags, LPDDBLTFX blt_fx);
	HRESULT __stdcall BltBatch(LPDDBLTBATCH blt_batch, DWORD count, DWORD flags);
	HRESULT __stdcall BltFast(DWORD x, DWORD y, LPDIRECTDRAWSURFACE surface, LPRECT in_src_rect, DWORD flags);
	HRESULT __stdcall DeleteAttachedSurface(DWORD flags, LPDIRECTDRAWSURFACE surface);
	HRESULT __stdcall EnumAttachedSurfaces(LPVOID context, LPDDENUMSURFACESCALLBACK enum_surfaces_callback);
	HRESULT __stdcall EnumOverlayZOrders(DWORD flags, LPVOID context, LPDDENUMSURFACESCALLBACK enum_surfaces_callback);
	HRESULT __stdcall Flip(LPDIRECTDRAWSURFACE surface, DWORD flags);
	HRESULT __stdcall GetAttachedSurface(LPDDSCAPS caps, LPDIRECTDRAWSURFACE FAR* surface);
	HRESULT __stdcall GetBltStatus(DWORD flags);
	HRESULT __stdcall GetCaps(LPDDSCAPS caps);
	HRESULT __stdcall GetClipper(LPDIRECTDRAWCLIPPER FAR* clipper);
	HRESULT __stdcall GetColorKey(DWORD flags, LPDDCOLORKEY color_key);
	HRESULT __stdcall GetDC(HDC FAR* hdc);
	HRESULT __stdcall GetFlipStatus(DWORD flags);
	HRESULT __stdcall GetOverlayPosition(LPLONG x, LPLONG y);
	HRESULT __stdcall GetPalette(LPDIRECTDRAWPALETTE FAR* palette);
	HRESULT __stdcall GetPixelFormat(LPDDPIXELFORMAT pixel_format);
	HRESULT __stdcall GetSurfaceDesc(LPDDSURFACEDESC surface_desc);
	HRESULT __stdcall Initialize(LPDIRECTDRAW ddraw, LPDDSURFACEDESC surface_desc);
	HRESULT __stdcall IsLost();
	HRESULT __stdcall Lock(LPRECT dst_rect, LPDDSURFACEDESC surface_desc, DWORD flags, HANDLE event);
	HRESULT __stdcall ReleaseDC(HDC hdc);
	HRESULT __stdcall Restore();
	HRESULT __stdcall SetClipper(LPDIRECTDRAWCLIPPER clipper);
	HRESULT __stdcall SetColorKey(DWORD flags, LPDDCOLORKEY color_key);
	HRESULT __stdcall SetOverlayPosition(LONG x, LONG y);
	HRESULT __stdcall SetPalette(LPDIRECTDRAWPALETTE palette);
	HRESULT __stdcall Unlock(LPVOID rect);
	HRESULT __stdcall UpdateOverlay(LPRECT src_rect, LPDIRECTDRAWSURFACE surface, LPRECT dst_rect, DWORD flags, LPDDOVERLAYFX overlay_fx);
	HRESULT __stdcall UpdateOverlayDisplay(DWORD flags);
	HRESULT __stdcall UpdateOverlayZOrder(DWORD flags, LPDIRECTDRAWSURFACE surface);
	// HRESULT __stdcall GetDDInterface(LPVOID* lplpDD);
	// HRESULT __stdcall PageLock(DWORD dwFlags);
	// HRESULT __stdcall PageUnlock(DWORD dwFlags);
	// HRESULT __stdcall SetSurfaceDesc(LPDDSURFACEDESC2 lpDDSD, DWORD dwFlags);
	// HRESULT __stdcall SetPrivateData(REFGUID rtag, LPVOID lpData, DWORD dwSize, DWORD dwFlags);
	// HRESULT __stdcall GetPrivateData(REFGUID rtag, LPVOID lpBuffer, LPDWORD lpdwBufferSize);
	// HRESULT __stdcall FreePrivateData(REFGUID rtag);
	// HRESULT __stdcall GetUniquenessValue(LPDWORD lpdwValue);
	// HRESULT __stdcall ChangeUniquenessValue();
	// HRESULT __stdcall SetPriority(DWORD dwPrio);
	// HRESULT __stdcall GetPriority(LPDWORD lpdwPrio);
	// HRESULT __stdcall SetLOD(DWORD dwLod);
	// HRESULT __stdcall GetLOD(LPDWORD lpdwLod);
};

extern DirectDrawSurface* DDrawSurface;

}