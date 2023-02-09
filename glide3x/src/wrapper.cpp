#include "pch.h"
#include "wrapper.h"
#include "d2/common.h"
#include "helpers.h"
#include "modules/motion_prediction.h"
#include "option/menu.h"
#include "win32.h"

namespace d2gl {

std::unique_ptr<Wrapper> GlideWrapper;

const ShaderSource g_shader_game = {
#include "shaders/game.glsl.h"
};

const ShaderSource g_shader_prefx = {
#include "shaders/prefx.glsl.h"
};

Wrapper::Wrapper()
	: ctx(App.context.get())
{
	g_glide_texture.memory = new uint8_t[GLIDE_TEX_MEMORY * GLIDE_MAX_NUM_TMU];

	m_blend_types = {
		{ 0, { 0, BlendType::One_Zero } },
		{ 2, { 1, BlendType::Zero_SColor } },
		{ 4, { 2, BlendType::One_One } },
		{ 5, { 3, BlendType::SAlpha_OneMinusSAlpha } },
	};

	TextureCreateInfo movie_texture_ci;
	movie_texture_ci.size = { 640, 480 };
	movie_texture_ci.min_filter = GL_LINEAR;
	movie_texture_ci.mag_filter = GL_LINEAR;
	movie_texture_ci.format = GL_BGRA;
	m_movie_texture = Context::createTexture(movie_texture_ci);

	PipelineCreateInfo movie_pipeline_ci;
	movie_pipeline_ci.shader = (ShaderSource*)&g_shader_movie;
	movie_pipeline_ci.bindings = { { BindingType::Texture, "u_Texture", m_movie_texture->getSlot() } };
	m_movie_pipeline = Context::createPipeline(movie_pipeline_ci);

	UniformBufferCreateInfo ubo_ci;
	ubo_ci.variables = { { "palette", 256 * sizeof(glm::vec4) }, { "gamma", 256 * sizeof(glm::vec4) } };
	m_game_color_ubo = Context::createUniformBuffer(ubo_ci);

	SubTextureCounts sub_texture_counts = { { 256, 256 }, { 128, 128 }, { 64, 64 }, { 32, 32 }, { 16, 5 }, { 8, 1 } };
	m_game_texture = std::make_unique<TextureManager>(sub_texture_counts);

	PipelineCreateInfo game_pipeline_ci;
	game_pipeline_ci.shader = (ShaderSource*)&g_shader_game;
	game_pipeline_ci.bindings = {
		{ BindingType::UniformBuffer, "ubo_Colors", m_game_color_ubo->getBinding() },
		{ BindingType::Texture, "u_Texture", m_game_texture->getTexture()->getSlot() },
	};
	game_pipeline_ci.attachment_blends.clear();
	for (auto& blend : m_blend_types)
		game_pipeline_ci.attachment_blends.push_back({ blend.second.second, BlendType::SAlpha_OneMinusSAlpha });
	m_game_pipeline = Context::createPipeline(game_pipeline_ci);

	TextureCreateInfo texture_create_info;
	texture_create_info.size = { 1024, 32 };
	texture_create_info.layer_count = 14;
	texture_create_info.slot = GL_TEXTURE_SLOT_LUT;
	m_lut_texture = Context::createTexture(texture_create_info);

	auto image_data = helpers::loadImage("assets\\textures\\lut.png", false);
	m_lut_texture->fillImage(image_data, 1, 14);
	helpers::clearImage(image_data);

	PipelineCreateInfo prefx_pipeline_ci;
	prefx_pipeline_ci.shader = (ShaderSource*)&g_shader_prefx;
	prefx_pipeline_ci.bindings = {
		{ BindingType::Texture, "u_Texture", GL_TEXTURE_SLOT_GAME },
		{ BindingType::Texture, "u_LUTTexture", m_lut_texture->getSlot() },
	};
	m_prefx_pipeline = Context::createPipeline(prefx_pipeline_ci);
	m_prefx_pipeline->setUniformMat4f("u_MVP", glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f));

	UniformBufferCreateInfo upscale_ubo_ci;
	upscale_ubo_ci.variables = { { "out_size", sizeof(glm::vec2) }, { "tex_size", sizeof(glm::vec2) }, { "rel_size", sizeof(glm::vec2) } };
	m_upscale_ubo = Context::createUniformBuffer(upscale_ubo_ci);

	UniformBufferCreateInfo postfx_ubo_ci;
	postfx_ubo_ci.variables = { { "sharpen", sizeof(glm::vec4) }, { "rel_size", sizeof(glm::vec2) } };
	m_postfx_ubo = Context::createUniformBuffer(postfx_ubo_ci);

	m_sharpen_data = { App.sharpen.strength.value, App.sharpen.clamp.value, App.sharpen.radius.value };
	m_postfx_ubo->updateDataVec4f("sharpen", glm::vec4(m_sharpen_data, 1.0f));

	PipelineCreateInfo postfx_pipeline_ci;
	postfx_pipeline_ci.shader = (ShaderSource*)&g_shader_postfx;
	postfx_pipeline_ci.bindings = {
		{ BindingType::UniformBuffer, "ubo_Metrics", m_postfx_ubo->getBinding() },
		{ BindingType::Texture, "u_Texture", GL_TEXTURE_SLOT_POSTFX1 },
	};
	m_postfx_pipeline = Context::createPipeline(postfx_pipeline_ci);
	m_postfx_pipeline->setUniformMat4f("u_MVP", glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f));
}

Wrapper::~Wrapper()
{
	delete[] g_glide_texture.memory;
}

void Wrapper::onResize()
{
	static glm::uvec2 game_size = { 0, 0 };
	static glm::uvec2 window_size = { 0, 0 };

	bool game_resized = game_size != App.game.size;
	game_size = App.game.size;

	bool window_resized = (App.window.resized || window_size != App.window.size);
	window_size = App.window.size;
	App.window.resized = false;

	if (game_resized) {
		m_game_pipeline->setUniformMat4f("u_MVP", glm::ortho(0.0f, (float)game_size.x, (float)game_size.y, 0.0f));
		m_upscale_ubo->updateDataVec2f("tex_size", { (float)App.game.tex_size.x, (float)App.game.tex_size.y });
		m_upscale_ubo->updateDataVec2f("rel_size", { 1.0f / App.game.tex_size.x, 1.0f / App.game.tex_size.y });

		FrameBufferCreateInfo game_frambuffer_ci;
		game_frambuffer_ci.size = game_size;
		game_frambuffer_ci.attachments = { { GL_TEXTURE_SLOT_GAME }, { GL_TEXTURE_SLOT_MAP, { 0.0f, 0.0f, 0.0f, 0.0f } } };
		m_game_framebuffer = Context::createFrameBuffer(game_frambuffer_ci);
	}

	if (window_resized) {
		ctx->toggleVsync();

		glm::vec2 scale = { (float)window_size.x / 640, (float)window_size.y / 360 };
		float offset_x = ((scale.x > scale.y) ? scale.x / scale.y : 1.0f) * 1.00f;
		float offset_y = ((scale.x < scale.y) ? scale.y / scale.x : 1.0f) * 0.75f;
		m_movie_pipeline->setUniformMat4f("u_MVP", glm::ortho(-1.0f * offset_x, 1.0f * offset_x, 1.0f * offset_y, -1.0f * offset_y));
	}

	FrameBufferCreateInfo frambuffer_ci;
	frambuffer_ci.size = App.viewport.size;
	frambuffer_ci.attachments = { { GL_TEXTURE_SLOT_POSTFX1, {}, GL_LINEAR, GL_LINEAR } };
	m_postfx_framebuffer = Context::createFrameBuffer(frambuffer_ci);

	onShaderChange(game_resized);
	m_upscale_ubo->updateDataVec2f("out_size", { (float)App.game.tex_size.x * App.viewport.scale.x, (float)App.game.tex_size.y * App.viewport.scale.y });
	m_postfx_ubo->updateDataVec2f("rel_size", { 1.0f / App.viewport.size.x, 1.0f / App.viewport.size.y });
}

void Wrapper::onShaderChange(bool texture)
{
	if (texture || (m_current_shader != App.shader.selected && (m_current_shader == 0 || App.shader.selected == 0))) {
		TextureCreateInfo texture_ci;
		texture_ci.size = App.game.tex_size;
		texture_ci.slot = GL_TEXTURE_SLOT_UPSCALE;
		if (App.shader.selected == 0) {
			texture_ci.min_filter = GL_LINEAR;
			texture_ci.mag_filter = GL_LINEAR;
		}
		m_upscale_texture = Context::createTexture(texture_ci);
	}

	if (m_current_shader != App.shader.selected) {
		PipelineCreateInfo pipeline_ci;
		pipeline_ci.shader = (ShaderSource*)&g_shader_upscale[App.shader.selected].second;
		pipeline_ci.bindings = {
			{ BindingType::UniformBuffer, "ubo_Metrics", m_upscale_ubo->getBinding() },
			{ BindingType::Texture, "u_Texture", GL_TEXTURE_SLOT_UPSCALE },
		};
		m_upscale_pipeline = Context::createPipeline(pipeline_ci);
	}

	m_upscale_pipeline->setUniformMat4f("u_MVP", glm::ortho(-App.game.tex_scale.x, App.game.tex_scale.x, -App.game.tex_scale.y, App.game.tex_scale.y));
	m_current_shader = App.shader.selected;
}

void Wrapper::onStageChange()
{
	if (App.game.screen == GameScreen::Movie)
		return;

	switch (App.game.draw_stage) {
		case DrawStage::World:
			break;
		case DrawStage::UI:
			if (App.lut.selected) {
				ctx->bindPipeline(m_prefx_pipeline);
				ctx->pushQuad(App.lut.selected - 1);

				ctx->bindPipeline(m_game_pipeline, m_current_blend_index);
			}
			break;
		case DrawStage::Map:
			// m_blend_locked = true;
			// ctx->bindPipeline(m_game_pipeline, 3);
			// ctx->setVertexFlagW(2);
			break;
		case DrawStage::HUD:
			// m_blend_locked = false;
			// ctx->bindPipeline(m_game_pipeline, m_current_blend_index);
			// ctx->setVertexFlagW(0);
			/*if (App.feature.mini_map.active) {
			Renderer::Instance().Flush();
			modules::Minimap::Instance().FillTexture(buffer_game_);
			modules::Minimap::Instance().Draw();
			}*/
			break;
		case DrawStage::Cursor:
			// modules::HDText::Instance().DrawTest();
			// Renderer::Instance().AppendDelayedObjects();
			// modules::HDCursor::Instance().Draw();
			break;
	}
}

void Wrapper::grBufferClear()
{
	if (!m_swapped)
		return;
	m_swapped = false;

	if (App.window.resized)
		onResize();

	if (App.game.screen == GameScreen::Movie) {
		ctx->beginFrame();
		ctx->bindDefaultFrameBuffer();
		ctx->setViewport(App.window.size);
		ctx->bindPipeline(m_movie_pipeline);
		ctx->pushQuad();

		grBufferSwap();
	} else {
		if (m_current_shader != App.shader.selected)
			onShaderChange();

		if (App.sharpen.active) {
			const auto sharpen_data = glm::vec3(App.sharpen.strength.value, App.sharpen.clamp.value, App.sharpen.radius.value);
			if (m_sharpen_data != sharpen_data) {
				m_postfx_ubo->updateDataVec4f("sharpen", glm::vec4(sharpen_data, 1.0f));
				m_sharpen_data = sharpen_data;
			}
		}

		modules::MotionPrediction::Instance().update();

		ctx->beginFrame();
		ctx->bindFrameBuffer(m_game_framebuffer);
		ctx->setViewport(App.game.size);

		App.game.draw_stage = DrawStage::World;
		onStageChange();
	}
}

void Wrapper::grBufferSwap()
{
	if (m_swapped)
		return;
	m_swapped = true;

	if (App.game.screen != GameScreen::Movie) {
		m_upscale_texture->fillFromBuffer(m_game_framebuffer);

		if (App.sharpen.active || App.fxaa) {
			ctx->bindFrameBuffer(m_postfx_framebuffer, false);
			ctx->setViewport(App.viewport.size);
		} else {
			ctx->bindDefaultFrameBuffer();
			ctx->setViewport(App.viewport.size, App.viewport.offset);
		}
		ctx->bindPipeline(m_upscale_pipeline);
		ctx->pushQuad();

		if (App.sharpen.active) {
			if (!App.fxaa) {
				ctx->bindDefaultFrameBuffer();
				ctx->setViewport(App.viewport.size, App.viewport.offset);
			}
			ctx->bindPipeline(m_postfx_pipeline);
			ctx->pushQuad(0);
		}

		if (App.fxaa) {
			ctx->bindDefaultFrameBuffer();
			ctx->setViewport(App.viewport.size, App.viewport.offset);
			ctx->bindPipeline(m_postfx_pipeline);
			ctx->pushQuad(1);
		}
	}

	App.var1 = m_game_texture->getUsage(256);
	App.var2 = m_game_texture->getUsage(128);
	App.var3 = m_game_texture->getUsage(64);
	App.var4 = m_game_texture->getUsage(32);
	App.var5 = m_game_texture->getUsage(16);
	App.var6 = m_game_texture->getUsage(8);
	option::Menu::instance().draw();

	ctx->presentFrame();

	// trace("==== SWAP ====");
}

void Wrapper::grDrawPoint(const void* pt)
{
	GlideVertex* vertex = (GlideVertex*)pt;

	ctx->pushVertex(vertex);
	vertex->x += 1.0f;
	ctx->pushVertex(vertex);
	vertex->y += 1.0f;
	ctx->pushVertex(vertex);
	vertex->x -= 1.0f;
	ctx->pushVertex(vertex);
}

void Wrapper::grDrawLine(const void* v1, const void* v2)
{
	GlideVertex* vertex1 = (GlideVertex*)v1;
	GlideVertex* vertex2 = (GlideVertex*)v2;

	glm::vec2 widening_vec = { vertex1->y - vertex2->y, vertex2->x - vertex1->x };
	const float len = (2.0f * glm::length(widening_vec));
	const float half_inv_len = 1.0f / len;
	widening_vec *= half_inv_len;

	vertex1->x -= widening_vec.x;
	vertex1->y -= widening_vec.y;
	ctx->pushVertex(vertex1);
	vertex1->x += widening_vec.x * 2.0f;
	vertex1->y += widening_vec.y * 2.0f;
	ctx->pushVertex(vertex1);

	vertex2->x += widening_vec.x;
	vertex2->y += widening_vec.y;
	ctx->pushVertex(vertex2);
	vertex2->x -= widening_vec.x * 2.0f;
	vertex2->y -= widening_vec.y * 2.0f;
	ctx->pushVertex(vertex2);
}

void Wrapper::grDrawVertexArray(FxU32 mode, FxU32 count, void** pointers)
{
	if (mode == GR_TRIANGLE_STRIP) {
		const auto offset = modules::MotionPrediction::Instance().getGlobalOffset();
		for (FxU32 i = 0; i < count - 2; i += 2) {
			ctx->pushVertex((const GlideVertex*)pointers[i + 0], { -0.0001f, -0.0001f }, offset);
			ctx->pushVertex((const GlideVertex*)pointers[i + 1], { +0.0001f, -0.0001f }, offset);
			ctx->pushVertex((const GlideVertex*)pointers[i + 3], { +0.0001f, +0.0001f }, offset);
			ctx->pushVertex((const GlideVertex*)pointers[i + 2], { -0.0001f, +0.0001f }, offset);
		}
	} else {
		for (FxU32 i = 0; i < count; i++)
			ctx->pushVertex((const GlideVertex*)pointers[i]);
	}
}

void Wrapper::grDrawVertexArrayContiguous(FxU32 mode, FxU32 count, void* pointers)
{
	for (FxU32 i = 0; i < count; i++)
		ctx->pushVertex(&((const GlideVertex*)pointers)[i]);
}

void Wrapper::grAlphaBlendFunction(GrAlphaBlendFnc_t rgb_df)
{
	m_current_blend_index = m_blend_types.at(static_cast<uint32_t>(rgb_df)).first;
	if (!m_blend_locked)
		ctx->bindPipeline(m_game_pipeline, m_current_blend_index);
}

void Wrapper::grAlphaCombine(GrCombineFunction_t function)
{
	ctx->setVertexFlagZ(function == GR_COMBINE_FUNCTION_LOCAL ? 1 : 0);
}

void Wrapper::grChromakeyMode(GrChromakeyMode_t mode)
{
	ctx->setVertexFlagX(mode == GR_CHROMAKEY_ENABLE ? 1 : 0);
}

void Wrapper::grColorCombine(GrCombineFunction_t function)
{
	ctx->setVertexFlagY(function == GR_COMBINE_FUNCTION_LOCAL ? 1 : 0);
}

void Wrapper::grConstantColorValue(GrColor_t value)
{
	ctx->setVertexColor(value);
}

void Wrapper::grLoadGammaTable(FxU32 nentries, FxU32* red, FxU32* green, FxU32* blue)
{
	glm::vec4 gamma[256];
	for (int32_t i = 0; i < glm::min((int)nentries, 256); i++) {
		gamma[i].r = (float)red[i] / 255;
		gamma[i].g = (float)green[i] / 255;
		gamma[i].b = (float)blue[i] / 255;
	}
	m_game_color_ubo->updateData("gamma", &gamma[0]);
}

void Wrapper::guGammaCorrectionRGB(FxFloat red, FxFloat green, FxFloat blue)
{
	glm::vec4 gamma[256];
	for (int32_t i = 0; i < 256; i++) {
		float v = i / 255.0f;
		gamma[i].r = powf(v, 1.0f / red);
		gamma[i].g = powf(v, 1.0f / green);
		gamma[i].b = powf(v, 1.0f / blue);
	}
	m_game_color_ubo->updateData("gamma", &gamma[0]);
}

void Wrapper::grTexSource(GrChipID_t tmu, FxU32 start_address, GrTexInfo* info)
{
	uint32_t width, height;
	uint32_t size = Wrapper::getTexSize(info, width, height);
	start_address += GLIDE_TEX_MEMORY * tmu;

	const auto frame_count = ctx->getFrameCount();
	const auto sub_tex_info = m_game_texture->getSubTextureInfo(start_address, size, width, height, frame_count);
	if (sub_tex_info) {
		ctx->setVertexTexIds({ 0, sub_tex_info->tex_num });
		ctx->setVertexOffset(sub_tex_info->offset);
		ctx->setVertexTexShift(sub_tex_info->shift);
	}
}

void Wrapper::grTexDownloadMipMap(GrChipID_t tmu, FxU32 start_address, GrTexInfo* info)
{
	uint32_t width, height;
	uint32_t size = Wrapper::getTexSize(info, width, height);
	start_address += GLIDE_TEX_MEMORY * tmu;

	memcpy(g_glide_texture.memory + start_address, info->data, width * height);
	g_glide_texture.hash[start_address] = helpers::hash(info->data, width * height);
}

void Wrapper::grTexDownloadTable(void* data)
{
	static uint32_t old_hash = 0;
	const uint32_t hash = helpers::hash(data, sizeof(uint32_t) * 256);
	if (old_hash == hash)
		return;

	glm::vec4 palette[256];
	uint32_t* pal = (uint32_t*)data;

	for (int32_t i = 0; i < 256; i++) {
		palette[i].a = (float)((pal[i] >> 24) & 0xFF) / 255;
		palette[i].r = (float)((pal[i] >> 16) & 0xFF) / 255;
		palette[i].g = (float)((pal[i] >> 8) & 0xFF) / 255;
		palette[i].b = (float)((pal[i]) & 0xFF) / 255;
	}
	m_game_color_ubo->updateData("palette", &palette[0]);
	old_hash = hash;
}

FxBool Wrapper::grLfbLock(GrLfbWriteMode_t write_mode, GrOriginLocation_t origin, GrLfbInfo_t* info)
{
	if (write_mode == GR_LFBWRITEMODE_8888 && origin == GR_ORIGIN_UPPER_LEFT && info) {
		if (!m_movie_buffer.lfbPtr) {
			m_movie_buffer.lfbPtr = malloc(640 * 480 * 4);
			m_movie_buffer.strideInBytes = 640 * 4;
			m_movie_buffer.writeMode = write_mode;
			m_movie_buffer.origin = origin;
			if (m_movie_buffer.lfbPtr)
				memset(m_movie_buffer.lfbPtr, 0, 640 * 480 * 4);
		}
		*info = m_movie_buffer;

		return FXTRUE;
	}

	return FXFALSE;
}

FxBool Wrapper::grLfbUnlock()
{
	App.game.screen = GameScreen::Movie;
	m_movie_texture->fill((uint8_t*)m_movie_buffer.lfbPtr, 640, 480);
	grBufferClear();

	return FXTRUE;
}

GrContext_t Wrapper::grSstWinOpen(FxU32 hwnd, GrScreenResolution_t screen_resolution)
{
	App.game.screen = (GameScreen)(*d2::is_in_game);
	if (App.game.screen == GameScreen::InGame)
		App.game.screen = GameScreen::Loading;

	glm::uvec2 old_size = App.game.size;
	if (screen_resolution == GR_RESOLUTION_640x480)
		App.game.size = { 640, 480 };
	else if (screen_resolution == GR_RESOLUTION_800x600)
		App.game.size = { 800, 600 };
	else {
		if (App.game.custom_size.x != 0 && App.game.custom_size.y != 0) {
			App.game.size = App.game.custom_size;
			App.game.custom_size = { 0, 0 };
		} else
			App.game.size = { *d2::screen_width, *d2::screen_height };
	}
	trace("Game requested screen size: %d x %d\n", App.game.size.x, App.game.size.y);

	if (App.hwnd) {
		if (old_size != App.game.size) {
			win32::setWindowMetricts();
			win32::windowResize();
			GlideWrapper->onResize();
		}
		return 1;
	}

	win32::setWindow((HWND)hwnd);
	win32::setWindowRect();
	win32::setWindowMetricts();

	App.context = std::make_unique<Context>();
	GlideWrapper = std::make_unique<Wrapper>();
	GlideWrapper->onResize();

	App.game.onStageChange = (onStageChange_t)Wrapper::onGameStageChange;
	App.ready = true;

	LoadLibraryA("SGD2FreeRes.dll");

	return 1;
}

FxU32 Wrapper::grGet(FxU32 pname, FxI32& params)
{
	switch (pname) {
		case GR_MAX_TEXTURE_SIZE: params = 256; break;
		case GR_MAX_TEXTURE_ASPECT_RATIO: params = 3; break;
		case GR_TEXTURE_ALIGN: params = 256; break;
		case GR_NUM_BOARDS: params = 1; break;
		case GR_NUM_FB: params = 1; break;
		case GR_NUM_TMU: params = GLIDE_MAX_NUM_TMU; break;
		case GR_MEMORY_UMA: params = 0; break;
		case GR_GAMMA_TABLE_ENTRIES: params = 256; break;
		case GR_BITS_GAMMA: params = 8; break;
	}
	return 4;
}

uint32_t Wrapper::getTexSize(GrTexInfo* info, uint32_t& width, uint32_t& height)
{
	if (info->aspectRatioLog2 < 0) {
		height = 1 << info->largeLodLog2;
		width = height >> -info->aspectRatioLog2;
		return height;
	} else {
		width = 1 << info->largeLodLog2;
		height = width >> info->aspectRatioLog2;
		return width;
	}
}

inline void Wrapper::onGameStageChange()
{
	GlideWrapper->onStageChange();
}
}