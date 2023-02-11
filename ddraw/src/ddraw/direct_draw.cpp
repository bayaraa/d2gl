#include "pch.h"
#include "direct_draw.h"
#include "palette.h"
#include "surface.h"
#include "wrapper.h"

namespace d2gl {

DirectDraw* DDraw = nullptr;

HRESULT __stdcall DirectDraw::QueryInterface(REFIID riid, LPVOID FAR* object)
{
	return DDERR_UNSUPPORTED;
}

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

HRESULT __stdcall DirectDraw::Compact()
{
	return DDERR_UNSUPPORTED;
}

HRESULT __stdcall DirectDraw::CreateClipper(DWORD flags, LPDIRECTDRAWCLIPPER FAR* clipper, IUnknown FAR* unknown)
{
	return DDERR_UNSUPPORTED;
}

HRESULT __stdcall DirectDraw::CreatePalette(DWORD flags, LPPALETTEENTRY palette_entry, LPDIRECTDRAWPALETTE FAR* palette, IUnknown FAR* unknown)
{
	if (!palette || !palette_entry)
		return DDERR_INVALIDPARAMS;

	auto dd_palette = new DirectDrawPalette;
	dd_palette->m_flags = flags;

	dd_palette->SetEntries(flags, 0, 256, palette_entry);
	IDirectDrawPalette_AddRef(dd_palette);

	*palette = (LPDIRECTDRAWPALETTE)dd_palette;

	return DD_OK;
}

HRESULT __stdcall DirectDraw::CreateSurface(LPDDSURFACEDESC surface_desc, LPDIRECTDRAWSURFACE FAR* surface, IUnknown FAR* unknown)
{
	auto dst_surface = new DirectDrawSurface;

	surface_desc->dwFlags |= DDSD_CAPS;

	dst_surface->m_bpp = App.game.bpp == 0 ? 16 : App.game.bpp;
	dst_surface->m_flags = surface_desc->dwFlags;
	dst_surface->m_caps = surface_desc->ddsCaps.dwCaps;

	if (dst_surface->m_flags & DDSD_PIXELFORMAT) {
		switch (surface_desc->ddpfPixelFormat.dwRGBBitCount) {
			case 8:
			case 16:
			case 32: dst_surface->m_bpp = surface_desc->ddpfPixelFormat.dwRGBBitCount; break;
		}
	}

	if (dst_surface->m_caps & DDSCAPS_PRIMARYSURFACE) {
		dst_surface->m_width = App.game.size.x;
		dst_surface->m_height = App.game.size.y;
	} else {
		dst_surface->m_width = surface_desc->dwWidth;
		dst_surface->m_height = surface_desc->dwHeight;
	}

	if (dst_surface->m_width && dst_surface->m_height) {
		dst_surface->m_xpitch = dst_surface->m_bpp / 8;
		dst_surface->m_ypitch = dst_surface->m_width * dst_surface->m_xpitch;

		dst_surface->m_bmi = new BitmapInfo256;
		dst_surface->m_bmi->header.biSize = sizeof(BITMAPINFOHEADER);
		dst_surface->m_bmi->header.biWidth = dst_surface->m_width;
		dst_surface->m_bmi->header.biHeight = -((int)dst_surface->m_height + 200);
		dst_surface->m_bmi->header.biPlanes = 1;
		dst_surface->m_bmi->header.biBitCount = (uint16_t)dst_surface->m_bpp;
		dst_surface->m_bmi->header.biCompression = dst_surface->m_bpp == 8 ? BI_RGB : BI_BITFIELDS;

		uint16_t clr_bits = dst_surface->m_bmi->header.biPlanes * dst_surface->m_bmi->header.biBitCount;

		if (clr_bits < 24)
			dst_surface->m_bmi->header.biClrUsed = (1 << clr_bits);

		dst_surface->m_bmi->header.biSizeImage = ((dst_surface->m_width * clr_bits + 31) & ~31) / 8 * dst_surface->m_height;

		if (dst_surface->m_bpp == 8) {
			for (size_t i = 0; i < 256; i++) {
				dst_surface->m_bmi->colors[i].rgbRed = (uint8_t)i;
				dst_surface->m_bmi->colors[i].rgbGreen = (uint8_t)i;
				dst_surface->m_bmi->colors[i].rgbBlue = (uint8_t)i;
				dst_surface->m_bmi->colors[i].rgbReserved = 0;
			}
		} else if (dst_surface->m_bpp == 16) {
			((uint32_t*)dst_surface->m_bmi->colors)[0] = 0xF800;
			((uint32_t*)dst_surface->m_bmi->colors)[1] = 0x07E0;
			((uint32_t*)dst_surface->m_bmi->colors)[2] = 0x001F;
		} else if (dst_surface->m_bpp == 32) {
			((uint32_t*)dst_surface->m_bmi->colors)[0] = 0xFF0000;
			((uint32_t*)dst_surface->m_bmi->colors)[1] = 0x00FF00;
			((uint32_t*)dst_surface->m_bmi->colors)[2] = 0x0000FF;
		}

		dst_surface->m_hdc = CreateCompatibleDC(App.hdc);
		dst_surface->m_bitmap = CreateDIBSection(dst_surface->m_hdc, (BITMAPINFO*)dst_surface->m_bmi, DIB_RGB_COLORS, (void**)&dst_surface->m_data, NULL, 0);

		dst_surface->m_bmi->header.biHeight = -((int)dst_surface->m_height);

		if (!dst_surface->m_bitmap)
			dst_surface->m_data = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dst_surface->m_ypitch * (dst_surface->m_height + 200) * dst_surface->m_xpitch);

		if (dst_surface->m_caps & DDSCAPS_PRIMARYSURFACE)
			DDrawSurface = dst_surface;

		if (dst_surface->m_bitmap)
			SelectObject(dst_surface->m_hdc, dst_surface->m_bitmap);
	}

	if (dst_surface->m_flags & DDSD_BACKBUFFERCOUNT) {
		DDSURFACEDESC desc;
		memset(&desc, 0, sizeof(desc));

		if (surface_desc->dwBackBufferCount > 1) {
			desc.dwBackBufferCount = surface_desc->dwBackBufferCount - 1;
			desc.dwFlags |= DDSD_BACKBUFFERCOUNT;
		}

		desc.ddsCaps.dwCaps |= DDSCAPS_BACKBUFFER;
		desc.dwWidth = dst_surface->m_width;
		desc.dwHeight = dst_surface->m_height;

		CreateSurface(&desc, (LPDIRECTDRAWSURFACE*)&dst_surface->m_back_buffer, unknown);
	}

	*surface = dst_surface;
	IDirectDrawSurface_AddRef(dst_surface);

	return DD_OK;
}

HRESULT __stdcall DirectDraw::DuplicateSurface(LPDIRECTDRAWSURFACE src_surface, LPDIRECTDRAWSURFACE FAR* dst_surface)
{
	return DDERR_UNSUPPORTED;
}

HRESULT __stdcall DirectDraw::EnumDisplayModes(DWORD flags, LPDDSURFACEDESC surface_desc, LPVOID context, LPDDENUMMODESCALLBACK enum_modes_callback)
{
	return DDERR_UNSUPPORTED;
}

HRESULT __stdcall DirectDraw::EnumSurfaces(DWORD flags, LPDDSURFACEDESC surface_desc, LPVOID context, LPDDENUMSURFACESCALLBACK enum_surfaces_callback)
{
	return DDERR_UNSUPPORTED;
}

HRESULT __stdcall DirectDraw::FlipToGDISurface()
{
	return DDERR_UNSUPPORTED;
}

HRESULT __stdcall DirectDraw::GetCaps(LPDDCAPS driver_caps, LPDDCAPS emul_caps)
{
	return DDERR_UNSUPPORTED;
}

HRESULT __stdcall DirectDraw::GetDisplayMode(LPDDSURFACEDESC surface_desc)
{
	return DDERR_UNSUPPORTED;
}

HRESULT __stdcall DirectDraw::GetFourCCCodes(LPDWORD num_codes, LPDWORD codes)
{
	return DDERR_UNSUPPORTED;
}

HRESULT __stdcall DirectDraw::GetGDISurface(LPDIRECTDRAWSURFACE FAR* gdi_surface)
{
	return DDERR_UNSUPPORTED;
}

HRESULT __stdcall DirectDraw::GetMonitorFrequency(LPDWORD frequency)
{
	return DDERR_UNSUPPORTED;
}

HRESULT __stdcall DirectDraw::GetScanLine(LPDWORD scan_line)
{
	return DDERR_UNSUPPORTED;
}

HRESULT __stdcall DirectDraw::GetVerticalBlankStatus(LPBOOL in_vertical_blank)
{
	return DDERR_UNSUPPORTED;
}

HRESULT __stdcall DirectDraw::Initialize(GUID* guid)
{
	return DDERR_UNSUPPORTED;
}

HRESULT __stdcall DirectDraw::RestoreDisplayMode()
{
	return DDERR_UNSUPPORTED;
}

HRESULT __stdcall DirectDraw::SetCooperativeLevel(HWND hwnd, DWORD flags)
{
	return Wrapper::setCooperativeLevel(hwnd, flags);
}

HRESULT __stdcall DirectDraw::SetDisplayMode(DWORD width, DWORD height, DWORD bpp)
{
	return Wrapper::setDisplayMode(width, height, bpp);
}

HRESULT __stdcall DirectDraw::WaitForVerticalBlank(DWORD flags, HANDLE event)
{
	return DD_OK;
}

}

_Check_return_ HRESULT WINAPI DirectDrawCreate(GUID FAR* lpGUID, LPDIRECTDRAW FAR* lplpDD, IUnknown FAR* pUnkOuter)
{
	d2gl::DDraw = new d2gl::DirectDraw;
	IDirectDraw_AddRef(d2gl::DDraw);

	*lplpDD = (LPDIRECTDRAW)d2gl::DDraw;

	return DD_OK;
}