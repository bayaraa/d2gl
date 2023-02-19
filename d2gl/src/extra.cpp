#include "pch.h"
#include "extra.h"

namespace d2gl {

bool isPD2()
{
	HANDLE handle = GetModuleHandleA("ProjectDiablo.dll");
	return (bool)handle;
}

bool fixPD2drawShiftedImage(const d2::CellContext* cell)
{
	if (App.pd2_fix && App.api == Api::Glide && (uintptr_t)&cell == 0x19F63C)
		return true;

	return false;
}

bool fixPD2drawGroundTile(const d2::TileContext* tile)
{
	if (App.pd2_fix && App.api == Api::Glide && tile) {
		const auto len = strlen(tile->szTileName);
		if (len > 8) {
			const auto name = tile->szTileName + len - 8;
			if (strcmp(name, "Warp.dt1") == 0)
				return true;
		}
	}

	return false;
}

}

#ifdef __cplusplus
extern "C" {
#endif

__declspec(dllexport) void __stdcall setCustomScreenSize(uint32_t width, uint32_t height)
{
	d2gl::App.game.custom_size = { width, height };
}

#ifdef __cplusplus
}
#endif