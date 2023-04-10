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
#include "ini.h"
#include "helpers.h"

namespace d2gl::option {

#define boolString(a) (a ? "true" : "false")

bool getValue(LPCSTR section, LPCSTR key, LPSTR return_string, DWORD size)
{
	return GetPrivateProfileStringA(section, key, "", return_string, size, App.ini_file.c_str()) > 0;
}

bool getBool(LPCSTR section, LPCSTR key, bool def)
{
	char value[10];
	if (getValue(section, key, value, 10))
		return (std::string(value) == "true" || std::string(value) == "1");

	return def;
}

int getInt(LPCSTR section, LPCSTR key, int def, int min, int max)
{
	char value[10] = { 0 };
	if (getValue(section, key, value, 10)) {
		int ret = def;
		try {
			ret = std::atoi(value);
		} catch (const std::invalid_argument& e) {}

		if (ret < min)
			return min;
		if (ret > max)
			return max;

		return ret;
	}
	return def;
}

float getFloat(LPCSTR section, LPCSTR key, Range<float> val)
{
	char value[10] = { 0 };
	if (getValue(section, key, value, 10)) {
		float ret = val.value;
		try {
			ret = std::stof(value);
		} catch (const std::invalid_argument& e) {}

		if (ret < val.min)
			return val.min;
		if (ret > val.max)
			return val.max;

		return ret;
	}
	return val.value;
}

std::string getString(LPCSTR section, LPCSTR key, const std::string& def)
{
	char value[200] = { 0 };
	if (getValue(section, key, value, 200)) {
		return std::string(value);
	}
	return def;
}

bool saveValue(LPCSTR section, LPCSTR key, LPCSTR value_string)
{
	return WritePrivateProfileStringA(section, key, value_string, App.ini_file.c_str()) > 0;
}

bool saveBool(LPCSTR section, LPCSTR key, bool val)
{
	char value[10] = { 0 };
	sprintf_s(value, "%s", (val ? "true" : "false"));
	return saveValue(section, key, value);
}

bool saveInt(LPCSTR section, LPCSTR key, int val)
{
	char value[10] = { 0 };
	sprintf_s(value, "%d", val);
	return saveValue(section, key, value);
}

bool saveFloat(LPCSTR section, LPCSTR key, float val)
{
	char value[10] = { 0 };
	sprintf_s(value, "%.3f", val);
	return saveValue(section, key, value);
}

bool saveString(LPCSTR section, LPCSTR key, const std::string& val)
{
	return saveValue(section, key, val.c_str());
}

void saveIni()
{
	char buf[3000] = { 0 };
	std::ofstream out_file;
	out_file.open(App.ini_file);

	static const char* screen_setting =
	  "; ==== D2GL Configs ====\n\n"
	  "; Recommended: (ctrl+O) opens in-game option menu.\n"
	  "; All settings except \"Other\" section can be changed there.\n\n\n"
	  "[Screen]\n\n"
	  "; Game will run in fullscreen window.\n"
	  "; Window size (window_width/window_height) will be ignored.\n"
	  "fullscreen=%s\n\n"
	  "; Window size.\n"
	  "window_width=%d\n"
	  "window_height=%d\n\n"
	  "; Always centered window on launch.\n"
	  "; Window position (window_posx/window_posy) will be ignored.\n"
	  "centered_window=%s\n\n"
	  "; Window position.\n"
	  "window_posx=%d\n"
	  "window_posy=%d\n\n"
	  "; Dark style window title bar.\n"
	  "dark_mode=%s\n\n"
	  "; Vertical synchronization.\n"
	  "; Game fps adapt screen refresh rate (might have input lag).\n"
	  "vsync=%s\n\n"
	  "; Max Foreground FPS.\n"
	  "; Limit maximum fps when game window is focused(active) (vsync must be disabled).\n"
	  "foreground_fps=%s\n"
	  "foreground_fps_value=%d\n\n"
	  "; Max Background FPS.\n"
	  "; Limit maximum fps when game window is in background(inactive).\n"
	  "background_fps=%s\n"
	  "background_fps_value=%d\n\n\n";

	sprintf_s(buf, screen_setting,
	  boolString(App.window.fullscreen),
	  App.window.size.x,
	  App.window.size.y,
	  boolString(App.window.centered),
	  App.window.position.x,
	  App.window.position.y,
	  boolString(App.window.dark_mode),
	  boolString(App.vsync),
	  boolString(App.foreground_fps.active),
	  App.foreground_fps.range.value,
	  boolString(App.background_fps.active),
	  App.background_fps.range.value);
	out_file << buf;

	std::string shader_str = "";
	for (size_t i = 0; i < App.shader.items.size(); i++) {
		char label[100] = { 0 };
		sprintf_s(label, "; %2d: %s", i, App.shader.items[i].name.c_str());
		shader_str += std::string(label) + "\n";
	}

	static const char* graphic_setting =
	  "[Graphic]\n\n"
	  "; Upscale shader.\n"
	  "; Set one of following shaders.\n"
	  "%s"
	  "shader=%d\n\n"
	  "; Color grading (LUT) (only available in glide mode).\n"
	  "; Set one of 1-%d predefined luts. 0 = game default.\n"
	  "lut=%d\n\n"
	  "; Luma sharpen.\n"
	  "sharpen=%s\n"
	  "sharpen_strength=%.3f\n"
	  "sharpen_clamp=%.3f\n"
	  "sharpen_radius=%.3f\n\n"
	  "; Fast approximate anti-aliasing.\n"
	  "fxaa=%s\n\n"
	  "; Bloom effect.\n"
	  "bloom=%s\n"
	  "bloom_exposure=%.3f\n"
	  "bloom_gamma=%.3f\n\n\n";

	sprintf_s(buf, graphic_setting,
	  shader_str.c_str(),
	  App.shader.selected,
	  App.lut.items.size() - 1,
	  App.lut.selected,
	  boolString(App.sharpen.active),
	  App.sharpen.strength.value,
	  App.sharpen.clamp.value,
	  App.sharpen.radius.value,
	  boolString(App.fxaa),
	  boolString(App.bloom.active),
	  App.bloom.exposure.value,
	  App.bloom.gamma.value);
	out_file << buf;

	static const char* feature_setting =
	  "[Feature]\n\n"
	  "; HD cursor in game & menu screen.\n"
	  "hd_cursor=%s\n\n"
	  "; HD in game text (HD cursor required).\n"
	  "hd_text=%s\n\n"
	  //"; HD life & mana orbs (HD cursor required).\n"
	  //"hd_orbs=%s\n"
	  //"hd_orbs_centered=%s\n\n"
	  "; Always on Minimap widget (only available in glide mode, HD cursor required).\n"
	  "mini_map=%s\n"
	  "mini_map_text_over=%s\n"
	  "mini_map_width=%d\n"
	  "mini_map_height=%d\n\n"
	  "; D2DX's Motion Prediction.\n"
	  "motion_prediction=%s\n\n"
	  "; Skip the Intro videos.\n"
	  "skip_intro=%s\n\n"
	  "; Auto /nopickup option on launch (exclude 1.09d).\n"
	  "no_pickup=%s\n\n"
	  "; Show FPS Counter (bottom center).\n"
	  "show_fps=%s\n\n"
	  "; No Cursor Lock (cursor will not locked within window).\n"
	  "no_cursor_lock=%s\n\n\n";

	sprintf_s(buf, feature_setting,
	  boolString(App.hd_cursor),
	  boolString(App.hd_text),
	  // boolString(App.hd_orbs.active),
	  // boolString(App.hd_orbs.centered),
	  boolString(App.mini_map.active),
	  boolString(App.mini_map.text_over),
	  App.mini_map.width.value,
	  App.mini_map.height.value,
	  boolString(App.motion_prediction),
	  boolString(App.skip_intro),
	  boolString(App.no_pickup),
	  boolString(App.show_fps),
	  boolString(App.cursor.no_lock));
	out_file << buf;

	static const char* other_setting =
	  "[Other]\n\n"
	  "; Preferred OpenGL Version (must be 3.3 or between 4.0 to 4.6).\n"
	  "gl_ver_major=%d\n"
	  "gl_ver_minor=%d\n\n"
	  "; Use compute shader (enabling this might be better on some gpu).\n"
	  "use_compute_shader=%s\n\n"
	  "; Frame Latency (how many frames cpu generate before rendering).\n"
	  "; Set 1-5 (increasing this value notice less frame stutter but introduces more input lag).\n"
	  "frame_latency=%d\n\n"
	  "; Comma delimitered DLLs to load (early: right after attached).\n"
	  "load_dlls_early=%s\n\n"
	  "; Comma delimitered DLLs to load (late: right after window created).\n"
	  "load_dlls_late=%s\n";

	sprintf_s(buf, other_setting,
	  App.gl_ver_major,
	  App.gl_ver_minor,
	  boolString(App.use_compute_shader),
	  App.frame_latency,
	  App.dlls_early.c_str(),
	  App.dlls_late.c_str());
	out_file << buf;

	out_file.close();
}

void loadIni()
{
	App.ini_file = std::string(helpers::getCurrentDir() + App.ini_file);
	App.desktop_resolution = {
		GetSystemMetrics(SM_XVIRTUALSCREEN),
		GetSystemMetrics(SM_YVIRTUALSCREEN),
		GetSystemMetrics(SM_CXVIRTUALSCREEN),
		GetSystemMetrics(SM_CYVIRTUALSCREEN),
	};

	for (auto& shader : g_shader_upscale)
		App.shader.items.push_back({ shader.name });

	App.lut.items.push_back({ "Game Default" });
	for (int i = 1; i <= 14; i++) {
		char label[50] = { 0 };
		sprintf_s(label, 50, "Color #%d", i);
		App.lut.items.push_back({ label });
	}

	if (helpers::fileExists(App.ini_file)) {
		App.window.fullscreen = getBool("Screen", "fullscreen", App.window.fullscreen);
		App.window.dark_mode = getBool("Screen", "dark_mode", App.window.dark_mode);
		App.vsync = getBool("Screen", "vsync", App.vsync);

		App.window.size.x = getInt("Screen", "window_width", App.window.size.x, 800, App.desktop_resolution.z);
		App.window.size.y = getInt("Screen", "window_height", App.window.size.y, 600, App.desktop_resolution.w);
		App.window.size_save = App.window.size;

		App.window.centered = getBool("Screen", "centered_window", App.window.centered);
		App.window.position.x = getInt("Screen", "window_posx", App.window.position.x, App.desktop_resolution.x, App.desktop_resolution.z);
		App.window.position.y = getInt("Screen", "window_posy", App.window.position.y, App.desktop_resolution.y, App.desktop_resolution.w);

		App.foreground_fps.active = getBool("Screen", "foreground_fps", App.foreground_fps.active);
		App.foreground_fps.range.value = getInt("Screen", "foreground_fps_value", App.foreground_fps.range.value, App.foreground_fps.range.min, App.foreground_fps.range.max);
		App.background_fps.active = getBool("Screen", "background_fps", App.background_fps.active);
		App.background_fps.range.value = getInt("Screen", "background_fps_value", App.background_fps.range.value, App.background_fps.range.min, App.background_fps.range.max);

		App.shader.selected = getInt("Graphic", "shader", App.shader.selected, 0, App.shader.items.size() - 1);
		App.lut.selected = getInt("Graphic", "lut", App.lut.selected, 0, App.lut.items.size() - 1);
		App.fxaa = getBool("Graphic", "fxaa", App.fxaa);

		App.sharpen.active = getBool("Graphic", "sharpen", App.sharpen.active);
		App.sharpen.strength.value = getFloat("Graphic", "sharpen_strength", App.sharpen.strength);
		App.sharpen.clamp.value = getFloat("Graphic", "sharpen_clamp", App.sharpen.clamp);
		App.sharpen.radius.value = getFloat("Graphic", "sharpen_radius", App.sharpen.radius);

		App.bloom.active = getBool("Graphic", "bloom", App.bloom.active);
		App.bloom.exposure.value = getFloat("Graphic", "bloom_exposure", App.bloom.exposure);
		App.bloom.gamma.value = getFloat("Graphic", "bloom_gamma", App.bloom.gamma);

		App.hd_cursor = getBool("Feature", "hd_cursor", App.hd_cursor);
		App.hd_text = getBool("Feature", "hd_text", App.hd_text);

		// App.hd_orbs.active = getBool("Feature", "hd_orbs", App.hd_orbs.active);
		// App.hd_orbs.centered = getBool("Feature", "hd_orbs_centered", App.hd_orbs.centered);

		App.mini_map.active = getBool("Feature", "mini_map", App.mini_map.active) && App.api == Api::Glide;
		App.mini_map.text_over = getBool("Feature", "mini_map_text_over", App.mini_map.text_over);
		App.mini_map.width.value = getInt("Feature", "mini_map_width", App.mini_map.width.value, App.mini_map.width.min, App.mini_map.width.max);
		App.mini_map.height.value = getInt("Feature", "mini_map_height", App.mini_map.height.value, App.mini_map.height.min, App.mini_map.height.max);

		App.motion_prediction = getBool("Feature", "motion_prediction", App.motion_prediction);
		App.skip_intro = getBool("Feature", "skip_intro", App.skip_intro);
		App.no_pickup = getBool("Feature", "no_pickup", App.no_pickup);
		App.show_fps = getBool("Feature", "show_fps", App.show_fps);
		App.cursor.no_lock = getBool("Feature", "no_cursor_lock", App.cursor.no_lock);

		App.gl_ver_major = getInt("Other", "gl_ver_major", App.gl_ver_major, 3, 4);
		App.gl_ver_minor = getInt("Other", "gl_ver_minor", App.gl_ver_minor, 0, 6);
		if (App.gl_ver_major == 3)
			App.gl_ver_minor = 3;

		App.use_compute_shader = getBool("Other", "use_compute_shader", App.use_compute_shader);
		App.frame_latency = getInt("Other", "frame_latency", App.frame_latency, 1, 5);

		App.dlls_early = getString("Other", "load_dlls_early", App.dlls_early);
		App.dlls_late = getString("Other", "load_dlls_late", App.dlls_late);
	}

	// clang-format off
	std::vector<std::pair<uint32_t, uint32_t>> window_sizes = {
		{  800, 600 }, {  960, 720 }, { 1024, 768 }, { 1200,  900 }, { 1280,  960 }, { 1440, 1080 }, { 1600, 1200 }, { 1920, 1440 }, { 2560, 1920 }, { 2732, 2048 },
		{ 1068, 600 }, { 1280, 720 }, { 1600, 900 }, { 1920, 1080 }, { 2048, 1152 }, { 2560, 1440 }, { 3200, 1800 }, { 3840, 2160 },
	};
	// clang-format on

	int index = 1;
	App.resolutions.items.push_back({ "Custom Size", glm::uvec2(0, 0) });
	for (size_t i = 0; i < window_sizes.size(); i++) {
		const auto& p = window_sizes[i];
		if (App.desktop_resolution.z <= p.first || App.desktop_resolution.w <= p.second)
			continue;

		char label[50] = { 0 };
		bool is_4x3 = p.first / 4 == p.second / 3;
		sprintf_s(label, 50, "%d x %d (%s)", p.first, p.second, (is_4x3 ? "4:3" : "16:9"));
		App.resolutions.items.push_back({ label, glm::uvec2(p.first, p.second) });

		if (App.window.size.x == p.first && App.window.size.y == p.second)
			App.resolutions.selected = index;
		index++;
	}

	saveIni();
}

}