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

#include "pch.h"
#include "wrapper.h"
#include "d2/common.h"
#include "ddraw/surface.h"
#include "modules/hd_cursor.h"
#include "modules/hd_text.h"
#include "win32.h"

namespace d2gl {

std::unique_ptr<Wrapper> DDrawWrapper;

const char* g_shader_game = {
#include "shaders/game.glsl.h"
};

Wrapper::Wrapper()
	: ctx(App.context.get())
{
	UniformBufferCreateInfo ubo_ci;
	ubo_ci.variables = { { "palette", 256 * sizeof(glm::vec4) } };
	m_game_palette_ubo = Context::createUniformBuffer(ubo_ci);

	PipelineCreateInfo game_pipeline_ci;
	game_pipeline_ci.shader = g_shader_game;
	game_pipeline_ci.bindings = {
		{ BindingType::UniformBuffer, "ubo_Colors", m_game_palette_ubo->getBinding() },
		{ BindingType::Texture, "u_Texture", TEXTURE_SLOT_DEFAULT },
	};
	m_game_pipeline = Context::createPipeline(game_pipeline_ci);
	m_game_pipeline->setUniformMat4f("u_MVP", glm::ortho(-1.0f, 1.0f, 1.0f, -1.0f));

	ctx->onInitialize();
}

void Wrapper::onResize()
{
	static glm::uvec2 game_size = { 0, 0 };
	bool game_resized = game_size != App.game.size;
	game_size = App.game.size;

	if (game_resized) {
		TextureCreateInfo texture_ci;
		texture_ci.size = game_size;
		texture_ci.format = App.game.bpp == 8 ? GL_RED : GL_BGRA;
		texture_ci.min_filter = GL_LINEAR;
		texture_ci.mag_filter = GL_LINEAR;
		m_ddraw_texture = Context::createTexture(texture_ci);
	}

	ctx->onResize(game_resized);
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
			modules::HDText::Instance().drawFpsCounter();
			break;
		case DrawStage::Cursor:
			ctx->appendDelayedObjects();
			modules::HDText::Instance().drawEntryText();
			modules::HDCursor::Instance().draw();
			break;
	}
}

void Wrapper::onBufferClear()
{
	if (!m_swapped)
		return;
	m_swapped = false;

	if (App.window.resized)
		onResize();

	ctx->beginFrame();

	App.game.draw_stage = DrawStage::World;
	onStageChange();
}

void Wrapper::onBufferSwap(bool flip)
{
	if ((!flip && App.game.screen == GameScreen::InGame) || (flip && App.game.screen != GameScreen::InGame))
		return;

	if (m_swapped)
		return;
	m_swapped = true;

	m_ddraw_texture->fill(DDrawSurface->getData(), App.game.size.x, App.game.size.y);

	if (App.game.screen != GameScreen::Movie) {
		ctx->bindPipeline(m_game_pipeline);
		ctx->pushQuad();
	}

	ctx->presentFrame();
}

HRESULT Wrapper::setCooperativeLevel(HWND hwnd, DWORD flags)
{
	if (App.video_test || App.hwnd)
		return DD_OK;

	win32::setWindow(hwnd);
	win32::setWindowRect();

	App.context = std::make_unique<Context>();
	DDrawWrapper = std::make_unique<Wrapper>();

	App.game.onStageChange = (onStageChange_t)Wrapper::onGameStageChange;
	App.ready = true;

	helpers::loadDlls(App.dlls_late, true);

	return DD_OK;
}

HRESULT Wrapper::setDisplayMode(DWORD width, DWORD height, DWORD bpp)
{
	static glm::uvec2 old_size = { 0, 0 };

	App.game.screen = (GameScreen)(*d2::is_in_game);
	if (App.game.screen == GameScreen::InGame)
		App.game.screen = GameScreen::Loading;

	if (bpp != 8)
		App.game.screen = GameScreen::Movie;

	App.game.bpp = bpp;
	App.game.size = { width, height };
	trace_log("Game requested screen size: %d x %d", App.game.size.x, App.game.size.y);

	if (!App.video_test && App.hwnd && old_size != App.game.size) {
		win32::setWindowMetrics();
		win32::windowResize();
		DDrawWrapper->onResize();

		old_size = App.game.size;
	}

	return DD_OK;
}

}