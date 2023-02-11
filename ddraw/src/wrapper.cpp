#include "pch.h"
#include "wrapper.h"
#include "d2/common.h"
#include "ddraw/surface.h"
#include "modules/motion_prediction.h"
#include "option/menu.h"
#include "win32.h"

namespace d2gl {

std::unique_ptr<Wrapper> DDrawWrapper;

const ShaderSource g_shader_game = {
#include "shaders/game.glsl.h"
};

Wrapper::Wrapper()
	: ctx(App.context.get())
{
	PipelineCreateInfo movie_pipeline_ci;
	movie_pipeline_ci.shader = (ShaderSource*)&g_shader_movie;
	movie_pipeline_ci.bindings = { { BindingType::Texture, "u_Texture", 0 } };
	m_movie_pipeline = Context::createPipeline(movie_pipeline_ci);

	UniformBufferCreateInfo ubo_ci;
	ubo_ci.variables = { { "palette", 256 * sizeof(glm::vec4) } };
	m_game_palette_ubo = Context::createUniformBuffer(ubo_ci);

	PipelineCreateInfo game_pipeline_ci;
	game_pipeline_ci.shader = (ShaderSource*)&g_shader_game;
	game_pipeline_ci.bindings = {
		{ BindingType::UniformBuffer, "ubo_Colors", m_game_palette_ubo->getBinding() },
		{ BindingType::Texture, "u_Texture", 0 },
	};
	m_game_pipeline = Context::createPipeline(game_pipeline_ci);
	m_game_pipeline->setUniformMat4f("u_MVP", glm::ortho(-1.0f, 1.0f, 1.0f, -1.0f));

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
		{ BindingType::Texture, "u_Textures[0]", GL_TEXTURE_SLOT_POSTFX1 },
		{ BindingType::Texture, "u_Textures[1]", GL_TEXTURE_SLOT_POSTFX2 },
	};
	m_postfx_pipeline = Context::createPipeline(postfx_pipeline_ci);
	m_postfx_pipeline->setUniformMat4f("u_MVP", glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f));
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
		m_upscale_ubo->updateDataVec2f("tex_size", { (float)App.game.tex_size.x, (float)App.game.tex_size.y });
		m_upscale_ubo->updateDataVec2f("rel_size", { 1.0f / App.game.tex_size.x, 1.0f / App.game.tex_size.y });

		FrameBufferCreateInfo frambuffer_ci;
		frambuffer_ci.size = game_size;
		frambuffer_ci.attachments = { { GL_TEXTURE_SLOT_GAME } };
		m_game_framebuffer = Context::createFrameBuffer(frambuffer_ci);

		TextureCreateInfo texture_ci;
		texture_ci.size = game_size;
		texture_ci.format = App.game.bpp == 8 ? GL_RED : GL_BGRA;
		texture_ci.min_filter = GL_LINEAR;
		texture_ci.mag_filter = GL_LINEAR;
		m_ddraw_texture = Context::createTexture(texture_ci);
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

	TextureCreateInfo texture_ci;
	texture_ci.size = App.viewport.size;
	texture_ci.slot = GL_TEXTURE_SLOT_POSTFX2;
	texture_ci.min_filter = GL_LINEAR;
	texture_ci.mag_filter = GL_LINEAR;
	m_postfx_texture = Context::createTexture(texture_ci);

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
			break;
		case DrawStage::Map:
			break;
		case DrawStage::HUD:
			break;
		case DrawStage::Cursor:
			// modules::HDText::Instance().DrawTest();
			// Renderer::Instance().AppendDelayedObjects();
			// modules::HDCursor::Instance().Draw();
			break;
	}
}

void Wrapper::onBufferClear()
{
	if (*d2::esc_menu_open)
		App.game.screen = GameScreen::InGame;

	modules::MotionPrediction::Instance().update();

	App.game.draw_stage = DrawStage::World;
	onStageChange();

	if (App.game.screen != GameScreen::InGame)
		onBufferSwap();
}

void Wrapper::onBufferSwap(bool flip)
{
	if (flip && App.game.screen != GameScreen::InGame)
		return;

	if (App.window.resized)
		onResize();

	ctx->beginFrame();

	m_ddraw_texture->fill((uint8_t*)DDrawSurface->m_data, App.game.size.x, App.game.size.y);

	if (App.game.screen == GameScreen::Movie) {
		ctx->bindDefaultFrameBuffer();
		ctx->setViewport(App.window.size);
		ctx->bindPipeline(m_movie_pipeline);
		ctx->pushQuad();
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

		ctx->bindFrameBuffer(m_game_framebuffer);
		ctx->setViewport(App.game.size);
		ctx->bindPipeline(m_game_pipeline);
		ctx->pushQuad();

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
			if (App.fxaa)
				m_postfx_texture->fillFromBuffer(m_postfx_framebuffer);
			else {
				ctx->bindDefaultFrameBuffer();
				ctx->setViewport(App.viewport.size, App.viewport.offset);
			}
			ctx->bindPipeline(m_postfx_pipeline);
			ctx->pushQuad(0, App.fxaa);
		}

		if (App.fxaa) {
			ctx->bindDefaultFrameBuffer();
			ctx->setViewport(App.viewport.size, App.viewport.offset);
			ctx->bindPipeline(m_postfx_pipeline);
			ctx->pushQuad(1);
		}
	}

	option::Menu::instance().draw();

	ctx->presentFrame();

	if (App.game.screen == GameScreen::InGame)
		App.game.screen = GameScreen::Loading;
}

void Wrapper::updatePalette(const glm::vec4* data)
{
	m_game_palette_ubo->updateData("palette", data);
}

HRESULT Wrapper::setCooperativeLevel(HWND hwnd, DWORD flags)
{
	if (App.hwnd)
		return DD_OK;

	win32::setWindow(hwnd);
	win32::setWindowRect();

	App.context = std::make_unique<Context>();
	DDrawWrapper = std::make_unique<Wrapper>();

	App.game.onStageChange = (onStageChange_t)Wrapper::onGameStageChange;
	App.ready = true;

	return DD_OK;
}

HRESULT Wrapper::setDisplayMode(DWORD width, DWORD height, DWORD bpp)
{
	static glm::uvec2 old_size = { 0, 0 };

	App.game.screen = (GameScreen)(*d2::is_in_game);
	if (App.game.screen == GameScreen::InGame)
		App.game.screen = GameScreen::Loading;

	if (bpp == 32)
		App.game.screen = GameScreen::Movie;

	App.game.bpp = bpp;
	App.game.size = { width, height };
	trace("Game requested screen size: %d x %d\n", App.game.size.x, App.game.size.y);

	if (App.hwnd && old_size != App.game.size) {
		win32::setWindowMetricts();
		win32::windowResize();
		DDrawWrapper->onResize();

		old_size = App.game.size;
	}

	return DD_OK;
}

inline void Wrapper::onGameStageChange()
{
	DDrawWrapper->onStageChange();
}

}