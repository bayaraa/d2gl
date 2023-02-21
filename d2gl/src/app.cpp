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
				break;
			}
			str = strtok_s(NULL, " ", &context);
		}
	}
}

void checkDPIAwareness()
{
	bool setDpiAware = SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
	if (!setDpiAware) {
		HRESULT result = SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);
		setDpiAware = result == S_OK || result == E_ACCESSDENIED;
	}
	if (!setDpiAware)
		SetProcessDPIAware();
}

void dllAttach(HMODULE hmodule)
{
	auto flag_3dfx = strstr(GetCommandLineA(), "-3dfx");
	if ((App.api == Api::Glide && !flag_3dfx) || (App.api == Api::DDraw && flag_3dfx))
		return;

	App.hmodule = hmodule;

	if (strstr(GetCommandLineA(), "-log"))
		App.log = true;

	logInit();
	timeBeginPeriod(1);
	checkCompatibilityMode();
	checkDPIAwareness();

	if (helpers::getVersion() == Version::Unknown) {
		MessageBoxA(NULL, "Game version is not supported!", "Unsupported version!", MB_OK | MB_ICONERROR);
		error_log("Game version is not supported!");
		exit(1);
	}
	trace_log("Game version %s detected.", helpers::getVersionString().c_str());

	option::loadIni();
	trace_log("Loading early DLLs.");
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