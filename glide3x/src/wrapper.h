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

#pragma once

#include "glide/texture_manager.h"

#define __MSC__
#include <glide3/glide.h>

namespace d2gl {

#define GLIDE_MAX_NUM_TMU 3
#define GLIDE_TEX_MEMORY 16 * 1024 * 1024

class Wrapper;
extern std::unique_ptr<Wrapper> GlideWrapper;

class Wrapper {
	Context* ctx;
	bool m_swapped = true;
	uint32_t m_gamma_hash = 0;
	GrLfbInfo_t m_movie_buffer = { 0 };
	std::unique_ptr<TextureManager> m_texture_manager;

public:
	Wrapper();
	~Wrapper();

	void onBufferClear();
	void onBufferSwap();

	void grDrawPoint(const void* pt);
	void grDrawLine(const void* v1, const void* v2);
	void grDrawVertexArray(FxU32 mode, FxU32 count, void** pointers);
	void grDrawVertexArrayContiguous(FxU32 mode, FxU32 count, void* pointers);
	void grAlphaBlendFunction(GrAlphaBlendFnc_t rgb_df);
	void grAlphaCombine(GrCombineFunction_t function);
	void grChromakeyMode(GrChromakeyMode_t mode);
	void grColorCombine(GrCombineFunction_t function);
	void grConstantColorValue(GrColor_t value);
	void grLoadGammaTable(FxU32 nentries, FxU32* red, FxU32* green, FxU32* blue);
	void guGammaCorrectionRGB(FxFloat red, FxFloat green, FxFloat blue);
	void grTexSource(GrChipID_t tmu, FxU32 start_address, GrTexInfo* info);
	void grTexDownloadMipMap(GrChipID_t tmu, FxU32 start_address, GrTexInfo* info);
	void grTexDownloadTable(void* data);
	FxBool grLfbLock(GrLfbWriteMode_t write_mode, GrOriginLocation_t origin, GrLfbInfo_t* info);
	FxBool grLfbUnlock();

	static GrContext_t grSstWinOpen(FxU32 hwnd, GrScreenResolution_t screen_resolution);
	static FxU32 grGet(FxU32 pname, FxI32& params);
	static const char* grGetString(FxU32 pname);

	static uint32_t getTexSize(GrTexInfo* info, uint32_t& width, uint32_t& height);
};

}