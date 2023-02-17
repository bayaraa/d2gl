#pragma once

#include "structs.h"

namespace d2gl::d2 {

uint32_t mpqLoad(const char* mpqfile);
glm::ivec2 getCursorPos();
bool isPerspective();
bool isEscMenuOpen();

UnitAny* getPlayerUnit();
UnitAny* findUnit(uint32_t type_id);
DWORD getUnitID(UnitAny* unit);
DWORD getUnitFlag(UnitAny* unit);
Path* getUnitPath(UnitAny* unit);
StaticPath* getUnitStaticPath(UnitAny* unit);
uint32_t getUnitStat(UnitAny* unit, uint32_t stat);
UnitAny* getSelectedItem();
bool isUnitDead(UnitAny* unit);
MonsterType getMonsterType(UnitAny* unit);
wchar_t* getMonsterName(UnitAny* unit);

CellFile* getCellFile(CellContext* cell);
DWORD getCellNo(CellContext* cell);

void gameDrawBegin();
void automapDrawBegin();
void automapDrawEnd();
void uiDrawBegin();
void uiDrawEnd();

void __stdcall drawImageHooked(CellContext* cell, int x, int y, uint32_t gamma, int draw_mode, uint8_t* palette);
void __stdcall drawPerspectiveImageHooked(CellContext* cell, int x, int y, uint32_t gamma, int draw_mode, int screen_mode, uint8_t* palette);
void __stdcall drawShiftedImageHooked(CellContext* cell, int x, int y, uint32_t gamma, int draw_mode, int global_palette_shift);
void __stdcall drawVerticalCropImageHooked(CellContext* cell, int x, int y, int skip_lines, int draw_lines, int draw_mode);
void __stdcall drawClippedImageHooked(CellContext* cell, int x, int y, void* crop_rect, int draw_mode);
void __stdcall drawImageFastHooked(CellContext* cell, int x, int y, uint8_t palette_index);
void __stdcall drawShadowHooked(CellContext* cell, int x, int y);
void __stdcall drawSolidRectExHooked(int left, int top, int right, int bottom, uint32_t color, int draw_mode);
void __stdcall drawLineHooked(int x_start, int y_start, int x_end, int y_end, uint8_t color, uint8_t alpha);
bool __stdcall drawGroundTileHooked(TileContext* tile, GFXLight* light, int x, int y, int world_x, int world_y, uint8_t alpha, int screen_panels, bool tile_data);
bool __stdcall drawWallTileHooked(TileContext* tile, int x, int y, GFXLight* light, int screen_panels);
bool __stdcall drawTransWallTileHooked(TileContext* tile, int x, int y, GFXLight* light, int screen_panels, uint8_t alpha);
bool __stdcall drawShadowTileHooked(TileContext* tile, int x, int y, int draw_mode, int screen_panels);

void __fastcall takeScreenShotHooked();
void __fastcall drawNormalTextHooked(const wchar_t* str, int x, int y, uint32_t color, uint32_t centered);
void __fastcall drawNormalTextExHooked(const wchar_t* str, int x, int y, uint32_t color, uint32_t centered, uint32_t trans_lvl);
void __fastcall drawFramedTextHooked(const wchar_t* str, int x, int y, uint32_t color, uint32_t centered);
void __fastcall drawRectangledTextHooked(const wchar_t* str, int x, int y, uint32_t rect_color, uint32_t rect_transparency, uint32_t color);
uint32_t __fastcall getNormalTextWidthHooked(const wchar_t* str);
uint32_t __fastcall getNormalTextNWidthHooked(const wchar_t* str, const int n_chars);
uint32_t __fastcall getFramedTextSizeHooked(const wchar_t* str, uint32_t* width, uint32_t* height);
uint16_t __fastcall getFontHeightHooked();
uint32_t __fastcall setTextSizeHooked(uint32_t dwSize);

void rectangledTextBegin();
void rectangledTextEnd();
void unitHoverText();
void altItemsText();
// void DrawRectFrameBegin();

void loadUIImage();
void drawSubTextA();
void drawSubTextB();
void drawSubTextC();
}