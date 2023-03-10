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
#include "direct_draw.h"
#include "helpers.h"
#include "palette.h"
#include "surface.h"
#include "wrapper.h"

namespace d2gl {

HRESULT __stdcall DirectDraw::QueryInterface(REFIID riid, LPVOID FAR* ppvObj)
{
	if (IsEqualGUID(IID_IDirectDraw4, riid)) {
		auto ddraw = new d2gl::DirectDraw;

		*ppvObj = (LPDIRECTDRAW4)ddraw;
		IDirectDraw_AddRef(ddraw);

		return S_OK;
	}

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

HRESULT __stdcall DirectDraw::EnumDisplayModes(DWORD flags, LPDDSURFACEDESC surface_desc, LPVOID context, LPDDENUMMODESCALLBACK enum_modes_callback)
{
	std::vector<std::pair<DWORD, DWORD>> resolutions = { { 640, 480 }, { 800, 600 } };

	for (auto& res : resolutions) {
		DDSURFACEDESC desc = { 0 };
		desc.dwSize = sizeof(DDSURFACEDESC);
		desc.dwFlags = DDSD_HEIGHT | DDSD_WIDTH | DDSD_REFRESHRATE | DDSD_PITCH | DDSD_PIXELFORMAT;
		desc.dwRefreshRate = 60;
		desc.dwWidth = res.first;
		desc.dwHeight = res.second;
		desc.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);

		desc.ddpfPixelFormat.dwFlags = DDPF_PALETTEINDEXED8 | DDPF_RGB;
		desc.ddpfPixelFormat.dwRGBBitCount = 8;
		desc.lPitch = ((res.first * 8 + 31) & ~31) >> 3;
		enum_modes_callback((LPDDSURFACEDESC)&desc, context);

		desc.ddpfPixelFormat.dwFlags = DDPF_RGB;
		desc.ddpfPixelFormat.dwRGBBitCount = 16;
		desc.ddpfPixelFormat.dwRBitMask = 0xF800;
		desc.ddpfPixelFormat.dwGBitMask = 0x07E0;
		desc.ddpfPixelFormat.dwBBitMask = 0x001F;
		desc.lPitch = ((res.first * 16 + 31) & ~31) >> 3;
		enum_modes_callback((LPDDSURFACEDESC)&desc, context);

		desc.ddpfPixelFormat.dwFlags = DDPF_RGB;
		desc.ddpfPixelFormat.dwRGBBitCount = 32;
		desc.ddpfPixelFormat.dwRBitMask = 0xFF0000;
		desc.ddpfPixelFormat.dwGBitMask = 0x00FF00;
		desc.ddpfPixelFormat.dwBBitMask = 0x0000FF;
		desc.lPitch = ((res.first * 32 + 31) & ~31) >> 3;
		enum_modes_callback((LPDDSURFACEDESC)&desc, context);
	}

	return DD_OK;
}

HRESULT __stdcall DirectDraw::GetCaps(LPDDCAPS driver_caps, LPDDCAPS enum_caps)
{
	if (driver_caps) {
		driver_caps->dwCaps = DDCAPS_BLT | DDCAPS_PALETTE | DDCAPS_BLTCOLORFILL | DDCAPS_BLTSTRETCH | DDCAPS_CANCLIP | DDCAPS_CANBLTSYSMEM;
		driver_caps->dwPalCaps = DDPCAPS_8BIT | DDPCAPS_PRIMARYSURFACE;
		driver_caps->dwVidMemTotal = 16777216;
		driver_caps->dwVidMemFree = 16777216;
		driver_caps->ddsCaps.dwCaps = DDSCAPS_FLIP;
	}

	return DD_OK;
}

HRESULT __stdcall DirectDraw::SetCooperativeLevel(HWND hwnd, DWORD flags)
{
	return Wrapper::setCooperativeLevel(hwnd, flags);
}

HRESULT __stdcall DirectDraw::SetDisplayMode(DWORD width, DWORD height, DWORD bpp)
{
	return Wrapper::setDisplayMode(width, height, bpp == 16 ? 32 : bpp);
}

HRESULT __stdcall DirectDraw::GetDeviceIdentifier(LPDDDEVICEIDENTIFIER device_idenfitier, DWORD flags)
{
	if (!device_idenfitier)
		return DDERR_INVALIDPARAMS;

	memset(device_idenfitier, 0, sizeof(DDDEVICEIDENTIFIER));
	sprintf_s(device_idenfitier->szDescription, "D2GL");

	return DD_OK;
}

}

_Check_return_ HRESULT WINAPI DirectDrawCreate(GUID FAR* lpGUID, LPDIRECTDRAW FAR* lplpDD, IUnknown FAR* pUnkOuter)
{
	auto ddraw = new d2gl::DirectDraw;

	*lplpDD = (LPDIRECTDRAW)ddraw;
	IDirectDraw_AddRef(ddraw);

	return DD_OK;
}

_Check_return_ HRESULT WINAPI DirectDrawEnumerateA(LPDDENUMCALLBACKA lpCallback, LPVOID lpContext)
{
	if (d2gl::helpers::getVersion() == d2gl::Version::V_114d) {
		MessageBoxA(NULL, "Diablo 2 v1.14d doesn't not support DirectDraw mode!\nDelete ddraw.dll and re-launch the game.", "Unsupported mode!", MB_OK | MB_ICONWARNING);
		exit(1);
	}

	if (lpCallback)
		lpCallback(NULL, (LPSTR)("D2GL"), (LPSTR)(""), lpContext);

	return DD_OK;
}