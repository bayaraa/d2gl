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
#define DLL_USER32   "user32.dll"

#define UNPACK(...) __VA_ARGS__
#define getOffset(def, v109d, v110f, v111a, v111b, v112a, v113c, v113d, v114d) helpers::getVersionOffset({ UNPACK def }, { UNPACK v109d }, { UNPACK v110f }, { UNPACK v111a }, { UNPACK v111b }, { UNPACK v112a }, { UNPACK v113c }, { UNPACK v113d }, { UNPACK v114d })
#define getProc(def, v109d, v110f, v111a, v111b, v112a, v113c, v113d, v114d)   helpers::getProcOffset(getOffset(def, v109d, v110f, v111a, v111b, v112a, v113c, v113d, v114d))
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

Version getVersion();
std::string getVersionString();
Offset getVersionOffset(OffsetDefault def_offset, Offset v109d, Offset v110f, Offset v111a, Offset v111b, Offset v112a, Offset v113c, Offset v113d, Offset v114d);

uintptr_t getProcOffset(Offset offset);
uintptr_t getProcOffset(LPCSTR module, int offset);
uintptr_t getProcOffset(LPCSTR module, LPCSTR function);

uint32_t hash(const void* key, size_t len);

BufferData loadFile(const std::string& file_path);
ImageData loadImage(const std::string& file_path, bool flipped = true);
void clearImage(ImageData& image);

}