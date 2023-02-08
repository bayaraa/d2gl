#pragma once

namespace d2gl {

enum class Api {
	Glide3x,
	DDraw,
};

enum class Version {
	Null = -1,
	Unknown,
	V_109d,
	V_110f,
	V_111a,
	V_111b,
	V_112a,
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