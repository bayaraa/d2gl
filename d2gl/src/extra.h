#pragma once

#include "d2/structs.h"

namespace d2gl {

bool isPD2();
bool fixPD2drawShiftedImage(const d2::CellContext* cell);
bool fixPD2drawGroundTile(const d2::TileContext* tile);

}