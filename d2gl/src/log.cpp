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

namespace d2gl {

FILE* log_file = nullptr;

void logInit()
{
#ifdef _DEBUG
	AllocConsole();
	freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
#endif

	if (App.log && logFileOpen("w") && log_file) {
		time_t now = time(0);
		tm gmt_time;
		localtime_s(&gmt_time, &now);
		fprintf(log_file, "== D2GL v%s logging started. (%d/%d/%d) ==\n\n", App.version_str.c_str(), gmt_time.tm_year + 1900, gmt_time.tm_mon + 1, gmt_time.tm_mday);
		logFileClose();
	}
}

void logTrace(WORD color, bool newline, const char* format, ...)
{
	static HANDLE hnd = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hnd, color);

	va_list args;
	va_start(args, format);
	vprintf(format, args);
	va_end(args);

	if (newline)
		printf("\n");
}

void logTraceDef(uint8_t type, const char* format, ...)
{
	static HANDLE hnd = GetStdHandle(STD_OUTPUT_HANDLE);
	if (type == 1) {
		SetConsoleTextAttribute(hnd, C_RED);
		printf("Error: ");
	} else if (type == 2) {
		SetConsoleTextAttribute(hnd, C_YELLOW);
		printf("Warning: ");
	}
	SetConsoleTextAttribute(hnd, C_WHITE);

	va_list args;
	va_start(args, format);
	vprintf(format, args);
	va_end(args);
	printf("\n");
}

bool logFileOpen(const char* mode)
{
	return fopen_s(&log_file, App.log_file.c_str(), mode) == 0;
}

void logFileWrite(uint8_t type, const char* format, ...)
{
	if (App.log && logFileOpen("a") && log_file) {
		time_t now = time(0);
		tm gmt_time;
		localtime_s(&gmt_time, &now);
		fprintf(log_file, "[%.2d:%.2d:%.2d][%s] ", gmt_time.tm_hour, gmt_time.tm_min, gmt_time.tm_sec, (type == 0 ? "INFO" : (type == 1 ? "ERROR" : "WARNING")));

		va_list args;
		va_start(args, format);
		vfprintf(log_file, format, args);
		va_end(args);
		fprintf(log_file, "\n");
		logFileClose();
	}
}

void logFileClose()
{
	fclose(log_file);
	log_file = nullptr;
}

}