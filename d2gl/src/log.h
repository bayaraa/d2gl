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

namespace d2gl {

void logInit();
void logTrace(WORD color, bool newline, const char* format, ...);
void logTraceDef(uint8_t type, const char* format, ...);

bool logFileOpen(const char* mode);
void logFileWrite(uint8_t type, const char* format, ...);
void logFileClose();

// clang-format off
#define C_GRAY    FOREGROUND_INTENSITY
#define C_WHITE   FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE
#define C_RED     FOREGROUND_INTENSITY | FOREGROUND_RED
#define C_GREEN   FOREGROUND_INTENSITY | FOREGROUND_GREEN
#define C_BLUE    FOREGROUND_INTENSITY | FOREGROUND_BLUE
#define C_YELLOW  FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN
#define C_MAGENTA FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_BLUE

#ifdef _DEBUG
#define trace(format, ...) logTraceDef(0, format, ##__VA_ARGS__)
#define error(format, ...) logTraceDef(1, format, ##__VA_ARGS__)
#define warn(format, ...) logTraceDef(2, format, ##__VA_ARGS__)
#else
#define trace(format, ...)
#define error(format, ...)
#define warn(format, ...)
#endif

#define trace_log(format, ...) { logFileWrite(0, format, ##__VA_ARGS__);trace(format, ##__VA_ARGS__); }
#define error_log(format, ...) { logFileWrite(1, format, ##__VA_ARGS__);error(format, ##__VA_ARGS__); }
#define warn_log(format, ...) { logFileWrite(2, format, ##__VA_ARGS__);warn(format, ##__VA_ARGS__); }
// clang-format on

}