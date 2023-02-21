/*
	This file is "partly" of cnc-ddraw.
	Copyright (c) 2022 github.com/FunkyFr3sh
	https://github.com/FunkyFr3sh/cnc-ddraw/blob/master/src/ddsurface.c

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.
*/

#include "pch.h"
#include "surface.h"
#include "direct_draw.h"
#include "wrapper.h"

namespace d2gl {

DirectDrawSurface* DDrawSurface = nullptr;

DirectDrawSurface::DirectDrawSurface(LPDDSURFACEDESC surface_desc)
	: m_bpp(App.game.bpp == 0 ? 32 : App.game.bpp), m_flags(surface_desc->dwFlags), m_caps(surface_desc->ddsCaps.dwCaps)
{
	if (m_flags & DDSD_PIXELFORMAT) {
		switch (surface_desc->ddpfPixelFormat.dwRGBBitCount) {
			case 16: assert(false && "DDSD_PIXELFORMAT 16");
			case 8:
			case 32: m_bpp = surface_desc->ddpfPixelFormat.dwRGBBitCount; break;
		}
	}

	if (m_caps & DDSCAPS_PRIMARYSURFACE) {
		m_width = App.game.size.x;
		m_height = App.game.size.y;
	} else {
		m_width = surface_desc->dwWidth;
		m_height = surface_desc->dwHeight;
	}

	if (m_width && m_height) {
		m_xpitch = m_bpp / 8;
		m_ypitch = m_width * m_xpitch;

		m_bmi = new BitmapInfo256;
		m_bmi->header.biSize = sizeof(BITMAPINFOHEADER);
		m_bmi->header.biWidth = m_width;
		m_bmi->header.biHeight = -((int)m_height + 200);
		m_bmi->header.biPlanes = 1;
		m_bmi->header.biBitCount = (uint16_t)m_bpp;
		m_bmi->header.biCompression = m_bpp == 8 ? BI_RGB : BI_BITFIELDS;

		uint16_t clr_bits = m_bmi->header.biPlanes * m_bmi->header.biBitCount;

		if (clr_bits < 24)
			m_bmi->header.biClrUsed = (1 << clr_bits);

		m_bmi->header.biSizeImage = ((m_width * clr_bits + 31) & ~31) / 8 * m_height;

		if (m_bpp == 8) {
			for (size_t i = 0; i < 256; i++) {
				m_bmi->colors[i].rgbRed = (uint8_t)i;
				m_bmi->colors[i].rgbGreen = (uint8_t)i;
				m_bmi->colors[i].rgbBlue = (uint8_t)i;
				m_bmi->colors[i].rgbReserved = 0;
			}
		} else if (m_bpp == 16) {
			((uint32_t*)m_bmi->colors)[0] = 0xF800;
			((uint32_t*)m_bmi->colors)[1] = 0x07E0;
			((uint32_t*)m_bmi->colors)[2] = 0x001F;
			assert(false && "m_bpp 16");
		} else if (m_bpp == 32) {
			((uint32_t*)m_bmi->colors)[0] = 0xFF0000;
			((uint32_t*)m_bmi->colors)[1] = 0x00FF00;
			((uint32_t*)m_bmi->colors)[2] = 0x0000FF;
		}

		m_hdc = CreateCompatibleDC(App.hdc);
		m_bitmap = CreateDIBSection(m_hdc, (BITMAPINFO*)m_bmi, DIB_RGB_COLORS, (void**)&m_data, NULL, 0);

		m_bmi->header.biHeight = -((int)m_height);

		if (!m_bitmap)
			m_data = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, m_ypitch * (m_height + 200) * m_xpitch);

		if (m_caps & DDSCAPS_PRIMARYSURFACE)
			DDrawSurface = this;

		if (m_bitmap)
			SelectObject(m_hdc, m_bitmap);
	}

	if (m_flags & DDSD_BACKBUFFERCOUNT) {
		DDSURFACEDESC desc;
		memset(&desc, 0, sizeof(desc));

		if (surface_desc->dwBackBufferCount > 1) {
			desc.dwBackBufferCount = surface_desc->dwBackBufferCount - 1;
			desc.dwFlags |= DDSD_BACKBUFFERCOUNT;
		}

		desc.ddsCaps.dwCaps |= DDSCAPS_BACKBUFFER;
		desc.dwWidth = m_width;
		desc.dwHeight = m_height;

		m_back_buffer = new DirectDrawSurface(&desc);
		IDirectDrawSurface_AddRef(m_back_buffer);
	}
}

DirectDrawSurface::~DirectDrawSurface()
{
	if (DDrawSurface && (m_caps & DDSCAPS_PRIMARYSURFACE))
		DDrawSurface = nullptr;

	if (m_bitmap)
		DeleteObject(m_bitmap);
	else if (m_data)
		HeapFree(GetProcessHeap(), 0, m_data);

	if (m_hdc)
		DeleteDC(m_hdc);

	if (m_bmi)
		delete m_bmi;

	if (m_back_buffer)
		IDirectDrawSurface_Release(m_back_buffer);

	if (m_palette)
		IDirectDrawPalette_Release(m_palette);
}

ULONG __stdcall DirectDrawSurface::AddRef()
{
	m_ref++;
	return m_ref;
}

ULONG __stdcall DirectDrawSurface::Release()
{
	m_ref--;
	if (m_ref == 0) {
		delete this;
		return 0;
	}

	return m_ref;
}

HRESULT __stdcall DirectDrawSurface::Blt(LPRECT in_dst_rect, LPDIRECTDRAWSURFACE surface, LPRECT in_src_rect, DWORD flags, LPDDBLTFX blt_fx)
{
	const auto src_surface = (DirectDrawSurface*)surface;

	RECT src_rect = { 0, 0, src_surface ? src_surface->m_width : 0, src_surface ? src_surface->m_height : 0 };
	RECT dst_rect = { 0, 0, m_width, m_height };

	if (in_src_rect && src_surface)
		memcpy(&src_rect, in_src_rect, sizeof(src_rect));

	if (in_dst_rect)
		memcpy(&dst_rect, in_dst_rect, sizeof(dst_rect));

	if (src_surface) {
		if (src_rect.left < 0)
			src_rect.left = 0;

		if (src_rect.top < 0)
			src_rect.top = 0;

		if (src_rect.right > src_surface->m_width)
			src_rect.right = src_surface->m_width;

		if (src_rect.left > src_rect.right)
			src_rect.left = src_rect.right;

		if (src_rect.bottom > src_surface->m_height)
			src_rect.bottom = src_surface->m_height;

		if (src_rect.top > src_rect.bottom)
			src_rect.top = src_rect.bottom;
	}

	if (dst_rect.left < 0)
		dst_rect.left = 0;

	if (dst_rect.top < 0)
		dst_rect.top = 0;

	if (dst_rect.right > m_width)
		dst_rect.right = m_width;

	if (dst_rect.left > dst_rect.right)
		dst_rect.left = dst_rect.right;

	if (dst_rect.bottom > m_height)
		dst_rect.bottom = m_height;

	if (dst_rect.top > dst_rect.bottom)
		dst_rect.top = dst_rect.bottom;

	const int src_w = src_rect.right - src_rect.left;
	const int src_h = src_rect.bottom - src_rect.top;
	const int src_x = src_rect.left;
	const int src_y = src_rect.top;

	const int dst_w = dst_rect.right - dst_rect.left;
	const int dst_h = dst_rect.bottom - dst_rect.top;
	const int dst_x = dst_rect.left;
	const int dst_y = dst_rect.top;

	if (m_data && (flags & DDBLT_COLORFILL) && dst_w > 0 && dst_h > 0) {
		uint8_t* dst = (uint8_t*)m_data + (dst_x * m_xpitch) + (m_ypitch * dst_y);
		uint8_t* first_row = dst;
		uint32_t dst_pitch = dst_w * m_xpitch;

		if (m_bpp == 8) {
			const auto color = (uint8_t)blt_fx->dwFillColor;

			for (int i = 0; i < dst_h; i++) {
				memset(dst, color, dst_pitch);
				dst += m_ypitch;
			}
		} else if (m_bpp == 16) // Unused
		{
			auto row1 = (uint16_t*)dst;
			const auto color = (uint16_t)blt_fx->dwFillColor;

			if ((color & 0xFF) == (color >> 8)) {
				const auto c8 = (uint8_t)(color & 0xFF);

				for (int i = 0; i < dst_h; i++) {
					memset(dst, c8, dst_pitch);
					dst += m_ypitch;
				}
			} else {
				for (int i = 0; i < dst_w; i++)
					row1[i] = color;

				for (int i = 1; i < dst_h; i++) {
					dst += m_ypitch;
					memcpy(dst, first_row, dst_pitch);
				}
			}
			assert(false && "m_bpp 16");
		} else if (m_bpp == 32) {
			auto row1 = (uint32_t*)dst;
			const auto color = blt_fx->dwFillColor;

			if ((color & 0xFF) == ((color >> 8) & 0xFF) && (color & 0xFF) == ((color >> 16) & 0xFF) && (color & 0xFF) == ((color >> 24) & 0xFF)) {
				const auto c8 = (uint8_t)(color & 0xFF);

				for (int i = 0; i < dst_h; i++) {
					memset(dst, c8, dst_pitch);
					dst += m_ypitch;
				}
			} else {
				for (int i = 0; i < dst_w; i++)
					row1[i] = color;

				for (int i = 1; i < dst_h; i++) {
					dst += m_ypitch;
					memcpy(dst, first_row, dst_pitch);
				}
			}
		}
	}

	return DD_OK;
}

HRESULT __stdcall DirectDrawSurface::BltFast(DWORD x, DWORD y, LPDIRECTDRAWSURFACE surface, LPRECT in_src_rect, DWORD flags)
{
	const auto src_surface = (DirectDrawSurface*)surface;

	RECT src_rect = { 0, 0, src_surface ? src_surface->m_width : 0, src_surface ? src_surface->m_height : 0 };

	if (in_src_rect && src_surface) {
		memcpy(&src_rect, in_src_rect, sizeof(src_rect));

		if (src_rect.left < 0)
			src_rect.left = 0;

		if (src_rect.top < 0)
			src_rect.top = 0;

		if (src_rect.right > src_surface->m_width)
			src_rect.right = src_surface->m_width;

		if (src_rect.left > src_rect.right)
			src_rect.left = src_rect.right;

		if (src_rect.bottom > src_surface->m_height)
			src_rect.bottom = src_surface->m_height;

		if (src_rect.top > src_rect.bottom)
			src_rect.top = src_rect.bottom;
	}

	const int src_x = src_rect.left;
	const int src_y = src_rect.top;

	RECT dst_rect = { (LONG)x, (LONG)y, (src_rect.right - src_rect.left) + (LONG)x, (src_rect.bottom - src_rect.top) + (LONG)y };

	if (dst_rect.left < 0)
		dst_rect.left = 0;

	if (dst_rect.top < 0)
		dst_rect.top = 0;

	if (dst_rect.right > m_width)
		dst_rect.right = m_width;

	if (dst_rect.left > dst_rect.right)
		dst_rect.left = dst_rect.right;

	if (dst_rect.bottom > m_height)
		dst_rect.bottom = m_height;

	if (dst_rect.top > dst_rect.bottom)
		dst_rect.top = dst_rect.bottom;

	const int dst_x = dst_rect.left;
	const int dst_y = dst_rect.top;
	const int dst_w = dst_rect.right - dst_rect.left;
	const int dst_h = dst_rect.bottom - dst_rect.top;

	if (src_surface && dst_w > 0 && dst_h > 0) {
		auto src = (uint8_t*)src_surface->m_data + (src_x * src_surface->m_xpitch) + (src_surface->m_ypitch * src_y);
		auto dst = (uint8_t*)m_data + (dst_x * m_xpitch) + (m_ypitch * dst_y);
		uint32_t dst_pitch = dst_w * m_xpitch;

		for (int i = 0; i < dst_h; i++) {
			memcpy(dst, src, dst_pitch);
			src += src_surface->m_ypitch;
			dst += m_ypitch;
		}
	}

	return DD_OK;
}

HRESULT __stdcall DirectDrawSurface::Flip(LPDIRECTDRAWSURFACE surface, DWORD flags)
{
	if (m_back_buffer) {
		const auto back_buffer = surface ? (DirectDrawSurface*)surface : m_back_buffer;

		void* surface_data = m_data;
		HBITMAP bitmap = m_bitmap;
		HDC hdc = m_hdc;

		m_data = back_buffer->m_data;
		m_bitmap = back_buffer->m_bitmap;
		m_hdc = back_buffer->m_hdc;

		back_buffer->m_data = surface_data;
		back_buffer->m_bitmap = bitmap;
		back_buffer->m_hdc = hdc;

		if (!surface && m_back_buffer->m_back_buffer)
			m_back_buffer->Flip(NULL, 0);
	}

	if (m_caps & DDSCAPS_PRIMARYSURFACE)
		DDrawWrapper->onBufferSwap(true);

	return DD_OK;
}

HRESULT __stdcall DirectDrawSurface::GetAttachedSurface(LPDDSCAPS caps, LPDIRECTDRAWSURFACE FAR* surface)
{
	if ((m_caps & DDSCAPS_PRIMARYSURFACE) && (m_caps & DDSCAPS_FLIP) && (caps->dwCaps & DDSCAPS_BACKBUFFER)) {
		if (m_back_buffer) {
			IDirectDrawSurface_AddRef(m_back_buffer);
			*surface = m_back_buffer;
		} else {
			IDirectDrawSurface_AddRef(this);
			*surface = this;
		}
	}

	return DD_OK;
}

HRESULT __stdcall DirectDrawSurface::GetPixelFormat(LPDDPIXELFORMAT pixel_format)
{
	if (pixel_format) {
		memset(pixel_format, 0, sizeof(DDPIXELFORMAT));

		pixel_format->dwSize = sizeof(DDPIXELFORMAT);
		pixel_format->dwFlags = DDPF_RGB;
		pixel_format->dwRGBBitCount = m_bpp;

		if (m_bpp == 8) {
			pixel_format->dwFlags |= DDPF_PALETTEINDEXED8;
		} else if (m_bpp == 16) {
			pixel_format->dwRBitMask = 0xF800;
			pixel_format->dwGBitMask = 0x07E0;
			pixel_format->dwBBitMask = 0x001F;
			assert(false && "m_bpp 16");
		} else if (m_bpp == 32) {
			pixel_format->dwRBitMask = 0xFF0000;
			pixel_format->dwGBitMask = 0x00FF00;
			pixel_format->dwBBitMask = 0x0000FF;
		}

		return DD_OK;
	}

	return DDERR_INVALIDPARAMS;
}

HRESULT __stdcall DirectDrawSurface::GetSurfaceDesc(LPDDSURFACEDESC surface_desc)
{
	if (surface_desc) {
		const size_t size = surface_desc->dwSize == sizeof(DDSURFACEDESC2) ? sizeof(DDSURFACEDESC2) : sizeof(DDSURFACEDESC);
		memset(surface_desc, 0, size);

		surface_desc->dwSize = size;
		surface_desc->dwFlags = DDSD_WIDTH | DDSD_HEIGHT | DDSD_PITCH | DDSD_PIXELFORMAT | DDSD_LPSURFACE;
		surface_desc->dwWidth = m_width;
		surface_desc->dwHeight = m_height;
		surface_desc->lPitch = m_ypitch;
		surface_desc->lpSurface = m_data;
		surface_desc->ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);
		surface_desc->ddpfPixelFormat.dwFlags = DDPF_RGB;
		surface_desc->ddpfPixelFormat.dwRGBBitCount = m_bpp;
		surface_desc->ddsCaps.dwCaps = m_caps;

		if (m_caps & (DDSCAPS_PRIMARYSURFACE | DDSCAPS_BACKBUFFER))
			surface_desc->ddsCaps.dwCaps |= DDSCAPS_VIDEOMEMORY;

		if (m_bpp == 8) {
			surface_desc->ddpfPixelFormat.dwFlags |= DDPF_PALETTEINDEXED8;
		} else if (m_bpp == 16) {
			surface_desc->ddpfPixelFormat.dwRBitMask = 0xF800;
			surface_desc->ddpfPixelFormat.dwGBitMask = 0x07E0;
			surface_desc->ddpfPixelFormat.dwBBitMask = 0x001F;
			assert(false && "m_bpp 16");
		} else if (m_bpp == 32) {
			surface_desc->ddpfPixelFormat.dwRBitMask = 0xFF0000;
			surface_desc->ddpfPixelFormat.dwGBitMask = 0x00FF00;
			surface_desc->ddpfPixelFormat.dwBBitMask = 0x0000FF;
		}

		return DD_OK;
	}

	return DDERR_INVALIDPARAMS;
}

HRESULT __stdcall DirectDrawSurface::Lock(LPRECT dst_rect, LPDDSURFACEDESC surface_desc, DWORD flags, HANDLE event)
{
	DDrawWrapper->onBufferClear();

	return GetSurfaceDesc(surface_desc);
}

HRESULT __stdcall DirectDrawSurface::SetPalette(LPDIRECTDRAWPALETTE palette)
{
	if (palette) {
		IDirectDrawPalette_AddRef(palette);

		if (m_palette)
			IDirectDrawPalette_Release(m_palette);

		m_palette = (DirectDrawPalette*)palette;

		if (m_caps & DDSCAPS_PRIMARYSURFACE)
			DDrawWrapper->updatePalette(m_palette->getData());
	}

	return DD_OK;
}

HRESULT __stdcall DirectDrawSurface::Unlock(LPVOID rect)
{
	DDrawWrapper->onBufferSwap();

	return DD_OK;
}

}