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

enum class WINDCOMPATTR {
	WCA_UNDEFINED = 0,
	WCA_USEDARKMODECOLORS = 26,
	WCA_LAST = 27
};
struct WINDCOMPATTRDATA {
	WINDCOMPATTR Attrib;
	PVOID pvData;
	SIZE_T cbData;
};
enum class PROCESS_DPI_AWARENESS {
	PROCESS_DPI_UNAWARE = 0,
	PROCESS_SYSTEM_DPI_AWARE = 1,
	PROCESS_PER_MONITOR_DPI_AWARE = 2
};

typedef int(WINAPI* ShowCursor_t)(BOOL bShow);
typedef BOOL(WINAPI* SetCursorPos_t)(int X, int Y);
typedef BOOL(WINAPI* SetWindowPos_t)(HWND hWnd, HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags);
typedef LRESULT(WINAPI* SendMessageA_t)(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
typedef int(WINAPI* DrawTextA_t)(HDC hdc, LPCSTR lpchText, int cchText, LPRECT lprc, UINT format);
typedef COLORREF(WINAPI* GetPixel_t)(HDC hdc, int x, int y);

extern ShowCursor_t ShowCursor_Og;
extern SetCursorPos_t SetCursorPos_Og;
extern SetWindowPos_t SetWindowPos_Og;
extern SendMessageA_t SendMessageA_Og;
extern DrawTextA_t DrawTextA_Og;
extern GetPixel_t GetPixel_Og;

namespace d2gl::win32 {

void initHooks();
void destroyHooks();

int WINAPI ShowCursor(BOOL bShow);
BOOL WINAPI SetCursorPos(int X, int Y);
BOOL WINAPI SetWindowPos(HWND hWnd, HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags);
LRESULT WINAPI SendMessageA(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
int WINAPI DrawTextA(HDC hdc, LPCSTR lpchText, int cchText, LPRECT lprc, UINT format);
COLORREF WINAPI GetPixel(HDC hdc, int x, int y);
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

void setWindow(HWND hwnd);
void setWindowRect();
void setWindowMetrics();
void setCursorLock();
void setCursorUnlock();
void windowResize();
void toggleDarkmode();
void setDPIAwareness();
RTL_OSVERSIONINFOW getOSVersion();

}