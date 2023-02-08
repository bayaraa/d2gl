#include "pch.h"

namespace d2gl {

void logInit()
{
#ifdef _DEBUG
	AllocConsole();
	freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
#endif
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

}