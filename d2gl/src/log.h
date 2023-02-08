#pragma once

namespace d2gl {

void logInit();
void logTrace(WORD color, bool newline, const char* format, ...);
void logTraceDef(uint8_t type, const char* format, ...);

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
// clang-format on

}