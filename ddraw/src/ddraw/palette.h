#pragma once

#include <ddraw.h>

namespace d2gl {

struct DirectDrawPalette : public IDirectDrawPalette {
	ULONG m_ref = 0;
	RGBQUAD m_data_rgb[256];
	glm::vec4 m_data[256];
	DWORD m_flags = 0;

	HRESULT __stdcall QueryInterface(REFIID riid, LPVOID FAR* object);
	ULONG __stdcall AddRef();
	ULONG __stdcall Release();

	HRESULT __stdcall GetCaps(LPDWORD caps);
	HRESULT __stdcall GetEntries(DWORD flags, DWORD base, DWORD num_entries, LPPALETTEENTRY entries);
	HRESULT __stdcall Initialize(LPDIRECTDRAW ddraw, DWORD flags, LPPALETTEENTRY color_table);
	HRESULT __stdcall SetEntries(DWORD flags, DWORD starting_entry, DWORD count, LPPALETTEENTRY entries);
};

}