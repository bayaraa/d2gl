#include "pch.h"
#include "direct_draw.h"
#include "palette.h"
#include "surface.h"
#include "wrapper.h"

namespace d2gl {

// DirectDraw* DDraw = nullptr;

ULONG __stdcall DirectDraw::AddRef()
{
	m_ref++;
	return m_ref;
}

ULONG __stdcall DirectDraw::Release()
{
	m_ref--;
	if (m_ref == 0) {
		delete this;
		return 0;
	}

	return m_ref;
}

HRESULT __stdcall DirectDraw::CreatePalette(DWORD flags, LPPALETTEENTRY palette_entry, LPDIRECTDRAWPALETTE FAR* palette, IUnknown FAR* unknown)
{
	if (!palette || !palette_entry)
		return DDERR_INVALIDPARAMS;

	auto dd_palette = new DirectDrawPalette(flags, palette_entry);

	*palette = (LPDIRECTDRAWPALETTE)dd_palette;
	IDirectDrawPalette_AddRef(dd_palette);

	return DD_OK;
}

HRESULT __stdcall DirectDraw::CreateSurface(LPDDSURFACEDESC surface_desc, LPDIRECTDRAWSURFACE FAR* surface, IUnknown FAR* unknown)
{
	surface_desc->dwFlags |= DDSD_CAPS;
	auto dst_surface = new DirectDrawSurface(surface_desc);

	*surface = dst_surface;
	IDirectDrawSurface_AddRef(dst_surface);

	return DD_OK;
}

HRESULT __stdcall DirectDraw::SetCooperativeLevel(HWND hwnd, DWORD flags)
{
	return Wrapper::setCooperativeLevel(hwnd, flags);
}

HRESULT __stdcall DirectDraw::SetDisplayMode(DWORD width, DWORD height, DWORD bpp)
{
	return Wrapper::setDisplayMode(width, height, bpp);
}

}

_Check_return_ HRESULT WINAPI DirectDrawCreate(GUID FAR* lpGUID, LPDIRECTDRAW FAR* lplpDD, IUnknown FAR* pUnkOuter)
{
	auto ddraw = new d2gl::DirectDraw;

	*lplpDD = (LPDIRECTDRAW)ddraw;
	IDirectDraw_AddRef(ddraw);

	return DD_OK;
}