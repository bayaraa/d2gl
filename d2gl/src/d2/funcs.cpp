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
#include "funcs.h"
#include "common.h"
#include "helpers.h"
#include "modules/hd_text.h"
#include "modules/motion_prediction.h"
#include "stubs.h"

namespace d2gl::d2 {

uint32_t mpqLoad(const char* mpqfile)
{
	if (isVer(V_114d))
		return ((mpqLoad_114_t)mpqLoad_Fn)(mpqfile, 0, 0, 0);
	else if (isVerMin(V_111))
		return mpqLoadStub("d2gl.dll", mpqfile, "d2gl", 0, 0);

	return ((mpqLoad_110_t)mpqLoad_Fn)("d2gl.dll", mpqfile, 0, 0, 0, 0, 0);
}

glm::ivec2 getCursorPos()
{
	if (App.game.screen == GameScreen::InGame)
		return { *mouse_x, *mouse_y };

	return { *mouse_x_menu, *mouse_y_menu };
}

bool isPerspective()
{
	const bool available = *(bool*)(perspective + 4);
	return *perspective && available;
}

bool isEscMenuOpen()
{
	const bool control = *(bool*)(esc_menu_open + 8);
	return *esc_menu_open || control;
}

UnitAny* getPlayerUnit()
{
	return (UnitAny*)*(uintptr_t*)player_unit;
}

UnitAny* findUnit(uint32_t type_id)
{
	const uint32_t unit_id = type_id & 0x00FFFFFF;
	const uint32_t unit_type = type_id >> 24;

	auto unit = findUnitClient(unit_id, unit_type);
	if (unit)
		return unit;

	return findUnitServer(unit_id, unit_type);
}

DWORD getUnitID(UnitAny* unit)
{
	return isVer(V_109d) ? unit->v109.dwUnitId : unit->v110.dwUnitId;
}

DWORD getUnitFlag(UnitAny* unit)
{
	return isVer(V_109d) ? unit->v109.dwFlags : unit->v110.dwFlags;
}

Path* getUnitPath(UnitAny* unit)
{
	return isVer(V_109d) ? unit->v109.pPath : unit->v110.pPath;
}

StaticPath* getUnitStaticPath(UnitAny* unit)
{
	return isVer(V_109d) ? unit->v109.pStaticPath : unit->v110.pStaticPath;
}

uint32_t getUnitStat(UnitAny* unit, uint32_t stat)
{
	return isVer(V_109d) ? ((getUnitStat109d_t)getUnitStat_Fn)(unit, stat) : getUnitStat_Fn(unit, stat, 0);
}

UnitAny* getSelectedItem()
{
	return (UnitAny*)*(uintptr_t*)selected_item;
}

bool isUnitDead(UnitAny* unit)
{
	return unit && (d2::getUnitFlag(unit) & 0x10000);
}

MonsterType getMonsterType(UnitAny* unit)
{
	if (isVer(V_109d)) {
		if (unit->v109.pMonsterData->fChamp)
			return MonsterType::Champion;
		else if (unit->v109.pMonsterData->fBoss)
			return MonsterType::Boss;
	} else {
		if (unit->v110.pMonsterData->fSuperUniq)
			return MonsterType::SuperUnique;
		else if (unit->v110.pMonsterData->fChamp || unit->v110.pMonsterData->fPossesed || unit->v110.pMonsterData->fGhostly)
			return MonsterType::Champion;
		else if (unit->v110.pMonsterData->fBoss)
			return MonsterType::Boss;
	}

	return MonsterType::Normal;
}

wchar_t* getMonsterName(UnitAny* unit)
{
	return isVer(V_109d) ? unit->v109.pMonsterData->wName : unit->v110.pMonsterData->wName;
}

ItemQuality getItemQuality(UnitAny* unit)
{
	return isVer(V_109d) ? unit->v109.pItemData->dwQuality : unit->v110.pItemData->dwQuality;
}

CellFile* getCellFile(CellContext* cell)
{
	if (isVerMin(V_113c))
		return cell->v113.pCellFile;
	else if (isVer(V_112))
		return cell->v112.pCellFile;
	else if (isVerMin(V_111))
		return cell->v111.pCellFile;
	return cell->v109.pCellFile;
}

DWORD getCellNo(CellContext* cell)
{
	if (isVerMin(V_113c))
		return cell->v113.nCellNo;
	else if (isVer(V_112))
		return cell->v112.nCellNo;
	else if (isVerMin(V_111))
		return cell->v111.nCellNo;
	return cell->v109.nCellNo;
}

void gameDrawBegin()
{
	if (App.game.screen == GameScreen::Loading || App.game.screen == GameScreen::Menu)
		App.game.screen = GameScreen::InGame;
}

void automapDrawBegin()
{
	App.game.draw_stage = DrawStage::Map;
	App.context->onStageChange();
}

void automapDrawEnd()
{
	App.game.draw_stage = DrawStage::HUD;
	App.context->onStageChange();
}

void uiDrawBegin()
{
	App.game.draw_stage = DrawStage::UI;
	App.context->onStageChange();
}

void uiDrawEnd()
{
	App.game.draw_stage = DrawStage::Cursor;
	App.context->onStageChange();
}

void __stdcall drawImageHooked(CellContext* cell, int x, int y, uint32_t gamma, int draw_mode, uint8_t* palette)
{
	if (App.hd_cursor && App.game.draw_stage >= DrawStage::Cursor)
		return;

	if (modules::HDText::Instance().drawImage(cell, x, y, gamma, draw_mode)) {
		const auto pos = modules::MotionPrediction::Instance().drawImage(x, y, D2DrawFn::Image, gamma, draw_mode);
		drawImage(cell, pos.x, pos.y, gamma, draw_mode, palette);
	}
}

void __stdcall drawPerspectiveImageHooked(CellContext* cell, int x, int y, uint32_t gamma, int draw_mode, int screen_mode, uint8_t* palette)
{
	const auto pos = modules::MotionPrediction::Instance().drawImage(x, y, D2DrawFn::PerspectiveImage);
	drawPerspectiveImage(cell, pos.x, pos.y, gamma, draw_mode, screen_mode, palette);
}

void __stdcall drawShiftedImageHooked(CellContext* cell, int x, int y, uint32_t gamma, int draw_mode, int global_palette_shift)
{
	if (modules::HDText::Instance().drawShiftedImage(cell, x, y, gamma, draw_mode)) {
		auto pos = modules::MotionPrediction::Instance().drawImage(x, y, D2DrawFn::ShiftedImage);
		drawShiftedImage(cell, pos.x, pos.y, gamma, draw_mode, global_palette_shift);
	}
}

void __stdcall drawVerticalCropImageHooked(CellContext* cell, int x, int y, int skip_lines, int draw_lines, int draw_mode)
{
	const auto pos = modules::MotionPrediction::Instance().drawImage(x, y, D2DrawFn::VerticalCropImage);
	drawVerticalCropImage(cell, pos.x, pos.y, skip_lines, draw_lines, draw_mode);
}

void __stdcall drawClippedImageHooked(CellContext* cell, int x, int y, void* crop_rect, int draw_mode)
{
	const auto pos = modules::MotionPrediction::Instance().drawImage(x, y, D2DrawFn::ClippedImage);
	drawClippedImage(cell, pos.x, pos.y, crop_rect, draw_mode);
}

void __stdcall drawImageFastHooked(CellContext* cell, int x, int y, uint8_t palette_index)
{
	const auto pos = modules::MotionPrediction::Instance().drawImage(x, y, D2DrawFn::ImageFast);
	drawImageFast(cell, pos.x, pos.y, palette_index);
}

void __stdcall drawShadowHooked(CellContext* cell, int x, int y)
{
	const auto pos = modules::MotionPrediction::Instance().drawImage(x, y, D2DrawFn::Shadow);
	drawShadow(cell, pos.x, pos.y);
}

void __stdcall drawSolidRectExHooked(int left, int top, int right, int bottom, uint32_t color, int draw_mode)
{
	auto offset = modules::MotionPrediction::Instance().drawSolidRect();
	if (!modules::HDText::Instance().drawSolidRect(left - offset.x, top - offset.y, right - offset.x, bottom - offset.y, color, draw_mode))
		drawSolidRectEx(left - offset.x, top - offset.y, right - offset.x, bottom - offset.y, color, draw_mode);
}

void __stdcall drawLineHooked(int x_start, int y_start, int x_end, int y_end, uint8_t color, uint8_t alpha)
{
	const auto offset = modules::MotionPrediction::Instance().drawLine(x_start, y_start);
	drawLine(x_start - offset.x, y_start - offset.y, x_end - offset.x, y_end - offset.y, color, alpha);
}

bool __stdcall drawGroundTileHooked(TileContext* tile, GFXLight* light, int x, int y, int world_x, int world_y, uint8_t alpha, int screen_panels, bool tile_data)
{
	// Drawing invisible tile crashes on glide mode.
	if (App.api == Api::Glide && tile) {
		const auto len = strlen(tile->szTileName);
		if (len > 8) {
			const auto name = tile->szTileName + len - 8;
			if (strcmp(name, "Warp.dt1") == 0)
				return true;
		}
	}

	const auto offset = modules::MotionPrediction::Instance().getGlobalOffset();
	return drawGroundTile(tile, light, x - offset.x, y - offset.y, world_x, world_y, alpha, screen_panels, tile_data);
}

bool __stdcall drawWallTileHooked(TileContext* tile, int x, int y, GFXLight* light, int screen_panels)
{
	const auto offset = modules::MotionPrediction::Instance().getGlobalOffset(true);
	return drawWallTile(tile, x - offset.x, y - offset.y, light, screen_panels);
}

bool __stdcall drawTransWallTileHooked(TileContext* tile, int x, int y, GFXLight* light, int screen_panels, uint8_t alpha)
{
	const auto offset = modules::MotionPrediction::Instance().getGlobalOffset(true);
	return drawTransWallTile(tile, x - offset.x, y - offset.y, light, screen_panels, alpha);
}

bool __stdcall drawShadowTileHooked(TileContext* tile, int x, int y, int draw_mode, int screen_panels)
{
	const auto offset = modules::MotionPrediction::Instance().getGlobalOffset(true);
	return drawShadowTile(tile, x - offset.x, y - offset.y, draw_mode, screen_panels);
}

void __fastcall takeScreenShotHooked()
{
	App.context->getCommandBuffer()->pushCommand(CommandType::TakeScreenShot);
}

void __fastcall drawNormalTextHooked(const wchar_t* str, int x, int y, uint32_t color, uint32_t centered)
{
	// Glide mode light gray text appears black. So direct to dark gray.
	if (App.api == Api::Glide && !App.hd_text && color == 15)
		color = 5;

	const auto pos = modules::MotionPrediction::Instance().drawText(str, x, y, D2DrawFn::NormalText);
	if (!modules::HDText::Instance().drawText(str, pos.x, pos.y, color, centered))
		drawNormalText(str, pos.x, pos.y, color, centered);
}

void __fastcall drawNormalTextExHooked(const wchar_t* str, int x, int y, uint32_t color, uint32_t centered, uint32_t trans_lvl)
{
	const auto pos = modules::MotionPrediction::Instance().drawText(str, x, y, D2DrawFn::NormalTextEx);
	if (!modules::HDText::Instance().drawText(str, pos.x, pos.y, color, centered))
		drawNormalTextEx(str, pos.x, pos.y, color, centered, trans_lvl);
}

void __fastcall drawFramedTextHooked(const wchar_t* str, int x, int y, uint32_t color, uint32_t centered)
{
	const auto pos = modules::MotionPrediction::Instance().drawText(str, x, y, D2DrawFn::FramedText);
	if (!modules::HDText::Instance().drawFramedText(str, pos.x, pos.y, color, centered))
		drawFramedText(str, pos.x, pos.y, color, centered);
}

void __fastcall drawRectangledTextHooked(const wchar_t* str, int x, int y, uint32_t rect_color, uint32_t rect_transparency, uint32_t color)
{
	const auto pos = modules::MotionPrediction::Instance().drawText(str, x, y, D2DrawFn::RectangledText);
	if (!modules::HDText::Instance().drawRectangledText(str, pos.x, pos.y, rect_color, rect_transparency, color))
		drawRectangledText(str, pos.x, pos.y, rect_color, rect_transparency, color);
}

uint32_t __fastcall getNormalTextWidthHooked(const wchar_t* str)
{
	if (modules::HDText::Instance().isActive())
		return modules::HDText::Instance().getNormalTextWidth(str, 0);
	return getNormalTextWidth(str);
}

uint32_t __fastcall getNormalTextNWidthHooked(const wchar_t* str, const int n_chars)
{
	if (modules::HDText::Instance().isActive())
		return modules::HDText::Instance().getNormalTextWidth(str, n_chars);
	return getNormalTextNWidth(str, n_chars);
}

uint32_t __fastcall getFramedTextSizeHooked(const wchar_t* str, uint32_t* width, uint32_t* height)
{
	if (modules::HDText::Instance().isActive())
		return modules::HDText::Instance().getFramedTextSize(str, width, height);
	return getFramedTextSize(str, width, height);
}

uint16_t __fastcall getFontHeightHooked()
{
	if (modules::HDText::Instance().isActive())
		return modules::HDText::Instance().getFontHeight();
	return getFontHeight();
}

uint32_t __fastcall setTextSizeHooked(uint32_t size)
{
	modules::HDText::Instance().setTextSize(size);
	size = (size > 13) ? 11 : size;
	return setTextSize(size);
}

void rectangledTextBegin()
{
	modules::MotionPrediction::Instance().textMotion(D2DrawFn::NormalText);
	modules::HDText::Instance().borderedRect(true);
}

void rectangledTextEnd()
{
	modules::MotionPrediction::Instance().textMotion(D2DrawFn::None);
	modules::HDText::Instance().borderedRect(false);
}

void unitHoverText()
{
	modules::MotionPrediction::Instance().unitHoverTextMotion();
}

void altItemsText()
{
	modules::MotionPrediction::Instance().altItemsTextMotion();
}

void drawRectFrame()
{
	modules::HDText::Instance().drawRectFrame();
}

void loadUIImage()
{
	modules::HDText::Instance().loadUIImage();
}

void drawSubTextA()
{
	modules::HDText::Instance().drawSubText(1);
}

void drawSubTextB()
{
	modules::HDText::Instance().drawSubText(2);
}

void drawSubTextC()
{
	modules::HDText::Instance().drawSubText(3);
}

void levelEntryText()
{
	modules::HDText::Instance().startEntryText();
}

}