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

void checkCompatibilityMode()
{
	char buffer[1024] = { 0 };
	if (GetEnvironmentVariableA("__COMPAT_LAYER", buffer, sizeof(buffer))) {
		char* context = NULL;
		char* str = strtok_s(buffer, " ", &context);
		while (str) {
			std::string str_s(str);
			if (str_s == "WIN95" || str_s == "WIN98" || str_s == "NT4SP5") {
				char msg[128] = { 0 };
				sprintf_s(msg, "Please disable the '%s' compatibility mode for game executable and then try to start the game again.", str);
				MessageBoxA(NULL, msg, "Compatibility mode detected!", MB_OK);
				error_log("Compatibility mode '%s' detected!", str);
				break;
			}
			str = strtok_s(NULL, " ", &context);
		}
	}
}

void dllAttach(HMODULE hmodule)
{
	std::string command_line = GetCommandLineA();
	helpers::strToLower(command_line);

	if (command_line.find("-w ") != std::string::npos || command_line.find("-w") == command_line.length() - 2)
		return;

	if (command_line.find("d2vidtst") != std::string::npos) {
		App.video_test = true;
		return;
	}

	bool flag_3dfx = command_line.find("-3dfx") != std::string::npos;
	flag_3dfx = !flag_3dfx ? *d2::video_mode == 4 : flag_3dfx;

	if ((App.api == Api::Glide && !flag_3dfx) || (App.api == Api::DDraw && flag_3dfx))
		return;

	if (command_line.find("-log") != std::string::npos)
		App.log = true;

	logInit();

	if (helpers::getVersion() == Version::Unknown) {
		MessageBoxA(NULL, "Game version is not supported!", "Unsupported version!", MB_OK | MB_ICONERROR);
		error_log("Game version is not supported!");
		exit(1);
	}
	trace_log("Diablo 2 LoD version %s detected.", helpers::getVersionString().c_str());

	timeBeginPeriod(1);
	checkCompatibilityMode();
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