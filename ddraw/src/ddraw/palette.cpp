#include "pch.h"
#include "palette.h"
#include "surface.h"
#include "wrapper.h"

namespace d2gl {

HRESULT __stdcall DirectDrawPalette::QueryInterface(REFIID riid, LPVOID FAR* object)
{
	return DDERR_UNSUPPORTED;
}

ULONG __stdcall DirectDrawPalette::AddRef()
{
	m_ref++;
	return m_ref;
}

ULONG __stdcall DirectDrawPalette::Release()
{
	m_ref--;
	if (m_ref == 0) {
		delete this;
		return 0;
	}

	return m_ref;
}

HRESULT __stdcall DirectDrawPalette::GetCaps(LPDWORD caps)
{
	return DDERR_UNSUPPORTED;
}

HRESULT __stdcall DirectDrawPalette::GetEntries(DWORD flags, DWORD base, DWORD num_entries, LPPALETTEENTRY entries)
{
	for (size_t i = base, x = 0; i < base + num_entries; i++, x++) {
		entries[x].peRed = m_data_rgb[i].rgbRed;
		entries[x].peGreen = m_data_rgb[i].rgbGreen;
		entries[x].peBlue = m_data_rgb[i].rgbBlue;
	}

	return DD_OK;
}

HRESULT __stdcall DirectDrawPalette::Initialize(LPDIRECTDRAW ddraw, DWORD flags, LPPALETTEENTRY color_table)
{
	return DDERR_UNSUPPORTED;
}

HRESULT __stdcall DirectDrawPalette::SetEntries(DWORD flags, DWORD starting_entry, DWORD count, LPPALETTEENTRY entries)
{
	for (size_t i = starting_entry, x = 0; i < starting_entry + count; i++, x++) {
		m_data_rgb[i].rgbRed = entries[x].peRed;
		m_data_rgb[i].rgbGreen = entries[x].peGreen;
		m_data_rgb[i].rgbBlue = entries[x].peBlue;
		m_data_rgb[i].rgbReserved = 0;

		m_data[i] = { (float)entries[x].peRed / 255.0, (float)entries[x].peGreen / 255.0, (float)entries[x].peBlue / 255.0, 1.0f };
	}

	if (!(m_flags & DDPCAPS_ALLOW256)) {
		m_data_rgb[0].rgbRed = 0;
		m_data_rgb[0].rgbGreen = 0;
		m_data_rgb[0].rgbBlue = 0;
		m_data_rgb[0].rgbReserved = 0;

		m_data_rgb[255].rgbRed = 255;
		m_data_rgb[255].rgbGreen = 255;
		m_data_rgb[255].rgbBlue = 255;
		m_data_rgb[255].rgbReserved = 0;

		m_data[0] = { 0.0f, 0.0f, 0.0f, 1.0f };
		m_data[255] = { 1.0f, 1.0f, 1.0f, 1.0f };
	}

	if (DDrawSurface && DDrawSurface->m_palette == this)
		DDrawWrapper->updatePalette(m_data);

	return DD_OK;
}

}