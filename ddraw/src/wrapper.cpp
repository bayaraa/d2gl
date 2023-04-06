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
#include "helpers.h"
#include "win32.h"

namespace d2gl {

std::unique_ptr<Wrapper> DDrawWrapper;

Wrapper::Wrapper()
	: ctx(App.context.get())
{}

void Wrapper::onBufferClear()
{
	if (!m_swapped)
		return;
	m_swapped = false;

	ctx->beginFrame();
}

void Wrapper::onBufferSwap(bool flip)
{
	if ((!flip && App.game.screen == GameScreen::InGame) || (flip && App.game.screen != GameScreen::InGame))
		return;

	if (m_swapped)
		return;
	m_swapped = true;

	ctx->getCommandBuffer()->gameTextureUpdate((uint8_t*)DDrawSurface->getData(), { App.game.size.x, App.game.size.y }, App.game.bpp == 8 ? 1 : 4);
	ctx->presentFrame();
}

void Wrapper::updatePalette(const glm::vec4* data)
{
	static uint32_t old_hash = 0;
	const uint32_t hash = helpers::hash(data, sizeof(glm::vec4) * 256);
	if (old_hash == hash)
		return;

	ctx->getCommandBuffer()->colorUpdate(UBOType::Palette, data);
	old_hash = hash;
}

HRESULT Wrapper::setCooperativeLevel(HWND hwnd, DWORD flags)
{
	if (App.video_test || App.hwnd)
		return DD_OK;

	win32::setWindow(hwnd);
	win32::setWindowRect();

	App.context = std::make_unique<Context>();
	DDrawWrapper = std::make_unique<Wrapper>();
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

		old_size = App.game.size;
	}

	return DD_OK;
}

}