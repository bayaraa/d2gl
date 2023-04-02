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

enum class Api {
	Glide,
	DDraw,
};

enum class Version {
	Null = -1,
	Unknown,
	V_109d,
	V_110,
	V_111,
	V_111b,
	V_112,
	V_113c,
	V_113d,
	V_114d,
};

enum class GameScreen {
	Menu,
	InGame,
	Movie,
	Loading,
};

enum class DrawStage : uint8_t {
	World,
	UI,
	Map,
	HUD,
	Cursor,
};

enum class D2DrawFn {
	None,
	Image,
	PerspectiveImage,
	ShiftedImage,
	VerticalCropImage,
	ClippedImage,
	ImageFast,
	Shadow,
	SolidRectEx,
	NormalText,
	NormalTextEx,
	FramedText,
	RectangledText,
};

struct Vertex {
	glm::vec<2, int16_t> position;
	glm::vec2 tex_coord;
	uint32_t color1;
	uint32_t color2;
	uint16_t tex_num;
	uint16_t flags;
};

struct Offset {
	int pos = 0;
	uint32_t og_4bytes = 0;
	int add = 0;
	LPCSTR module = 0;
};

struct OffsetDefault {
	LPCSTR module = 0;
	uint32_t og_4bytes = 0;
	int add = 0;
};

struct BufferData {
	size_t size = 0;
	uint8_t* data = nullptr;
};

struct ImageData {
	int width = 0;
	int height = 0;
	int bit = 0;
	uint8_t* data = nullptr;
};

template <typename T>
struct SelectItem {
	std::string name;
	T value;
};

template <typename T>
struct Select {
	int selected = 0;
	std::vector<SelectItem<T>> items;
};

template <typename T>
struct Range {
	T value;
	T min;
	T max;
};

typedef void (*onStageChange_t)(void);

}