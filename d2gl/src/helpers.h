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

// clang-format off
#define EXE_GAME     "game.exe"
#define DLL_FOG      "fog.dll"
#define DLL_D2WIN    "d2win.dll"
#define DLL_D2GFX    "d2gfx.dll"
#define DLL_D2GLIDE  "d2glide.dll"
#define DLL_D2SOUND  "d2sound.dll"
#define DLL_D2CLIENT "d2client.dll"
#define DLL_D2COMMON "d2common.dll"
#define DLL_D2LAUNCH "d2launch.dll"
#define DLL_D2LANG   "d2lang.dll"
#define DLL_USER32   "user32.dll"
#define DLL_NTDLL    "ntdll.dll"
#define DLL_SHCORE   "shcore.dll"

#define UNPACK(...) __VA_ARGS__
#define getOffset(def, v109d, v110, v111, v111b, v112, v113c, v113d, v114d) helpers::getVersionOffset({ UNPACK def }, { UNPACK v109d }, { UNPACK v110 }, { UNPACK v111 }, { UNPACK v111b }, { UNPACK v112 }, { UNPACK v113c }, { UNPACK v113d }, { UNPACK v114d })
#define getProc(def, v109d, v110, v111, v111b, v112, v113c, v113d, v114d)   helpers::getProcOffset(getOffset(def, v109d, v110, v111, v111b, v112, v113c, v113d, v114d))
#define isVer(ver)    helpers::getVersion() == Version::##ver
#define isVerNot(ver) helpers::getVersion() != Version::##ver
#define isVerMin(ver) helpers::getVersion() >= Version::##ver
#define isVerMax(ver) helpers::getVersion() <= Version::##ver
// clang-format on

namespace d2gl::helpers {

std::string getCurrentDir();
bool fileExists(std::string file_path);

std::vector<std::string> strToLines(const std::string& str);
std::vector<std::wstring> strToLines(const std::wstring& str);

void replaceAll(std::string& str, const std::string& from, const std::string& to);
void strToLower(std::string& str);

std::string getLangString();

Version getVersion();
std::string getVersionString();
Offset getVersionOffset(OffsetDefault def_offset, Offset v109d, Offset v110, Offset v111, Offset v111b, Offset v112, Offset v113c, Offset v113d, Offset v114d);

uintptr_t getProcOffset(Offset offset);
uintptr_t getProcOffset(LPCSTR module, int offset);
uintptr_t getProcOffset(LPCSTR module, LPCSTR function);

uint32_t hash(const void* key, size_t len);

BufferData loadFile(const std::string& file_path);
ImageData loadImage(const std::string& file_path, bool flipped = true);
void clearImage(ImageData& image);
std::string saveScreenShot(uint8_t* data, int width, int height);

void loadDlls(const std::string& dlls, bool late = false);

}