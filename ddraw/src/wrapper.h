#pragma once

#include <ddraw.h>

namespace d2gl {

extern const char* g_shader_game;

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

	void updatePalette(const glm::vec4* data);

	static HRESULT setCooperativeLevel(HWND hwnd, DWORD flags);
	static HRESULT setDisplayMode(DWORD width, DWORD height, DWORD bpp);

	static inline void onGameStageChange();
};

extern std::unique_ptr<Wrapper> DDrawWrapper;

}