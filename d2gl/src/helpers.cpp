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
#include "helpers.h"
#include "d2/common.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#define __STDC_LIB_EXT1__
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"

namespace d2gl::helpers {

std::string getCurrentDir()
{
	auto path = std::filesystem::current_path() / "";
	return path.string();
}

bool fileExists(std::string file_path)
{
	return std::filesystem::exists(file_path) && std::filesystem::is_regular_file(file_path);
}

std::string filePathFix(std::string parent_file_path, std::string file_path)
{
	helpers::replaceAll(parent_file_path, "/", "\\");
	parent_file_path = parent_file_path.substr(0, parent_file_path.rfind('\\'));

	helpers::replaceAll(file_path, "/", "\\");
	if (file_path.find(".\\") == 0)
		file_path = file_path.substr(2);

	size_t occurence_pos = 0;
	while (file_path.find("..\\", occurence_pos) != std::string::npos)
		occurence_pos += 3;

	if (occurence_pos) {
		file_path = file_path.substr(occurence_pos);
		for (size_t i = 0; i < occurence_pos / 3; i++) {
			size_t pos = parent_file_path.rfind("\\");
			if (pos != std::string::npos)
				parent_file_path = parent_file_path.substr(0, pos);
		}
	}
	return parent_file_path + "\\" + file_path;
}

std::vector<std::string> strToLines(const std::string& str)
{
	std::vector<std::string> result;
	auto ss = std::stringstream(str);

	for (std::string line; std::getline(ss, line, '\n');)
		result.push_back(line);

	return result;
}

std::vector<std::wstring> strToLines(const std::wstring& str)
{
	std::vector<std::wstring> result;
	auto ss = std::wstringstream(str);

	for (std::wstring line; std::getline(ss, line, L'\n');)
		result.push_back(line);

	return result;
}

std::vector<std::string> splitToVector(const std::string& str, char delimeter)
{
	uint32_t index = 0;
	std::vector<std::string> segments = { "" };

	for (auto& c : str) {
		if (c == delimeter) {
			segments.push_back("");
			index++;
		} else
			segments[index].push_back(c);
	}

	return segments;
}

void replaceAll(std::string& str, const std::string& from, const std::string& to)
{
	size_t start_pos = 0;
	while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
		str.replace(start_pos, from.length(), to);
		start_pos += to.length();
	}
}

void strToLower(std::string& str)
{
	std::transform(str.begin(), str.end(), str.begin(), [](uint8_t c) { return std::tolower(c); });
}

void trimString(std::string& str, const char* chars)
{
	str.erase(0, str.find_first_not_of(chars));
	str.erase(str.find_last_not_of(chars) + 1);
}

std::string getLangString(bool path)
{
	if (path) {
		switch (d2::getLangId()) {
			case LANG_ESP: return "esp";
			case LANG_DEU: return "deu";
			case LANG_FRA: return "fra";
			case LANG_POR: return "por";
			case LANG_ITA: return "ita";
			case LANG_JPN: return "jpn";
			case LANG_KOR: return "kor";
			case LANG_SIN: return "sin";
			case LANG_CHI: return "chi";
			case LANG_POL: return "pol";
			case LANG_RUS: return "rus";
		}
		return "eng";
	}

	switch (d2::getLangId()) {
		case LANG_ESP: return "Spanish";
		case LANG_DEU: return "German";
		case LANG_FRA: return "French";
		case LANG_POR: return "Portuguese";
		case LANG_ITA: return "Italian";
		case LANG_JPN: return "Japanese";
		case LANG_KOR: return "Korean";
		case LANG_SIN: return "Singaporean";
		case LANG_CHI: return "Chinese";
		case LANG_POL: return "Polish";
		case LANG_RUS: return "Russian";
	}
	return "English";
}

Version getVersion()
{
	static Version version = Version::Null;
	if (version != Version::Null)
		return version;

	version = Version::Unknown;

	DWORD ver_handle = 0;
	DWORD ver_size = GetFileVersionInfoSizeA(EXE_GAME, &ver_handle);
	if (ver_size == 0 || ver_handle != 0)
		return version;

	std::unique_ptr<wchar_t[]> ver_data(new wchar_t[ver_size]);
	if (!GetFileVersionInfoA(EXE_GAME, ver_handle, ver_size, ver_data.get()))
		return version;

	size_t size = 0;
	LPVOID buffer = nullptr;
	if (!VerQueryValueA(ver_data.get(), "\\", &buffer, &size) || size == 0)
		return version;

	auto ver_info = (VS_FIXEDFILEINFO*)buffer;
	if (ver_info->dwSignature != 0xfeef04bd)
		return version;

	std::ostringstream ss;
	// clang-format off
	ss << ((ver_info->dwFileVersionMS >> 16) & 0xffff) << ".";
	ss << ((ver_info->dwFileVersionMS >>  0) & 0xffff) << ".";
	ss << ((ver_info->dwFileVersionLS >> 16) & 0xffff) << ".";
	ss << ((ver_info->dwFileVersionLS >>  0) & 0xffff);

	     if (ss.str() == "1.0.9.22" ) version = Version::V_109d;
	else if (ss.str() == "1.0.10.39") version = Version::V_110;
	else if (ss.str() == "1.0.11.45") version = Version::V_111;
	else if (ss.str() == "1.0.11.46") version = Version::V_111b;
	else if (ss.str() == "1.0.12.49") version = Version::V_112;
	else if (ss.str() == "1.0.13.60") version = Version::V_113c;
	else if (ss.str() == "1.0.13.64") version = Version::V_113d;
	else if (ss.str() == "1.14.3.71") version = Version::V_114d;
	// clang-format on

	return version;
}

std::string getVersionString()
{
	// clang-format off
	switch (getVersion()) {
		case Version::V_109d: return "1.09d";
		case Version::V_110:  return "1.10";
		case Version::V_111:  return "1.11";
		case Version::V_111b: return "1.11b";
		case Version::V_112:  return "1.12";
		case Version::V_113c: return "1.13c";
		case Version::V_113d: return "1.13d";
		case Version::V_114d: return "1.14d";
	}
	// clang-format on

	return "Unknown";
}

Offset getVersionOffset(OffsetDefault def_offset, Offset v109d, Offset v110, Offset v111, Offset v111b, Offset v112, Offset v113c, Offset v113d, Offset v114d)
{
	Offset offset;
	// clang-format off
	switch (getVersion()) {
		case Version::V_109d: offset = v109d; break;
		case Version::V_110:  offset = v110;  break;
		case Version::V_111:  offset = v111;  break;
		case Version::V_111b: offset = v111b; break;
		case Version::V_112:  offset = v112;  break;
		case Version::V_113c: offset = v113c; break;
		case Version::V_113d: offset = v113d; break;
		case Version::V_114d: offset = v114d;
			if (!offset.module)
				offset.module = EXE_GAME;
		break;
	}
	// clang-format on

	if (!offset.module)
		offset.module = def_offset.module;

	if (!offset.og_4bytes && def_offset.og_4bytes)
		offset.og_4bytes = def_offset.og_4bytes;

	if (offset.add == 0 && def_offset.add)
		offset.add = def_offset.add;
	else if (offset.add == -1)
		offset.add = 0;

	return offset;
}

HMODULE getOrLoadModule(LPCSTR module)
{
	static std::unordered_map<LPCSTR, HMODULE> offsetCache;

	HMODULE handle = NULL;

	if (offsetCache.find(module) != offsetCache.end())
		handle = offsetCache[module];
	else {
		handle = GetModuleHandleA(module);
		if (handle == NULL)
			handle = LoadLibraryA(module);

		offsetCache[module] = handle;
	}
	return handle;
}

uintptr_t getProcOffset(Offset offset)
{
	uintptr_t address = getProcOffset(offset.module, offset.pos) + offset.add;
	if (address && offset.og_4bytes) {
		uint8_t* bytes = (uint8_t*)&offset.og_4bytes;
		uint32_t original_4bytes = bytes[0] << 24 | bytes[1] << 16 | bytes[2] << 8 | bytes[3];

		if (*(uint32_t*)address != original_4bytes) {
			bytes = (uint8_t*)address;
			uint32_t addr_4bytes = bytes[0] << 24 | bytes[1] << 16 | bytes[2] << 8 | bytes[3];
			error_log("Offset: %s, %d(0x%.8X), %d(0x%.8X). Original bytes are not equal: 0x%.8X != 0x%.8X", offset.module, offset.pos, offset.pos, offset.add, offset.add, addr_4bytes, offset.og_4bytes);
			return NULL;
		}
	}
	return address;
}

uintptr_t getProcOffset(LPCSTR module, int offset)
{
	HMODULE handle = getOrLoadModule(module);
	if (handle != NULL) {
		if (offset < 0)
			return (uintptr_t)GetProcAddress(handle, (LPCSTR)(-offset));

		return ((uintptr_t)handle) + offset;
	}
	return NULL;
}

uintptr_t getProcOffset(LPCSTR module, LPCSTR function)
{
	HMODULE handle = getOrLoadModule(module);
	if (handle != NULL)
		return (uintptr_t)GetProcAddress(handle, function);

	return NULL;
}

uint32_t hash(const void* key, size_t len)
{
	const uint8_t* data = (const uint8_t*)key;
	const int nblocks = len / 4;
	const uint32_t c1 = 0xcc9e2d51;
	const uint32_t c2 = 0x1b873593;

	const uint32_t* blocks = (const uint32_t*)(data + nblocks * 4);
	uint32_t h1 = 0;

	for (int i = -nblocks; i; i++) {
		uint32_t k1 = blocks[i];

		k1 *= c1;
		k1 = _rotl(k1, 15);
		k1 *= c2;

		h1 ^= k1;
		h1 = _rotl(h1, 13);
		h1 = h1 * 5 + 0xe6546b64;
	}

	const uint8_t* tail = (const uint8_t*)(data + nblocks * 4);
	uint32_t k1 = 0;

	switch (len & 3) {
		case 3:
			k1 ^= tail[2] << 16;
		case 2:
			k1 ^= tail[1] << 8;
		case 1:
			k1 ^= tail[0];
			k1 *= c1;
			k1 = _rotl(k1, 15);
			k1 *= c2;
			h1 ^= k1;
	};

	h1 ^= len;
	h1 ^= h1 >> 16;
	h1 *= 0x85ebca6b;
	h1 ^= h1 >> 13;
	h1 *= 0xc2b2ae35;
	h1 ^= h1 >> 16;

	return h1;
}

BufferData loadFile(const std::string& file_path)
{
	static bool is_mpq_loaded = false;
	if (!is_mpq_loaded) {
		std::string mpq_path = getCurrentDir() + App.mpq_file;
		if (!d2::mpqLoad(mpq_path.c_str()))
			error_log("%s not loaded.", mpq_path.c_str());
		is_mpq_loaded = true;
	}

	std::string path = std::string("data\\").append(file_path);

	void* ref_file;
	char c_filepath[MAX_PATH];
	strncpy_s(c_filepath, path.c_str(), path.size());

	if (d2::mpqOpenFile && d2::mpqOpenFile(c_filepath, &ref_file)) {
		size_t return_size = 0;
		DWORD fileSize = d2::mpqGetFileSize(ref_file, NULL);
		uint8_t* cache = new uint8_t[fileSize + 1];

		if (d2::mpqReadFile(ref_file, cache, fileSize, &return_size, NULL, NULL, NULL)) {
			d2::mpqCloseFile(ref_file);

			cache[fileSize] = 0;
			return { return_size, cache };
		} else
			error_log("File (MPQ): \"%s\" read error.", path.c_str());

		delete[] cache;
		return { 0 };
	} else
		error_log("File (MPQ): \"%s\" could not opened!", path.c_str());

	return { 0 };
}

ImageData loadImage(const std::string& file_path, bool flipped)
{
	ImageData image = { 0 };

	auto buffer = loadFile(file_path);
	if (buffer.size) {
		stbi_set_flip_vertically_on_load(flipped);
		image.data = stbi_load_from_memory(buffer.data, buffer.size, &image.width, &image.height, &image.bit, 4);
		delete[] buffer.data;
	}

	return image;
}

void clearImage(ImageData& image)
{
	stbi_image_free(image.data);
}

std::string saveScreenShot(uint8_t* data, int width, int height)
{
	static const char* file_name_format = "Screenshot%03d.png";
	char file_name[30] = { 0 };

	for (size_t i = 1; i < 999; i++) {
		sprintf_s(file_name, file_name_format, i);
		if (!fileExists(file_name))
			break;
	}

	stbi_flip_vertically_on_write(1);
	stbi_write_png(file_name, width, height, 3, data, width * 3);

	return file_name;
}

void loadDlls(const std::string& dlls, bool late)
{
	trace_log("Loading %s DLLs.", late ? "late" : "early");
	auto ss = std::stringstream(dlls);

	for (std::string dll; std::getline(ss, dll, ',');) {
		dll.erase(remove_if(dll.begin(), dll.end(), isspace), dll.end());
		if (dll != "") {
			auto segments = splitToVector(dll, ':');
			auto handle = LoadLibraryA(segments[0].c_str());
			if (handle) {
				auto log_str = segments[0] + " loaded";
				if (segments.size() == 3) {
					bool called = false;
					if (segments[1] == "cdecl") {
						if (auto func = (void(__cdecl*)())GetProcAddress(handle, segments[2].c_str())) {
							called = true;
							func();
						}
					} else if (segments[1] == "stdcall") {
						if (auto func = (void(__stdcall*)())GetProcAddress(handle, segments[2].c_str())) {
							called = true;
							func();
						}
					} else if (segments[1] == "fastcall") {
						if (auto func = (void(__fastcall*)())GetProcAddress(handle, segments[2].c_str())) {
							called = true;
							func();
						}
					}
					if (called)
						log_str += " and " + segments[1] + " " + segments[2] + " function called";
				}
				trace_log("%s.", log_str.c_str());
				if (!late && dll == "d2fps.dll:stdcall:_Init@0") {
					App.d2fps_mod = true;
					App.foreground_fps.active = false;
					App.background_fps.active = false;
					App.motion_prediction = false;
				}
			} else {
				error_log("%s not loaded.", segments[0].c_str());
			}
		}
	}
}

}