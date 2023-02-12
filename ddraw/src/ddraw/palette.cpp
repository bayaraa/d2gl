#include "pch.h"
#include "palette.h"
#include "surface.h"
#include "wrapper.h"

namespace d2gl {

DirectDrawPalette::DirectDrawPalette(DWORD flags, LPPALETTEENTRY palette_entry)
	: m_flags(flags)
{
	SetEntries(flags, 0, 256, palette_entry);
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

HRESULT __stdcall DirectDrawPalette::SetEntries(DWORD flags, DWORD starting_entry, DWORD count, LPPALETTEENTRY entries)
{
	for (size_t i = starting_entry, x = 0; i < starting_entry + count; i++, x++) {
		m_data[i].r = (float)entries[x].peRed / 255.0f;
		m_data[i].g = (float)entries[x].peGreen / 255.0f;
		m_data[i].b = (float)entries[x].peBlue / 255.0f;
		m_data[i].a = 1.0f;
	}

	if (!(m_flags & DDPCAPS_ALLOW256)) {
		m_data[0] = { 0.0f, 0.0f, 0.0f, 1.0f };
		m_data[255] = { 1.0f, 1.0f, 1.0f, 1.0f };
	}

	if (DDrawSurface && DDrawSurface->getPalette() == this)
		DDrawWrapper->updatePalette(m_data);

	return DD_OK;
}

}