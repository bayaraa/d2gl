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
#include "d2/common.h"
#include "helpers.h"
#include "option/ini.h"
#include "win32.h"

namespace d2gl {

D2GLApp App;

void dllAttach(HMODULE hmodule)
{
	std::string command_line = GetCommandLineA();
	helpers::strToLower(command_line);

	if (command_line.find("d2vidtst") != std::string::npos) {
		App.video_test = true;
		return;
	}

	bool flag_3dfx = command_line.find("-3dfx") != std::string::npos;
	flag_3dfx = !flag_3dfx ? *d2::video_mode == 4 : flag_3dfx;

	if ((App.api == Api::Glide && !flag_3dfx) || (App.api == Api::DDraw && flag_3dfx))
		return;

	if (command_line.find("-w ") != std::string::npos || command_line.find("-w") == command_line.length() - 2) {
		if (App.api == Api::Glide && flag_3dfx) {
			MessageBoxA(NULL, "D2GL Glide wrapper is not compatible with \"-w\" flag.\nRemove \"-w\" flag and run game again.", "Unsupported argument detected!", MB_OK | MB_ICONWARNING);
			exit(1);
		}
		return;
	}

	App.log = command_line.find("-log") != std::string::npos;
	App.direct = command_line.find("-direct") != std::string::npos;

	logInit();
	trace_log("Renderer Api: %s", App.api == Api::Glide ? "Glide" : "DDraw");

	auto ini_pos = command_line.find("-config ");
	if (ini_pos != std::string::npos) {
		std::string custom_ini = "";
		for (size_t i = ini_pos + 8; i < command_line.length(); i++) {
			if (command_line.at(i) == ' ')
				break;
			custom_ini += command_line.at(i);
		}
		custom_ini.erase(std::remove(custom_ini.begin(), custom_ini.end(), ' '), custom_ini.end());
		if (custom_ini.length() > 0) {
			App.ini_file = "d2gl_" + custom_ini + ".ini";
			trace_log("Custom config file: %s", App.ini_file.c_str());
		}
	}

	if (helpers::getVersion() == Version::Unknown) {
		MessageBoxA(NULL, "Game version is not supported!", "Unsupported version!", MB_OK | MB_ICONERROR);
		error_log("Game version is not supported!");
		exit(1);
	}
	trace_log("Diablo 2 LoD (%s) version %s detected.", helpers::getLangString().c_str(), helpers::getVersionString().c_str());

	timeBeginPeriod(1);
	win32::setDPIAwareness();
	App.hmodule = hmodule;

	option::loadIni();
	helpers::loadDlls(App.dlls_early);

	d2::initHooks();
	win32::initHooks();
}

void dllDetach()
{
	if (App.hmodule) {
		win32::destroyHooks();
		d2::destroyHooks();
		timeEndPeriod(1);
		exit(EXIT_SUCCESS);
	}
}

}