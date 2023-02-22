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

#include <ddraw.h>

namespace d2gl {

class Wrapper;

extern const char* g_shader_game;
extern std::unique_ptr<Wrapper> DDrawWrapper;

class Wrapper {
	Context* ctx;
	bool m_swapped = true;

	std::unique_ptr<Texture> m_ddraw_texture;

	std::unique_ptr<Pipeline> m_movie_pipeline;

	std::unique_ptr<UniformBuffer> m_game_palette_ubo;
	std::unique_ptr<FrameBuffer> m_game_framebuffer;
	std::unique_ptr<Pipeline> m_game_pipeline;

	std::unique_ptr<UniformBuffer> m_upscale_ubo;
	std::unique_ptr<Texture> m_upscale_texture;
	std::unique_ptr<Pipeline> m_upscale_pipeline;
	int m_current_shader = -1;

	glm::vec3 m_sharpen_data;
	glm::uvec2 m_fxaa_work_size = { 0, 0 };
	std::unique_ptr<UniformBuffer> m_postfx_ubo;
	std::unique_ptr<Texture> m_postfx_texture;
	std::unique_ptr<FrameBuffer> m_postfx_framebuffer;
	std::unique_ptr<Pipeline> m_postfx_pipeline;
	std::unique_ptr<Pipeline> m_fxaa_compute_pipeline;

	std::unique_ptr<Pipeline> m_mod_pipeline;

public:
	Wrapper();
	~Wrapper() = default;

	void onResize();
	void onShaderChange(bool texture = false);
	void onStageChange();
	void onBufferClear();
	void onBufferSwap(bool flip = false);

	inline void updatePalette(const glm::vec4* data) { m_game_palette_ubo->updateData("palette", data); }

	static HRESULT setCooperativeLevel(HWND hwnd, DWORD flags);
	static HRESULT setDisplayMode(DWORD width, DWORD height, DWORD bpp);

	static inline void onGameStageChange() { DDrawWrapper->onStageChange(); }
};

}