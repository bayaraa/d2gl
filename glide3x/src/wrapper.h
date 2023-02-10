#pragma once

#include "texture_manager.h"

#define __MSC__
#include <glide3/glide.h>

namespace d2gl {

#define GLIDE_MAX_NUM_TMU 2
#define GLIDE_TEX_MEMORY 16 * 1024 * 1024

#define GL_TEXTURE_SLOT_GAME 1
#define GL_TEXTURE_SLOT_MAP 2
#define GL_TEXTURE_SLOT_UPSCALE 3
#define GL_TEXTURE_SLOT_POSTFX1 4
#define GL_TEXTURE_SLOT_POSTFX2 5
#define GL_TEXTURE_SLOT_LUT 6
#define GL_TEXTURE_SLOT_PREFX 7

extern const ShaderSource g_shader_game;
extern const ShaderSource g_shader_prefx;

class Wrapper {
	Context* ctx;
	bool m_swapped = true;

	std::map<uint32_t, std::pair<uint32_t, BlendType>> m_blend_types;

	std::unique_ptr<Texture> m_movie_texture;
	std::unique_ptr<Pipeline> m_movie_pipeline;
	GrLfbInfo_t m_movie_buffer = { 0 };

	std::unique_ptr<UniformBuffer> m_game_color_ubo;
	std::unique_ptr<TextureManager> m_game_texture;
	std::unique_ptr<FrameBuffer> m_game_framebuffer;
	std::unique_ptr<Pipeline> m_game_pipeline;
	uint32_t m_current_blend_index = 0;
	bool m_blend_locked = false;

	std::unique_ptr<Texture> m_lut_texture;
	std::unique_ptr<Texture> m_prefx_texture;
	std::unique_ptr<Pipeline> m_prefx_pipeline;

	std::unique_ptr<UniformBuffer> m_upscale_ubo;
	std::unique_ptr<Texture> m_upscale_texture;
	std::unique_ptr<Pipeline> m_upscale_pipeline;
	int m_current_shader = -1;

	glm::vec3 m_sharpen_data;

	std::unique_ptr<UniformBuffer> m_postfx_ubo;
	std::unique_ptr<Texture> m_postfx_texture;
	std::unique_ptr<FrameBuffer> m_postfx_framebuffer;
	std::unique_ptr<Pipeline> m_postfx_pipeline;

public:
	Wrapper();
	~Wrapper();
	void onResize();
	void onShaderChange(bool texture = false);
	void onStageChange();

	void grBufferClear();
	void grBufferSwap();
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

	static uint32_t getTexSize(GrTexInfo* info, uint32_t& width, uint32_t& height);
	static inline void onGameStageChange();
};

extern std::unique_ptr<Wrapper> GlideWrapper;

}