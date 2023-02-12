#pragma once

#include <ddraw.h>

namespace d2gl {

// clang-format off
class DirectDrawPalette : public IDirectDrawPalette {
	ULONG m_ref = 0;

	DWORD m_flags = 0;
	glm::vec4 m_data[256];

public:
	DirectDrawPalette(DWORD flags, LPPALETTEENTRY palette_entry);
	~DirectDrawPalette() = default;

	inline const glm::vec4* getData() { return m_data; }

	/*** IUnknown methods ***/
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) { return DDERR_UNSUPPORTED; }
	STDMETHOD_(ULONG, AddRef)(THIS);
	STDMETHOD_(ULONG, Release)(THIS);
	/*** IDirectDrawPalette methods ***/
	STDMETHOD(GetCaps)(THIS_ LPDWORD) { return DDERR_UNSUPPORTED; }
	STDMETHOD(GetEntries)(THIS_ DWORD, DWORD, DWORD, LPPALETTEENTRY) { return DD_OK; }
	STDMETHOD(Initialize)(THIS_ LPDIRECTDRAW, DWORD, LPPALETTEENTRY) { return DDERR_UNSUPPORTED; }
	STDMETHOD(SetEntries)(THIS_ DWORD, DWORD, DWORD, LPPALETTEENTRY);
};
// clang-format on

}