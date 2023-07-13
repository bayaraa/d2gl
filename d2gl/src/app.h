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

#include "graphic/context.h"
#include "types.h"

// #define _DEBUG 1
// #define _HDTEXT 1

namespace d2gl {

// clang-format off
struct D2GLApp {
#ifdef _DEBUG
	const bool debug = true;
#else
	const bool debug = false;
#endif
	bool log = false;
	bool video_test = false;
	bool ready = false;

	std::string menu_title = "D2GL";
	std::string version_str = "D2GL v1.2.1 by Bayaraa.";
	std::string ini_file = "d2gl.ini";
	std::string mpq_file = "d2gl.mpq";
	std::string log_file = "d2gl.log";

	Api api = Api::Glide;
	std::unique_ptr<Context> context;
	std::string gl_version = "";
	bool vsync = true;
	uint32_t frame_latency = 1;

	GLCaps gl_caps;
	uint8_t gl_ver_major = 4;
	uint8_t gl_ver_minor = 6;
	bool use_compute_shader = false;

	HMODULE hmodule = 0;
	WNDPROC wndproc = 0;
	HWND hwnd = 0;
	HDC hdc = 0;
	std::string dlls_early = "";
	std::string dlls_late = "";

	struct Window {
		DWORD style = 0;
		glm::ivec2 position = { 0, 0 };
		glm::uvec2 size = { 800, 600 };
		glm::uvec2 size_save = { 800, 600 };
		bool fullscreen = false;
		bool centered = true;
		bool dark_mode = true;
		bool resized = false;
	} window;

	struct {
		glm::ivec2 offset = { 0, 0 };
		glm::uvec2 size = { 800, 600 };
		glm::vec2 scale = { 1.0f, 1.0f };
		glm::bvec2 stretched = { false, false };
	} viewport;

	struct {
		uint32_t bpp = 8;
		glm::uvec2 size = { 640, 480 };
		glm::uvec2 custom_size = { 0, 0 };
		GameScreen screen = GameScreen::Movie;
		DrawStage draw_stage = DrawStage::World;
		glm::uvec2 tex_size = { 0, 0 };
		glm::vec2 tex_scale = { 1.0f, 1.0f };
	} game;

	struct {
		bool locked = false;
		glm::vec2 scale = { 1.0f, 1.0f };
		glm::vec2 unscale = { 1.0f, 1.0f };
		bool unlock = false;
	} cursor;

	Select<glm::uvec2> resolutions = {};
	glm::ivec4 desktop_resolution = { 0, 0, 0, 0 };

	struct ForegroundFPS {
		bool active = false;
		Range<int> range = { 60, 25, 240 };
	} foreground_fps;

	struct BackgroundFPS {
		bool active = true;
		Range<int> range = { 25, 25, 60 };
	} background_fps;

	Select<int> shader = {};
	Select<int> lut = {};
	bool fxaa = false;

	struct {
		bool active = false;
		Range<float> strength = { 1.16f, 0.1f, 3.0f };
		Range<float> clamp = { 0.15f, 0.0f, 1.0f };
		Range<float> radius = { 0.4f, 0.0f, 6.0f };
	} sharpen;

	struct {
		bool active = false;
		Range<float> exposure = { 1.2f, 1.0f, 2.0f };
		Range<float> gamma = { 1.0f, 0.5f, 1.2f };
	} bloom;

	bool hd_cursor = false;
	bool hd_text = false;
	bool motion_prediction = false;
	bool skip_intro = false;
	bool no_pickup = false;
	bool show_fps = false;

	struct {
		bool active = false;
		bool centered = false;
	} hd_orbs;

	struct {
		bool active = false;
		bool available = true;
		bool text_over = true;
		Range<int> width = { 200, 200, 300 };
		Range<int> height = { 140, 100, 200 };
	} mini_map;

#ifdef _DEBUG
	int var[10] = { 0 };
#endif
#ifdef _HDTEXT
	struct {
		Select<int> fonts = {};
		Range<float> size = { 10.0f, 5.0f, 40.0f };
		Range<float> weight = { 1.0f, 0.5f, 1.5f };
		Range<float> letter_spacing = { 0.0f, -1.0f, 1.0f };
		Range<float> line_height = { 1.0f, 0.0f, 3.0f };
		Range<float> shadow_intensity = { 0.35f, 0.0f, 1.0f };
		Range<float> offset_x = { 0.0f, -10.0f, 10.0f };
		Range<float> offset_y = { 0.0f, -10.0f, 10.0f };
		bool show_sample = false;
	} hdt;
#endif
};
// clang-format on

extern D2GLApp App;

void dllAttach(HMODULE hmodule);
void dllDetach();

constexpr inline bool ISGLIDE3X()
{
#ifdef GLIDE3X_EXPORTS
	return true;
#else
	return false;
#endif
}

constexpr inline float FLOATVAL(float glide3x, float ddraw)
{
#ifdef GLIDE3X_EXPORTS
	return glide3x;
#else
	return ddraw;
#endif
}

}