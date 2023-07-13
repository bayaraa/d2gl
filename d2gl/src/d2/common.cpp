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
#include "common.h"
#include "funcs.h"
#include "helpers.h"
#include "modules/motion_prediction.h"
#include "stubs.h"

#include <detours/detours.h>

namespace d2gl::d2 {

uint32_t* screen_width = (uint32_t*)getProc((DLL_D2CLIENT), (0xD40E4), (0xD40F4), (0xF5C5C), (0xF4FC4), (0xDC6E0), (0xDBC48), (0xF7034), (0x31146C));
uint32_t* screen_height = (uint32_t*)getProc((DLL_D2CLIENT), (0xD40E0), (0xD40F0), (0xF5C60), (0xF4FC8), (0xDC6E4), (0xDBC4C), (0xF7038), (0x311470));
uint32_t* screen_shift = (uint32_t*)getProc((DLL_D2CLIENT), (0x115C10), (0x10B9C4), (0x11C1C0), (0x11C3E4), (0x11C1D0), (0x11C414), (0x11D070), (0x3A5210));

int* video_mode = (int*)getProc((DLL_D2GFX), (0x1D208), (0x1D264), (0x1A220), (0x14884), (0x1D44C), (0x11258), (0x14A38), (0x3C8CB0));
bool* perspective = (bool*)getProc((DLL_D2GFX), (0xE188), (0xE198), (0x10C94), (0x10C30), (0x10C8C), (0x10BE0), (0x10BE4), (0x32DA48));
bool* esc_menu_open = (bool*)getProc((DLL_D2CLIENT), (0x1248D8), (0x11A6CC), (0xFB094), (0x1040E4), (0x102B7C), (0xFADA4), (0x11C8B4), (0x3A27E4));
bool* is_alt_clicked = (bool*)getProc((DLL_D2CLIENT), (0x1248E8), (0x11A6DC), (0xFB0A4), (0x1040F4), (0x102B8C), (0xFADB4), (0x11C8C4), (0x3A27F4));
int is_unit_hovered = 0;

uint32_t* is_in_game = (uint32_t*)getProc((DLL_D2CLIENT), (0x1109FC), (0x1077C4), (0xE48EC), (0xF18C0), (0x11BCC4), (0xF8C9C), (0xF79E0), (0x3A27C0));
UnitAny* player_unit = (UnitAny*)getProc((DLL_D2CLIENT), (0x1263F8), (0x11C200), (0x11C4F0), (0x11C1E0), (0x11C3D0), (0x11BBFC), (0x11D050), (0x3A6A70));
UnitAny* selected_item = (UnitAny*)getProc((DLL_D2CLIENT), (0x11FBB8), (0x1158F0), (0x11BA40), (0x11BBDC), (0x11B9FC), (0x11BC38), (0x11CB28), (0x3BCBF4));
int* level_no = (int*)getProc((DLL_D2CLIENT), (0x1201B4), (0x115EF0), (0x11C17C), (0x11B99C), (0x11BCEC), (0x11C310), (0x11CDE8), (0x3A3140));

void* alt_item_pos = nullptr;
void* sub_text_ptr = nullptr;
const char* ui_image_path = nullptr;

int* mouse_x = (int*)getProc((DLL_D2CLIENT), (0x12B168), (0x121AE4), (0x10A410), (0x11B418), (0x101638), (0x11B828), (0x11C950), (0x3A6AB0));
int* mouse_y = (int*)getProc((DLL_D2CLIENT), (0x12B16C), (0x121AE8), (0x10A40C), (0x11B414), (0x101634), (0x11B824), (0x11C94C), (0x3A6AAC));
int* mouse_x_menu = (int*)getProc((DLL_D2WIN), (0x618A0), (0x5E234), (0xC8C5C), (0x20470), (0x5C700), (0x21488), (0x8DB1C), (0x3D55A4));
int* mouse_y_menu = (int*)getProc((DLL_D2WIN), (0x618A4), (0x5E238), (0xC8C60), (0x20474), (0x5C704), (0x2148C), (0x8DB20), (0x3D55A8));
uint8_t* cursor_state1 = (uint8_t*)getProc((DLL_D2CLIENT), (0x12B158), (0x121AD4), (0x10A44C), (0x11B454), (0x101674), (0x11B864), (0x11C98C), (0x3A6AF0));
uint8_t* cursor_state2 = (uint8_t*)getProc((DLL_D2CLIENT), (0x12B148), (0x121AC4), (0x10A43C), (0x11B444), (0x101664), (0x11B854), (0x11C97C), (0x3A6ADC));
uint8_t* cursor_state3 = (uint8_t*)getProc((DLL_D2CLIENT), (0x11FBB4), (0x1158EC), (0x11BA3C), (0x11BBD8), (0x11B9F8), (0x11BC34), (0x11CB24), (0x3BCBF0));
int* shake_x = (int*)getProc((DLL_D2CLIENT), (0x119B0C), (0x10F8C8), (0x11C204), (0x11BAFC), (0x11B9B4), (0x11BF00), (0x11CA6C), (0x3B9538));
int* shake_y = (int*)getProc((DLL_D2CLIENT), (0x116454), (0x10C20C), (0x10027C), (0xFD944), (0xFD114), (0x10B9DC), (0xFC3DC), (0x3B8D20));

uint32_t* no_pickup = (uint32_t*)getProc((DLL_D2CLIENT), (), (0x10B9E4), (0x11C43C), (0x11C32C), (0x11C114), (0x11C2F0), (0x11D574), (0x3A6A90));

int* hover_text_x = (int*)getProc((DLL_D2CLIENT), (0xEFB60), (0xEEA28), (0xDD174), (0xED8D4), (0xDD9EC), (0xDBC54), (0xF7F98), (0x30F854));
int* hover_text_y = (int*)getProc((DLL_D2CLIENT), (0xEFB64), (0xEEA2C), (0xDD178), (0xED8D8), (0xDD9F0), (0xDBC58), (0xF7F9C), (0x30F858));

uintptr_t mpqLoad_Fn = getProc((DLL_D2WIN), (0x142FB), (0x12399), (0x7D80), (0x7D80), (0x7E40), (0x7E60), (0x7E50), (0x117332));
mpqOpenFile_t mpqOpenFile = (mpqOpenFile_t)getProc((DLL_FOG), (-10102), (-10102), (-10102), (-10102), (-10102), (-10102), (-10102), (0x68E0));
mpqCloseFile_t mpqCloseFile = (mpqCloseFile_t)getProc((DLL_FOG), (-10103), (-10103), (-10103), (-10103), (-10103), (-10103), (-10103), (0x68F0));
mpqReadFile_t mpqReadFile = (mpqReadFile_t)getProc((DLL_FOG), (-10104), (-10104), (-10104), (-10104), (-10104), (-10104), (-10104), (0x6900));
mpqGetFileSize_t mpqGetFileSize = (mpqGetFileSize_t)getProc((DLL_FOG), (-10105), (-10105), (-10105), (-10105), (-10105), (-10105), (-10105), (0x6930));

uintptr_t d2SoundFn1_O = getProc((DLL_D2SOUND), (-10032), (-10032), (-10035), (-10049), (-10050), (-10057), (-10015), (0x114840));

uint32_t* automap_on = (uint32_t*)getProc((DLL_D2CLIENT), (0x1248DC), (0x11A6D0), (0xFB098), (0x1040E8), (0x102B80), (0xFADA8), (0x11C8B8), (0x3A27E8));
int* automap_type = (int*)getProc((DLL_D2CLIENT), (0xEB01C), (0xEABDC), (0xF1694), (0xEA77C), (0xF1654), (0xEF66C), (0xE99D4), (0x31FD9C));
uintptr_t d2ClientFn1_O = getProc((DLL_D2CLIENT), (0x8CFC0), (0x883D0), (0x57E20), (0x50A90), (0x3E530), (0x5F9F0), (0x71390), (0x57520));
uintptr_t d2ClientFn2_O = getProc((DLL_D2CLIENT), (0x8DC20), (0x89370), (0x9E990), (0x62670), (0x8BDA0), (0xBF0D0), (0x18E20), (0x55510));

findUnit_t findUnitClient = (findUnit_t)getProc((DLL_D2CLIENT), (0x8B560), (0x869F0), (0x262D0), (0x4C040), (0x1F1A0), (0xA5B20), (0x620B0), (0x63990));
findUnit_t findUnitServer = (findUnit_t)getProc((DLL_D2CLIENT), (0x8B5D0), (0x86A60), (0x262F0), (0x4C060), (0x1F1C0), (0xA5B40), (0x620D0), (0x639B0));
drawUnit_t drawUnit = (drawUnit_t)getProc((DLL_D2CLIENT), (0xB8350), (0xBA720), (0x6C760), (0x478D0), (0x94250), (0x6C490), (0x605b0), (0x70EC0));
drawUnit_t drawMissile = (drawUnit_t)getProc((DLL_D2CLIENT), (), (), (0x6CEB0), (0x480A0), (0x949C0), (0x6CC00), (0x60C70), (0x71EC0));
drawWeatherParticles_t drawWeatherParticles = (drawWeatherParticles_t)getProc((DLL_D2CLIENT), (0x07BC0), (0x08240), (0x4C980), (0x12730), (0x14210), (0x7FE80), (0x4AD90), (0x73470));

clearScreen_t clearScreen = (clearScreen_t)getProc((DLL_D2GFX), (-10058), (-10058), (-10067), (-10037), (-10047), (-10021), (-10010), (0xF63B0));
drawImage_t drawImage = (drawImage_t)getProc((DLL_D2GFX), (-10072), (-10072), (-10047), (-10044), (-10024), (-10041), (-10042), (0xF6480));
drawPerspectiveImage_t drawPerspectiveImage = (drawPerspectiveImage_t)getProc((DLL_D2GFX), (-10071), (-10071), (-10082), (-10047), (-10063), (-10016), (-10009), (0xF6450));
drawShiftedImage_t drawShiftedImage = (drawShiftedImage_t)getProc((DLL_D2GFX), (-10073), (-10073), (-10079), (-10068), (-10044), (-10019), (-10067), (0xF64B0));
drawVerticalCropImage_t drawVerticalCropImage = (drawVerticalCropImage_t)getProc((DLL_D2GFX), (-10074), (-10074), (-10005), (-10009), (-10046), (-10074), (-10082), (0xF64E0));
drawClippedImage_t drawClippedImage = (drawClippedImage_t)getProc((DLL_D2GFX), (-10077), (-10077), (-10023), (-10005), (-10061), (-10079), (-10015), (0xF6510));
drawImageFast_t drawImageFast = (drawImageFast_t)getProc((DLL_D2GFX), (-10076), (-10076), (-10017), (-10075), (-10012), (-10046), (-10006), (0xF6570));
drawShadow_t drawShadow = (drawShadow_t)getProc((DLL_D2GFX), (-10075), (-10075), (-10040), (-10032), (-10030), (-10011), (-10084), (0xF6540));
drawSolidRectEx_t drawSolidRectEx = (drawSolidRectEx_t)getProc((DLL_D2GFX), (-10055), (-10055), (-10028), (-10000), (-10062), (-10014), (-10028), (0xF6300));
drawLine_t drawLine = (drawLine_t)getProc((DLL_D2GFX), (-10057), (-10057), (-10009), (-10001), (-10031), (-10010), (-10013), (0xF6380));
drawGroundTile_t drawGroundTile = (drawGroundTile_t)getProc((DLL_D2GFX), (-10079), (-10079), (-10080), (-10082), (-10038), (-10076), (-10001), (0xF68E0));
drawWallTile_t drawWallTile = (drawWallTile_t)getProc((DLL_D2GFX), (-10080), (-10080), (-10065), (-10011), (-10014), (-10001), (-10081), (0xF6920));
drawTransWallTile_t drawTransWallTile = (drawTransWallTile_t)getProc((DLL_D2GFX), (-10081), (-10081), (-10035), (-10076), (-10075), (-10058), (-10040), (0xF6950));
drawShadowTile_t drawShadowTile = (drawShadowTile_t)getProc((DLL_D2GFX), (-10082), (-10082), (-10012), (-10043), (-10042), (-10069), (-10060), (0xF6980));

takeScreenShot_t takeScreenShot = (takeScreenShot_t)getProc((DLL_D2WIN), (-10168), (-10168), (-10205), (-10014), (-10196), (-10187), (-10107), (0xFA7A0));
drawNormalText_t drawNormalText = (drawNormalText_t)getProc((DLL_D2WIN), (-10117), (-10117), (-10020), (-10064), (-10001), (-10150), (-10076), (0x102320));
drawNormalTextEx_t drawNormalTextEx = (drawNormalTextEx_t)getProc((DLL_D2WIN), (-10118), (-10118), (-10006), (-10043), (-10157), (-10096), (-10084), (0x102360));
drawFramedText_t drawFramedText = (drawFramedText_t)getProc((DLL_D2WIN), (-10129), (-10129), (-10118), (-10039), (-10031), (-10085), (-10137), (0x102280));
drawRectangledText_t drawRectangledText = (drawRectangledText_t)getProc((DLL_D2WIN), (-10132), (-10132), (-10009), (-10103), (-10082), (-10013), (-10078), (0x1023B0));
getNormalTextWidth_t getNormalTextWidth = (getNormalTextWidth_t)getProc((DLL_D2WIN), (-10121), (-10121), (-10034), (-10128), (-10132), (-10028), (-10150), (0x101820));
getNormalTextNWidth_t getNormalTextNWidth = (getNormalTextNWidth_t)getProc((DLL_D2WIN), (-10122), (-10122), (-10015), (-10111), (-10041), (-10055), (-10148), (0x1017D0));
getFramedTextSize_t getFramedTextSize = (getFramedTextSize_t)getProc((DLL_D2WIN), (-10131), (-10131), (-10057), (-10183), (-10096), (-10177), (-10179), (0x102520));
getFontHeight_t getFontHeight = (getFontHeight_t)getProc((DLL_D2WIN), (-10125), (-10125), (-10000), (-10138), (-10146), (-10083), (-10088), (0x101A40));
setTextSize_t setTextSize = (setTextSize_t)getProc((DLL_D2WIN), (-10127), (-10127), (-10141), (-10170), (-10010), (-10184), (-10047), (0x102EF0));

getLangId_t getLangId = (getLangId_t)getProc((DLL_D2LANG), (-10007), (-10007), (-10009), (-10013), (-10002), (-10009), (-10001), (0x125150));
getSelectedUnit_t getSelectedUnit = (getSelectedUnit_t)getProc((DLL_D2CLIENT), (0x14CE0), (0x15A20), (0x37CA0), (0x2F950), (0x6ECA0), (0x51A80), (0x17280), (0x67A10));
getUnitStat_t getUnitStat_Fn = (getUnitStat_t)getProc((DLL_D2COMMON), (-10519), (-10519), (-11092), (-10061), (-10658), (-10973), (-10550), (0x225480));
getUnitState_t getUnitState_Fn = (getUnitState_t)getProc((DLL_D2COMMON), (-10487), (), (), (-10604), (), (-10494), (-10706), (0x239DF0));
getUnitRoom_t getUnitRoom = (getUnitRoom_t)getProc((DLL_D2COMMON), (), (), (), (), (), (), (-10846), ());
getLevelNoByRoom_t getLevelNoByRoom = (getLevelNoByRoom_t)getProc((DLL_D2COMMON), (), (), (), (), (), (), (-10691), ());
uintptr_t getLevelName_Fn = getProc((DLL_D2CLIENT), (0x88420), (0x839F0), (0x9DC10), (0x61AA0), (0x8B1A0), (0xBE240), (0x18250), (0x53E70));

// d2client + 0x291F8 : 1 2 3 4 (potion keys)
// d2client + 0x27708 : life XX/XX
// d2client + 0x277C7 : Mana XX/XX
// d2client + 0x3D88A 0x3DD72 0x3E69C 0x289C9 : keybindings

UnitAny* currently_drawing_unit = nullptr;
uint32_t currently_drawing_weather_particles = 0;
uint32_t* currently_drawing_weather_particle_index_ptr = nullptr;
RECT* currently_drawing_rect = nullptr;
UnitAny* headsup_text_unit = nullptr;

std::unique_ptr<Patch> patch_minimap;
std::unique_ptr<Patch> patch_motion_prediction;
std::unique_ptr<Patch> patch_hd_text;

void initHooks()
{
	*is_in_game = 0;

	Patch no_intro = Patch();
	no_intro.add(PatchType::Swap, getOffset((DLL_D2LAUNCH), (0x24F0C), (0x27A58), (0x1E210), (0x1E11C), (0x1E240), (0x1E0FC), (0x1E2F4), (0x2D4C84)), 1, 0x00);
	no_intro.add(PatchType::Swap, getOffset((DLL_D2LAUNCH), (0x24F34), (0x27A80), (0x1E238), (0x1E144), (0x1E268), (0x1E124), (0x1E31C), (0x2D4CAC)), 1, 0x00);
	no_intro.add(PatchType::Swap, getOffset((DLL_D2LAUNCH), (0x24F5C), (0x27AA8), (0x1E260), (0x1E16C), (0x1E290), (0x1E14C), (0x1E344), (0x2D4CD4)), 1, 0x00);
	no_intro.add(PatchType::Swap, getOffset((DLL_D2LAUNCH), (0x24F84), (0x27AD0), (0x1E288), (0x1E194), (0x1E2B8), (0x1E174), (0x1E36C), (0x2D4CFC)), 1, 0x00);
	no_intro.toggle(App.skip_intro);

	Patch multiple_instance = Patch();
	multiple_instance.add(PatchType::Swap, getOffset((DLL_D2GFX), (0x447C), (0x446A), (0x84CF), (0x84AF), (0x894F), (0x85BF), (0xB6B0), (0xF562B)), 2, isVer(V_109d) ? 0xEB47 : (isVer(V_110) ? 0xEB49 : 0xEB45));
	multiple_instance.toggle(true);

	Patch game_loop = Patch();
	game_loop.add(PatchType::Call, getOffset((DLL_D2CLIENT), (0x9B3D, 0xE87A5F0B), (0xA2A2, 0xE8B51B0C), (0x89A2F, 0xE84A37F8), (0x3356F, 0xE85E9CFD), (0x7D1BF, 0xE84801F9), (0x44E2F, 0xE81A85FC), (0x45E7F, 0xE8E473FC), (0x4F256, 0xE8E5550C)), 5, (uintptr_t)gameDrawBeginStub);
	game_loop.add(PatchType::Auto, getOffset((DLL_D2CLIENT, 0x5333DB3B), (0x865AC, 0x33ED894C), (0x81B7C, 0x33DB894C), (0xA35F6), (0x669F6), (0x90156), (0xC39E6), (0x1D3E6), (0x56EE1, 0x8BEC83EC)), isVerMax(V_110) ? 6 : 5, (uintptr_t)uiDrawBeginStub);
	game_loop.add(PatchType::Auto, getOffset((DLL_D2CLIENT, 0x2BC803CA), (0xB59A5, 0x8B442404), (0xB7B71, 0x8B442404), (0x38FD2), (0x28A82), (0x9F692), (0x16B72), (0x14FF2), (0x68578, 0xD1E82BC8)), 5, (uintptr_t)uiDrawCursorItemStub); // Cursor item
	game_loop.add(PatchType::Auto, getOffset((DLL_D2CLIENT, 0x508D5424), (0xB58CC, 0x6AFF5203), (0xB7A9C, 0x6AFF5203), (0x38461), (0x27F11), (0x9EB21), (0x16001), (0x14481), (0x684A5, 0x518D45B8)), 5, (uintptr_t)uiDrawEndStub); // Cursor changed
	game_loop.add(PatchType::Auto, getOffset((DLL_D2CLIENT, 0x52894424), (0xB5606), (0xB7776), (0x38D46), (0x287F6), (0x9F406), (0x168E6), (0x14D66), (0x68435, 0x57568D55)), 5, (uintptr_t)uiDrawEndStub); // Default cursor
	game_loop.add(PatchType::Auto, getOffset((DLL_D2WIN, 0xB9120000), (0xF4C2, 0x51895C24), (0xD922, 0x51895C24), (0xBA47), (0x133D7), (0xCDC7), (0x17F87), (0xE107), (0xF983F, 0x83EA0123)), 5, (uintptr_t)uiDrawEndStub); // InMenu
	game_loop.toggle(true);

	Patch fps_fix = Patch();
	fps_fix.add(PatchType::Nop, getOffset((DLL_D2CLIENT), (0x9B5F, 0x392D400C), (0xA2C9, 0x85C0752B), (0x89A51, 0x391D2034), (0x33591, 0x391D8036), (0x7D1E1, 0x391D2834), (0x44E51, 0x391D9034), (0x45EA1, 0x391DF046), (0x4F278, 0x391D0407)), isVer(V_110) ? 4 : 8); // InGame: Unlimited
	fps_fix.add(PatchType::Swap, getOffset((DLL_D2WIN, 0x2881FF18), (0xEC0C, 0x6A195150), (0xD02B), (0xC63B), (0x13F5B), (0xD94B), (0x18A1B), (0xED6B), (0xFA62D)), 4, isVer(V_109d) ? 0x6A2D5150 : 0x1481FF18); // InMenu: 45fps
	fps_fix.toggle(true);

	Patch sleep_fix = Patch();
	sleep_fix.add(PatchType::Nop, getOffset((DLL_D2CLIENT, 0x6A0AFF15), (0x2635, 0x6A00FF15), (0x2684), (0x8BD14), (0x5D4A4), (0x6CFD4), (0x3CB94), (0x27724), (0x51C42)), 8);
	sleep_fix.add(PatchType::Nop, getOffset((DLL_D2CLIENT, 0x6A00FFD3), (0x96C8, 0x55FFD783), (0x9E68, 0x53FFD783), (0x89288), (0x320B8), (0x7BD18), (0x43988), (0x44928), (0x4C711, 0x6A00FFD7)), isVerMax(V_110) ? 3 : 4);
	sleep_fix.add(PatchType::Nop, getOffset((DLL_D2CLIENT, 0x6A00FFD3), (0x96EC, 0x55FFD783), (0x9E8C, 0x53FFD783), (0x892AD), (0x320DD), (0x7BD3D), (0x439AD), (0x4494D), (0x4C740, 0x6A00FFD7)), isVerMax(V_110) ? 3 : 4);
	if (isVerMin(V_110) && isVerMax(V_113d))
		sleep_fix.add(PatchType::Nop, getOffset((DLL_D2WIN), (), (0xD075, 0x50FF15C0), (0xC683, 0x50FF15A0), (0x13FA3, 0x50FF15A8), (0xD993, 0x50FF15A8), (0x18A63, 0x50FF15C8), (0xEDB3, 0x50FF15B8), ()), 7);
	sleep_fix.toggle(true);

	Patch automap_loop = Patch();
	automap_loop.add(PatchType::Call, getOffset((DLL_D2CLIENT, 0xE84DDFFF), (0x27943, 0xE8785606), (0x2DCC3, 0xE808A705), (0x5AB5E, 0xE8BDD2FF), (0x52B3E), (0x405DE), (0x6269E, 0xE84DD3FF), (0x7343E), (0x5AD87, 0xE894C7FF)), 5, (uintptr_t)automapDrawBeginStub);
	automap_loop.add(PatchType::Call, getOffset((DLL_D2CLIENT), (0x867A0, 0xE87B7400), (0x81D6E, 0xE8FD7500), (0xA36B1, 0xE8DAB2FF), (0x66AB1, 0xE8BABBFF), (0x90211, 0xE88ABBFF), (0xC3AA1, 0xE82AB6FF), (0x1D4A1, 0xE87AB9FF), (0x56FAA, 0xE861E5FF)), 5, (uintptr_t)automapDrawEndStub);
	automap_loop.toggle(true);

	if (ISGLIDE3X()) {
		patch_minimap = std::make_unique<Patch>();
		patch_minimap->add(PatchType::Nop, getOffset((DLL_D2CLIENT, 0x740E833D), (0x8678F, 0x740FE87A), (0x81D5D, 0x740FE8EC), (0xA36A1), (0x66AA1), (0x90201), (0xC3A91), (0x1D491), (0x56F99, 0x740FE8F0)), 2);
		App.mini_map.available = automap_loop.isActive() && patch_minimap->prepare();
		if (App.mini_map.available)
			patch_minimap->toggle(App.mini_map.active);
		else
			App.mini_map.active = false;
	}

	Patch common_patches = Patch();
	common_patches.add(PatchType::Auto, getOffset((DLL_D2WIN, 0x85F60F9E, 0xC7), (-10124, 0, 0x150), (-10124, 0, 0x127), (-10175), (-10037), (-10201), (-10110), (-10124), (0x1030B4, 0x85DB0F9E, -1)), 5, (uintptr_t)unitHoverBeginPatch);
	common_patches.add(PatchType::Auto, getOffset((DLL_D2WIN, 0x8D140152, 0x162), (-10124, 0x03C55650, 0x1CC), (-10124, 0x03C55650, 0x1A3), (-10175), (-10037), (-10201), (-10110), (-10124), (0x10314B, 0, -1)), 5, (uintptr_t)unitHoverMidPatch);
	common_patches.add(PatchType::Auto, getOffset((DLL_D2WIN, 0xC1E81F03, 0x1A0), (-10124, 0, 0x228), (-10124, 0, 0x1FF), (-10175), (-10037), (-10201), (-10110), (-10124), (0x103188, 0, -1)), 5, (uintptr_t)unitHoverEndPatch);
	common_patches.toggle(true);

	patch_motion_prediction = std::make_unique<Patch>();
	patch_motion_prediction->add(PatchType::Auto, getOffset((DLL_D2CLIENT, 0x83EC1053), (0x7FCF0, 0x83EC5053), (0x7B4F0, 0x83EC4C53), (0x941F0), (0x15C80), (0x71990), (0x7CA40), (0x76170), (0xA0A01, 0x8BEC83EC)), 5, (uintptr_t)(isVer(V_109d) ? rectangledTextBeginStub109d : (isVer(V_110) ? rectangledTextBeginStub110f : rectangledTextBeginStub)));
	patch_motion_prediction->add(PatchType::Auto, getOffset((DLL_D2CLIENT, 0x5D5B83C4), (0x7FE50), (0x7B650), (0x9431A), (0x15DAA), (0x71ABA), (0x7CB6A), (0x7629A), (0xA0B1B, 0x5E5B8BE5)), 5, (uintptr_t)rectangledTextEndStub);
	patch_motion_prediction->add(PatchType::Auto, getOffset((DLL_D2CLIENT, 0x8B450083), (0x85629), (0x80B1E, 0x8B0683F8), (0xA05C5), (0x63E45), (0x8D595), (0xC0E25), (0x1A825), (0x5501E, 0x8B0683F8)), (isVer(V_110) || isVer(V_114d)) ? 5 : 6, (uintptr_t)unitHoverTextStub);
	if (isVer(V_109d) || isVer(V_110))
		patch_motion_prediction->add(PatchType::Call, getOffset((DLL_D2CLIENT), (0x63D95, 0x33FF8944), (0x6A19E, 0x89442430), (), (), (), (), (), ()), 6, (uintptr_t)(isVer(V_110) ? altItemsTextStub110f : altItemsTextStub109d));
	else
		patch_motion_prediction->add(PatchType::Auto, getOffset((DLL_D2CLIENT, 0xC7442450), (), (), (0xB754D), (0x5DB8D), (0x775BD), (0x5921D), (0x4E7AD), (0xC0A58, 0xC745BC01)), isVer(V_114d) ? 7 : 8, (uintptr_t)altItemsTextStub);
	modules::MotionPrediction::Instance().toggle(App.motion_prediction);

	patch_hd_text = std::make_unique<Patch>();
	patch_hd_text->add(PatchType::Auto, getOffset((DLL_D2CLIENT, 0x2D8C0000), (0x86952), (0x81EEF), (0x9EC05), (0x628E5), (0x8C015), (0xBF345), (0x19095), (0x556F2)), 5, (uintptr_t)levelEntryTextStub);
	patch_hd_text->add(PatchType::Auto, getOffset((DLL_D2CLIENT, 0x83EC5C8B), (0x86330), (0x81790), (0x9DDF0), (0x61440), (0x8AB50), (0xBE4C0), (0x17D10), (0x52E51, 0x8BEC83EC)), 5, (uintptr_t)(isVerMax(V_110) || isVer(V_114d) ? drawRectFrameStubECX : drawRectFrameStub));
	patch_hd_text->add(PatchType::Auto, getOffset((DLL_D2CLIENT, 0x8D9424FC), (0x1000, 0x81EC0801), (0x1000, 0x81EC0801), (0x75D00), (0xA9070), (0xBEF70), (0x2B420), (0xA9480), (0x788B3, 0x81EC0801)), isVer(V_109d) ? 6 : 7, (uintptr_t)(isVerMax(V_110) || isVer(V_114d) ? loadUIImageStubECX : loadUIImageStub));
	if (isVer(V_114d))
		patch_hd_text->add(PatchType::Auto, getOffset((), (), (), (), (), (), (), (), (0x101AC1, 0x8B0883C4)), 5, (uintptr_t)drawSubTextCStub); // Default texts & Credits screen
	else {
		patch_hd_text->add(PatchType::Call, getOffset((DLL_D2WIN, 0xB9120000, 0x254), (-10124, 0, 0x35A), (-10124, 0, 0x33A), (-10039), (-10031), (-10131), (-10070), (-10102), ()), 5, (uintptr_t)(isVerMax(V_110) ? drawSubTextBStub : drawSubTextAStub)); // Default texts & Credits screen
		patch_hd_text->add(PatchType::Call, getOffset((DLL_D2WIN, 0xB9120000, 0x74), (-10118, 0, 0x62), (-10118, 0, 0x62), (-10039), (-10031), (-10131), (-10070), (-10102), ()), 5, (uintptr_t)drawSubTextAStub); // Button black labels
		if (isVerMin(V_111))
			patch_hd_text->add(PatchType::Nop, getOffset((DLL_D2WIN, 0x66C70458, 0xD1), (), (), (-10134), (-10155, 0, 0x2D1), (-10201, 0, 0x271), (-10019), (-10194), ()), 6); // Get text N chars width fix
	}
	patch_hd_text->toggle(true);

	if (isVerNot(V_109d) && App.no_pickup)
		*no_pickup = 1;

	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourAttach(&(PVOID&)drawImage, drawImageHooked);
	DetourAttach(&(PVOID&)drawPerspectiveImage, drawPerspectiveImageHooked);
	DetourAttach(&(PVOID&)drawShiftedImage, drawShiftedImageHooked);
	DetourAttach(&(PVOID&)drawVerticalCropImage, drawVerticalCropImageHooked);
	DetourAttach(&(PVOID&)drawClippedImage, drawClippedImageHooked);
	DetourAttach(&(PVOID&)drawImageFast, drawImageFastHooked);
	DetourAttach(&(PVOID&)drawShadow, drawShadowHooked);
	DetourAttach(&(PVOID&)drawSolidRectEx, drawSolidRectExHooked);
	DetourAttach(&(PVOID&)drawLine, drawLineHooked);
	DetourAttach(&(PVOID&)drawGroundTile, drawGroundTileHooked);
	DetourAttach(&(PVOID&)drawWallTile, drawWallTileHooked);
	DetourAttach(&(PVOID&)drawTransWallTile, drawTransWallTileHooked);
	DetourAttach(&(PVOID&)drawShadowTile, drawShadowTileHooked);

	DetourAttach(&(PVOID&)takeScreenShot, takeScreenShotHooked);
	DetourAttach(&(PVOID&)drawNormalText, drawNormalTextHooked);
	DetourAttach(&(PVOID&)drawNormalTextEx, drawNormalTextExHooked);
	DetourAttach(&(PVOID&)drawFramedText, drawFramedTextHooked);
	DetourAttach(&(PVOID&)drawRectangledText, drawRectangledTextHooked);
	DetourAttach(&(PVOID&)getNormalTextWidth, getNormalTextWidthHooked);
	DetourAttach(&(PVOID&)getNormalTextNWidth, getNormalTextNWidthHooked);
	DetourAttach(&(PVOID&)getFramedTextSize, getFramedTextSizeHooked);
	DetourAttach(&(PVOID&)getFontHeight, getFontHeightHooked);
	DetourAttach(&(PVOID&)setTextSize, setTextSizeHooked);
	DetourTransactionCommit();
}

void destroyHooks()
{
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourDetach(&(PVOID&)drawImage, drawImageHooked);
	DetourDetach(&(PVOID&)drawPerspectiveImage, drawPerspectiveImageHooked);
	DetourDetach(&(PVOID&)drawShiftedImage, drawShiftedImageHooked);
	DetourDetach(&(PVOID&)drawVerticalCropImage, drawVerticalCropImageHooked);
	DetourDetach(&(PVOID&)drawClippedImage, drawClippedImageHooked);
	DetourDetach(&(PVOID&)drawImageFast, drawImageFastHooked);
	DetourDetach(&(PVOID&)drawShadow, drawShadowHooked);
	DetourDetach(&(PVOID&)drawSolidRectEx, drawSolidRectExHooked);
	DetourDetach(&(PVOID&)drawLine, drawLineHooked);
	DetourDetach(&(PVOID&)drawGroundTile, drawGroundTileHooked);
	DetourDetach(&(PVOID&)drawWallTile, drawWallTileHooked);
	DetourDetach(&(PVOID&)drawTransWallTile, drawTransWallTileHooked);
	DetourDetach(&(PVOID&)drawShadowTile, drawShadowTileHooked);

	DetourDetach(&(PVOID&)takeScreenShot, takeScreenShotHooked);
	DetourDetach(&(PVOID&)drawNormalText, drawNormalTextHooked);
	DetourDetach(&(PVOID&)drawNormalTextEx, drawNormalTextExHooked);
	DetourDetach(&(PVOID&)drawFramedText, drawFramedTextHooked);
	DetourDetach(&(PVOID&)drawRectangledText, drawRectangledTextHooked);
	DetourDetach(&(PVOID&)getNormalTextWidth, getNormalTextWidthHooked);
	DetourDetach(&(PVOID&)getNormalTextNWidth, getNormalTextNWidthHooked);
	DetourDetach(&(PVOID&)getFramedTextSize, getFramedTextSizeHooked);
	DetourDetach(&(PVOID&)getFontHeight, getFontHeightHooked);
	DetourDetach(&(PVOID&)setTextSize, setTextSizeHooked);
	DetourTransactionCommit();
}

}