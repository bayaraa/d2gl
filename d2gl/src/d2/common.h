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

#include "funcs.h"
#include "patch.h"
#include "structs.h"

namespace d2gl::d2 {

typedef uint32_t(__fastcall* mpqLoad_114_t)(const char* mpq_file, char* mpq_name, int, int);
typedef uint32_t(__fastcall* mpqLoad_110_t)(const char* dll, const char* mpq_file, char* mpq_name, int, int, int, int);
typedef uint32_t(__fastcall* mpqOpenFile_t)(char* path, void** buffer);
typedef uint32_t(__fastcall* mpqCloseFile_t)(void* mpq_file);
typedef uint32_t(__fastcall* mpqReadFile_t)(void* mpq_file, uint8_t* buffer, uint32_t nb_to_read, uint32_t* nb_read, int, int, int);
typedef uint32_t(__fastcall* mpqGetFileSize_t)(void* mpq_file, uintptr_t* to_reset);

typedef UnitAny*(__fastcall* findUnit_t)(uint32_t id, uint32_t type);
typedef uint32_t(__stdcall* drawUnit_t)(UnitAny* unit, uint32_t, uint32_t, uint32_t, uint32_t);
typedef void* drawWeatherParticles_t;
typedef UnitAny*(__stdcall* getSelectedUnit_t)(void);

typedef uint32_t(__stdcall* getLangId_t)(void);
typedef uint32_t(__stdcall* getUnitStat_t)(UnitAny* unit, uint32_t stat, uint32_t stat2);
typedef uint32_t(__stdcall* getUnitStat109d_t)(UnitAny* unit, uint32_t stat);
typedef uint32_t(__stdcall* getUnitState_t)(UnitAny* unit, uint32_t state_no);
typedef Room1*(__stdcall* getUnitRoom_t)(UnitAny* unit);
typedef int(__stdcall* getLevelNoByRoom_t)(Room1* room);

typedef void(__stdcall* clearScreen_t)(bool partial);
typedef void(__stdcall* drawImage_t)(CellContext* cell, int x, int y, uint32_t gamma, int draw_mode, uint8_t* palette);
typedef void(__stdcall* drawPerspectiveImage_t)(CellContext* cell, int x, int y, uint32_t gamma, int draw_mode, int screen_mode, uint8_t* palette);
typedef void(__stdcall* drawShiftedImage_t)(CellContext* cell, int x, int y, uint32_t gamma, int draw_mode, int global_palette_shift);
typedef void(__stdcall* drawVerticalCropImage_t)(CellContext* cell, int x, int y, int skip_lines, int draw_lines, int draw_mode);
typedef void(__stdcall* drawClippedImage_t)(CellContext* cell, int x, int y, void* crop_rect, int draw_mode);
typedef void(__stdcall* drawImageFast_t)(CellContext* cell, int x, int y, uint8_t palette_index);
typedef void(__stdcall* drawShadow_t)(CellContext* cell, int x, int y);
typedef void(__stdcall* drawSolidRectEx_t)(int left, int top, int right, int bottom, uint32_t color, int draw_mode);
typedef void(__stdcall* drawLine_t)(int x_start, int y_start, int x_end, int y_end, uint8_t color, uint8_t alpha);
typedef bool(__stdcall* drawGroundTile_t)(TileContext* tile, GFXLight* light, int x, int y, int world_x, int world_y, uint8_t alpha, int screen_panels, bool tile_data);
typedef bool(__stdcall* drawWallTile_t)(TileContext* tile, int x, int y, GFXLight* light, int screen_panels);
typedef bool(__stdcall* drawTransWallTile_t)(TileContext* tile, int x, int y, GFXLight* light, int screen_panels, uint8_t alpha);
typedef bool(__stdcall* drawShadowTile_t)(TileContext* tile, int x, int y, int draw_mode, int screen_panels);

typedef void(__fastcall* takeScreenShot_t)(void);
typedef void(__fastcall* drawNormalText_t)(const wchar_t* str, int x, int y, uint32_t color, uint32_t centered);
typedef void(__fastcall* drawNormalTextEx_t)(const wchar_t* str, int x, int y, uint32_t color, uint32_t centered, uint32_t trans_lvl);
typedef void(__fastcall* drawFramedText_t)(const wchar_t* str, int x, int y, uint32_t color, uint32_t centered);
typedef void(__fastcall* drawRectangledText_t)(const wchar_t* str, int x, int y, uint32_t rect_color, uint32_t rect_transparency, uint32_t color);
typedef uint32_t(__fastcall* getNormalTextWidth_t)(const wchar_t* str);
typedef uint32_t(__fastcall* getNormalTextNWidth_t)(const wchar_t* str, const int n_chars);
typedef uint32_t(__fastcall* getFramedTextSize_t)(const wchar_t* str, uint32_t* width, uint32_t* height);
typedef uint16_t(__fastcall* getFontHeight_t)(void);
typedef uint32_t(__fastcall* setTextSize_t)(uint32_t size);

extern uint32_t* screen_width;
extern uint32_t* screen_height;
extern uint32_t* screen_shift;

extern int* video_mode;
extern bool* perspective;
extern bool* esc_menu_open;

extern uint32_t* is_in_game;
extern UnitAny* player_unit;
extern UnitAny* selected_item;
extern int* level_no;

extern void* alt_item_pos;
extern void* sub_text_ptr;
extern const char* ui_image_path;

extern int* mouse_x;
extern int* mouse_y;
extern int* mouse_x_menu;
extern int* mouse_y_menu;
extern uint8_t* cursor_state1;
extern uint8_t* cursor_state2;
extern uint8_t* cursor_state3;
extern int* shake_x;
extern int* shake_y;
extern int* hover_text_x;
extern int* hover_text_y;

extern uint32_t* no_pickup;

extern uintptr_t mpqLoad_Fn;
extern mpqOpenFile_t mpqOpenFile;
extern mpqCloseFile_t mpqCloseFile;
extern mpqReadFile_t mpqReadFile;
extern mpqGetFileSize_t mpqGetFileSize;

extern uintptr_t d2SoundFn1_O;

extern uint32_t* automap_on;
extern int* automap_type;
extern uintptr_t d2ClientFn1_O;
extern uintptr_t d2ClientFn2_O;

extern findUnit_t findUnitClient;
extern findUnit_t findUnitServer;
extern drawUnit_t drawUnit;
extern drawUnit_t drawMissile;
extern drawWeatherParticles_t drawWeatherParticles;

extern clearScreen_t clearScreen;
extern drawImage_t drawImage;
extern drawPerspectiveImage_t drawPerspectiveImage;
extern drawShiftedImage_t drawShiftedImage;
extern drawVerticalCropImage_t drawVerticalCropImage;
extern drawClippedImage_t drawClippedImage;
extern drawImageFast_t drawImageFast;
extern drawShadow_t drawShadow;
extern drawSolidRectEx_t drawSolidRectEx;
extern drawLine_t drawLine;
extern drawGroundTile_t drawGroundTile;
extern drawWallTile_t drawWallTile;
extern drawTransWallTile_t drawTransWallTile;
extern drawShadowTile_t drawShadowTile;

extern takeScreenShot_t takeScreenShot;
extern drawNormalText_t drawNormalText;
extern drawNormalTextEx_t drawNormalTextEx;
extern drawFramedText_t drawFramedText;
extern drawRectangledText_t drawRectangledText;
extern getNormalTextWidth_t getNormalTextWidth;
extern getNormalTextNWidth_t getNormalTextNWidth;
extern getFramedTextSize_t getFramedTextSize;
extern getFontHeight_t getFontHeight;
extern setTextSize_t setTextSize;

extern getLangId_t getLangId;
extern getSelectedUnit_t getSelectedUnit;
extern getUnitStat_t getUnitStat_Fn;
extern getUnitState_t getUnitState_Fn;
extern getUnitRoom_t getUnitRoom;
extern getLevelNoByRoom_t getLevelNoByRoom;
extern uintptr_t getLevelName_Fn;

extern UnitAny* currently_drawing_unit;
extern uint32_t currently_drawing_weather_particles;
extern uintptr_t* currently_drawing_weather_particle_index_ptr;
extern RECT* currently_drawing_rect;

extern std::unique_ptr<Patch> patch_minimap;
extern std::unique_ptr<Patch> patch_motion_prediction;
extern std::unique_ptr<Patch> patch_hd_text;

void initHooks();
void destroyHooks();

}