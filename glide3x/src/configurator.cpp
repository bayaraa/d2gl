/*
SlashGaming Diablo II Free Resolution utilizes the API of D2DX Configurator.
This file is partially part of D2DX.
This is the text from D2DX's file's license header:

Copyright (C) 2021  Bolrog

D2DX is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

D2DX is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with D2DX. If not, see <https://www.gnu.org/licenses/>.
*/

#include "pch.h"

MIDL_INTERFACE("B11C5FA4-983F-4E34-9E43-BD82F9CCDB65")
ID2DXConfigurator : public IUnknown
{
public:
	virtual HRESULT STDMETHODCALLTYPE SetCustomResolution(int width, int height) = 0;
	virtual HRESULT STDMETHODCALLTYPE GetSuggestedCustomResolution(int* width, int* height) = 0;
};

class Configurator : public ID2DXConfigurator {
public:
	Configurator() noexcept {}
	virtual ~Configurator() noexcept {}

	STDMETHOD(QueryInterface)
	(REFIID riid, _COM_Outptr_ void __RPC_FAR* __RPC_FAR* ppvObject)
	{
		*ppvObject = this;
		return S_OK;
	}

	STDMETHOD_(ULONG, AddRef)
	(void)
	{
		return 1;
	}

	STDMETHOD_(ULONG, Release)
	(void)
	{
		return 0;
	}

	STDMETHOD(SetCustomResolution)
	(int32_t width, int32_t height) noexcept
	{
		d2gl::App.game.custom_size = { width, height };
		return S_OK;
	}

	STDMETHOD(GetSuggestedCustomResolution)
	(int32_t* width, int32_t* height) noexcept
	{
		return S_OK;
	}
};

#ifdef __cplusplus
extern "C" {
#endif

__declspec(dllexport) Configurator* __stdcall D2DXGetConfigurator()
{
	static Configurator configurator;
	return &configurator;
}

#ifdef __cplusplus
}
#endif