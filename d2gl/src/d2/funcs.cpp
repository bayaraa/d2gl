#include "pch.h"
#include "funcs.h"
#include "common.h"
#include "helpers.h"
#include "modules/motion_prediction.h"
#include "stubs.h"

namespace d2gl::d2 {

uint32_t mpqLoad(const char* mpqfile)
{
	if (isVer(V_114d))
		return ((mpqLoad_114_t)mpqLoad_Fn)(mpqfile, 0, 0, 0);
	else if (isVerMin(V_111a))
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
	const bool avaiable = *(bool*)(perspective + 4);
	return *perspective && avaiable;
}

UnitAny* getPlayerUnit()
{
	return (UnitAny*)*(uintptr_t*)player_unit;
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

UnitAny* getHoveredInvItem()
{
	return (UnitAny*)*(uintptr_t*)hovered_inv_item;
}

CellFile* getCellFile(CellContext* cell)
{
	if (isVerMin(V_113c))
		return cell->v113.pCellFile;
	else if (isVer(V_112a))
		return cell->v112.pCellFile;
	else if (isVerMin(V_111a))
		return cell->v111.pCellFile;
	return cell->v109.pCellFile;
}

DWORD getCellNo(CellContext* cell)
{
	if (isVerMin(V_113c))
		return cell->v113.nCellNo;
	else if (isVer(V_112a))
		return cell->v112.nCellNo;
	else if (isVerMin(V_111a))
		return cell->v111.nCellNo;
	return cell->v109.nCellNo;
}

void gameDrawBegin()
{
	if (App.game.screen == GameScreen::Loading || App.game.screen == GameScreen::Menu) {
		App.game.screen = GameScreen::InGame;
		if (App.game.onStageChange)
			App.game.onStageChange();
	}
}

void automapDrawBegin()
{
	App.game.draw_stage = DrawStage::Map;
	if (App.game.onStageChange)
		App.game.onStageChange();
}

void automapDrawEnd()
{
	App.game.draw_stage = DrawStage::HUD;
	if (App.game.onStageChange)
		App.game.onStageChange();
}

void uiDrawBegin()
{
	App.game.draw_stage = DrawStage::UI;
	if (App.game.onStageChange)
		App.game.onStageChange();
}

void uiDrawEnd()
{
	App.game.draw_stage = DrawStage::Cursor;
	if (App.game.onStageChange)
		App.game.onStageChange();
}

void __stdcall drawImageHooked(CellContext* cell, int x, int y, uint32_t gamma, int draw_mode, uint8_t* palette)
{
	// if (modules::HDText::Instance().DrawImage(pData, nXpos, nYpos, dwGamma, nDrawMode)) {
	const auto pos = modules::MotionPrediction::Instance().drawImage(x, y, D2DrawFn::Image, gamma, draw_mode);
	drawImage(cell, pos.x, pos.y, gamma, draw_mode, palette);
	//}
}

void __stdcall drawShiftedImageHooked(CellContext* cell, int x, int y, uint32_t gamma, int draw_mode, int global_palette_shift)
{
	// if (modules::HDText::Instance().DrawShiftedImage(pData, nXpos, nYpos, dwGamma, nDrawMode, nGlobalPaletteShift)) {
	auto pos = modules::MotionPrediction::Instance().drawImage(x, y, D2DrawFn::ShiftedImage);
	drawShiftedImage(cell, pos.x, pos.y, gamma, draw_mode, global_palette_shift);
	// }
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
	// const auto offset = modules::MotionPrediction::Instance().drawText(D2DrawFn::SolidRectEx);
	const auto offset = modules::MotionPrediction::Instance().drawSolidRect();
	//  if (!modules::HDText::Instance().DrawSolidRect(nXStart + offset.x, nYStart + offset.y, nXEnd + offset.x, nYEnd + offset.y, dwColor, nDrawMode))
	drawSolidRectEx(left - offset.x, top - offset.y, right - offset.x, bottom - offset.y, color, draw_mode);
}

void __stdcall drawLineHooked(int x_start, int y_start, int x_end, int y_end, uint8_t color, uint8_t alpha)
{
	const auto offset = modules::MotionPrediction::Instance().drawLine(x_start, y_start);
	drawLine(x_start - offset.x, y_start - offset.y, x_end - offset.x, y_end - offset.y, color, alpha);
}

bool __stdcall drawGroundTileHooked(TileContext* tile, GFXLight* light, int x, int y, int world_x, int world_y, uint8_t alpha, int screen_panels, bool tile_data)
{
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

void __fastcall drawNormalTextHooked(const wchar_t* str, int x, int y, uint32_t color, uint32_t centered)
{
	const auto pos = modules::MotionPrediction::Instance().drawText(str, x, y, D2DrawFn::NormalText);
	//  if (!modules::HDText::Instance().DrawText(wStr, pos.x, pos.y, dwColor, centered))
	drawNormalText(str, pos.x, pos.y, color, centered);
}

void __fastcall drawNormalTextExHooked(const wchar_t* str, int x, int y, uint32_t color, uint32_t centered, uint32_t trans_lvl)
{
	const auto pos = modules::MotionPrediction::Instance().drawText(str, x, y, D2DrawFn::NormalTextEx);
	//  if (!modules::HDText::Instance().DrawText(wStr, pos.x, pos.y, dwColor, centered))
	drawNormalTextEx(str, pos.x, pos.y, color, centered, trans_lvl);
}

void __fastcall drawFramedTextHooked(const wchar_t* str, int x, int y, uint32_t color, uint32_t centered)
{
	const auto pos = modules::MotionPrediction::Instance().drawText(str, x, y, D2DrawFn::FramedText);
	//  if (!modules::HDText::Instance().DrawFramedText(wStr, pos.x, pos.y, dwColor, centered))
	drawFramedText(str, pos.x, pos.y, color, centered);
}

void __fastcall drawRectangledTextHooked(const wchar_t* str, int x, int y, uint32_t rect_color, uint32_t rect_transparency, uint32_t color)
{
	const auto pos = modules::MotionPrediction::Instance().drawText(str, x, y, D2DrawFn::RectangledText);
	//  if (!modules::HDText::Instance().DrawRectangledText(wStr, pos.x, pos.y, rectColor, rectTransparency, dwColor))
	drawRectangledText(str, pos.x, pos.y, rect_color, rect_transparency, color);
}

uint32_t __fastcall getNormalTextWidthHooked(const wchar_t* str)
{
	// if (modules::HDText::Instance().IsActive())
	//	return modules::HDText::Instance().GetNormalTextWidth(wStr, 0);
	return getNormalTextWidth(str);
}

uint32_t __fastcall getNormalTextNWidthHooked(const wchar_t* str, const int n_chars)
{
	// if (modules::HDText::Instance().IsActive())
	//	return modules::HDText::Instance().GetNormalTextWidth(wStr, nChars);
	return getNormalTextNWidth(str, n_chars);
}

uint32_t __fastcall getFramedTextSizeHooked(const wchar_t* str, uint32_t* width, uint32_t* height)
{
	// if (modules::HDText::Instance().IsActive())
	//	return modules::HDText::Instance().GetFramedTextSize(wStr, dwWidth, dwHeight);
	return getFramedTextSize(str, width, height);
}

uint16_t __fastcall getFontHeightHooked()
{
	// if (modules::HDText::Instance().IsActive())
	//	return modules::HDText::Instance().GetFontHeight();
	return getFontHeight();
}

uint32_t __fastcall setTextSizeHooked(uint32_t size)
{
	// modules::HDText::Instance().SetTextSize(dwSize);
	return setTextSize(size);
}

void rectangledTextBegin()
{
	modules::MotionPrediction::Instance().textMotion(D2DrawFn::NormalText);
	//  modules::HDText::Instance().BorderedRect(true);
}

void rectangledTextEnd()
{
	modules::MotionPrediction::Instance().textMotion(D2DrawFn::None);
	//  modules::HDText::Instance().BorderedRect(false);
}

void unitHoverText()
{
	modules::MotionPrediction::Instance().unitHoverTextMotion();
}

void altItemsText()
{
	modules::MotionPrediction::Instance().altItemsTextMotion();
}

// void DrawRectFrameBegin()
//{
//	if (CurrentDrawingRect)
//	{
//		modules::MotionPrediction::Instance().DrawFrameBegin(CurrentDrawingRect);
//		CurrentDrawingRect->left = -10;
//		CurrentDrawingRect->top = -10;
//		CurrentDrawingRect->right = -10;
//		CurrentDrawingRect->bottom = -10;
//	}
// }

void loadUIImage()
{
	// modules::HDText::Instance().LoadUIImage();
}

void drawSubTextA()
{
	// modules::HDText::Instance().DrawSubText(1);
}

void drawSubTextB()
{
	// modules::HDText::Instance().DrawSubText(2);
}

void drawSubTextC()
{
	// modules::HDText::Instance().DrawSubText(3);
}

}