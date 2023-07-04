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

#include "d2/structs.h"

namespace d2gl {

enum class TextAlign {
	Left,
	Right,
	Center,
};

struct D2FontInfo {
	uint8_t id;
	float size;
	float weight;
	float line_height;
	float letter_spacing;
	wchar_t color = 0;
};

struct D2PopupInfo {
	glm::vec<2, uint16_t> size0;
	glm::vec<2, uint16_t> size1;
};

struct D2TextInfo {
	uint8_t cell_num;
	glm::vec<2, uint16_t> size;
	TextAlign align;
	const wchar_t* str;
};

struct D2TextMetrics {
	glm::vec<2, uint8_t> large_text;
	glm::vec<2, uint8_t> entry_text;
	uint8_t hp_bar;
};

// clang-format off
inline const std::map<wchar_t, uint32_t> g_text_colors = {
	{ L'\x01', 0xDFB679FF }, { L'\x02', 0x000000FF }, { L'\x03', 0xDFB67966 }, { L'\x04', 0xC22121FF },
	{ L'\x05', 0x959595FF }, { L'\x06', 0xE09595FF }, { L'\x07', 0xB6E12EFF }, { L'\x08', 0x9EA0A0FF },
	{ L'\x09', 0x5CA6A4FF }, { L'\x0A', 0xA2AAA5FF }, { L'\x0B', 0xA6E0A7FF }, { L'\x0C', 0xA9A9B0FF },
	{ L'\x0D', 0xA9B1B1FF }, { L'\x0E', 0xE2ABE2FF }, { L'\x0F', 0xEB2EAEFF }, { L'\x10', 0xB0B2E2FF },
	{ L'\x11', 0xEBEBE1FF }, { L'\x12', 0xE22EB8FF }, { L'\x13', 0xEBEBEBFF }, { L'\x14', 0x21C7CAFF },
	{ L'\x15', 0xCB21CDFF }, { L'\x16', 0xCED126FF }, { L'\x17', 0xD2D328FF }, { L'\x18', 0xD5D6D8FF },
	{ L'\x19', 0xD9D965FF }, { L'\x1A', 0xDA29DAFF }, { L'\x1B', 0xDC68DBFF }, { L'\x1C', 0xDCAD2BFF },
	{ L'\x1D', 0x2BDFDFFF }, { L'\x1E', 0xDFADDFFF }, { L'\x1F', 0x2CE02CFF }, { L'\x21', 0xE0B3B3FF },
	{ L'\x22', 0x2C2CE1FF }, { L'\x23', 0xE1E1E2FF }, { L'\x24', 0xE2EBEBFF }, { L'\x25', 0xEBEB77FF },
	{ L'\x26', 0x898D8BFF }, { L'\x27', 0x8D8D8BFF }, { L'\x28', 0x8EA1A0FF }, { L'\x29', 0x0DCCE9FF },
	{ L'\x2A', 0x2325CEFF }, { L'\x2B', 0x8DEAD3FF }, { L'\x2C', 0xD5D3EAFF }, { L'\x2D', 0xEAD8D9FF },
	{ L'\x2E', 0x29DCDCFF }, { L'\x2F', 0xDFDFEBFF }, { L'\x30', 0xFEFEFEFF }, { L'\x31', 0xFE6950FF },
	{ L'\x32', 0x25FE00FF }, { L'\x33', 0x7B7BFEFF }, { L'\x34', 0xDFB679FF }, { L'\x35', 0x7B7B7BFF },
	{ L'\x36', 0x00000000 }, { L'\x37', 0xD9CD8EFF }, { L'\x38', 0xFFB200FF }, { L'\x39', 0xEBEB75FF },
	{ L'\x3A', 0x008C00FF }, { L'\x3B', 0xB700FFFF }, { L'\x3C', 0x00D200FF }, { L'\x3D', 0xEBEBEBFF },
};

inline const std::array<wchar_t, 18> g_default_colors = {
	L'\x30', // 0.White (Item Descriptions)
	L'\x31', // 1.Red
	L'\x32', // 2.Bright Green (Set Items)
	L'\x33', // 3.Blue (Magic Items)
	L'\x34', // 4.Gold (Unique Items)
	L'\x35', // 5.Dark Gray (Socketed / Ethereal Items)
	L'\x36', // 6.Black
	L'\x37', // 7.Tan
	L'\x38', // 8.Orange (Crafted Items)
	L'\x39', // 9.Yellow (Rare Items)
	L'\x3A', // 10.Dark Green
	L'\x3B', // 11.Purple
	L'\x2F', // 12.White (Brighter than Light Gray)
	L'\x2E', // 13.Messed Up White (Same as above but text is messed up)
	L'\x09', // 14.Teal
	L'\x0C', // 15.Light Gray
	L'\x03', // 16.Options (Half transparent) //Custom
	L'\x04', // 17.Red (Death texts) //Custom
};

inline std::map<uint32_t, D2FontInfo> g_font_sizes = {
	{  1, { 0, 14.3f, 1.02f, 1.1f, -0.022f, } }, // All Normal texts
	{  9, { 0, 16.4f, 1.10f, 1.1f, -0.022f, L'\x02' } }, // Menu Button label
	{ 10, { 0, 12.0f, 1.14f, 1.1f, -0.022f, L'\x02' } }, // Menu Button label (smaller)
	{ 11, { 0, 12.2f, 1.06f, 1.1f, -0.022f, } }, // ?
	{ 99, { 0, 11.2f, 1.00f, 1.1f, -0.022f, } }, // FramedText in small size //Custom

	{  6, { 1,  9.8f, 1.14f, 1.0f,  0.115f } }, // All Small texts
	{ 98, { 1, 10.4f, 1.14f, 1.0f,  0.115f } }, // Custom

	{  0, { 2, 10.8f, 1.14f, 1.0f,  0.115f } }, // Char name on stats panel
	{  4, { 2, 10.2f, 1.14f, 1.0f,  0.115f } }, // Skill tree level number 2digits
	{  5, { 2, 14.8f, 1.14f, 1.0f,  0.115f } }, // Menu screen copyright text
	{  8, { 2, 12.4f, 1.14f, 1.0f,  0.115f } }, // Talking Text
	{ 12, { 2,  9.0f, 1.14f, 1.0f,  0.115f } }, // ?
	{ 13, { 2, 11.6f, 1.14f, 1.0f,  0.090f } }, // Message, Shrine, Keybind config

	{  2, { 3, 27.0f, 1.10f, 1.0f, -0.022f, L'\x01' } }, // Char create screen title, Resolution text eg:800x600
	{  3, { 3, 36.0f, 1.10f, 1.0f, -0.022f, L'\x01' } }, // Char selection title (selected char name)
	{  7, { 3, 20.9f, 1.10f, 1.0f, -0.022f, L'\x01' } }, // Menu screen popup title
};

inline const std::vector<D2PopupInfo> g_popups = {
	{ { 256, 256 }, {  70,  85 } }, // CinematicsSelection.dc6
	{ { 256, 256 }, {  70, 171 } }, // CinematicsSelectionEXP.dc6
	{ { 256, 256 }, {  84,  84 } }, // GWListBox.dc6 // PopUpLarge.dc6
	{ { 256, 224 }, {  84, 224 } }, // PopUp_340x224.dc6
	{ { 256, 256 }, { 229,  34 } }, // PopUpLargest.dc6 // PopUpLargest2.dc6
	{ { 256, 245 }, { 154, 245 } }, // PopupWide.dc6 // PopUpWideNoHoles.dc6
	{ { 256, 176 }, {   8, 176 } }, // popupok.dc6 // passwordbox.dc6 // PopUpOKCancel.DC6
	{ { 256, 200 }, {  70, 200 } }, // DifficultyLevels.dc6
	{ { 256, 256 }, {   0,   0 } }, // TileableDialog.dc6
};

inline const std::vector<D2TextMetrics> g_text_size = {
	{ { 36, 26 }, { 26, 41 }, 16 }, // ENG
	{ { 36, 26 }, { 26, 41 }, 16 }, // ESP
	{ { 36, 26 }, { 26, 41 }, 16 }, // DEU
	{ { 36, 26 }, { 26, 41 }, 16 }, // FRA
	{ {  0,  0 }, {  0,  0 },  0 }, // POR
	{ { 36, 26 }, { 26, 41 }, 16 }, // ITA
	{ { 44, 31 }, { 26, 31 }, 22 }, // JPN
	{ { 38, 34 }, { 26, 34 }, 22 }, // KOR
	{ {  0,  0 }, {  0,  0 },  0 }, // SIN
	{ { 54, 34 }, { 26, 33 }, 18 }, // CHI
	{ { 37, 29 }, { 26, 29 }, 19 }, // POL
	{ { 41, 30 }, { 26, 30 }, 17 }, // RUS
};

inline const std::vector<std::vector<std::string>> g_option_overwrites = {
	{ "3dbias", "perspective", "resolution", "smallon", "smalloff" }, // ENG
	{ "automapfade", "automapparty" }, // ESP
	{ "resolution" }, // DEU
	{ "textonly", "automapfade", "blendshadow", "perspective", "smallno", "smallyes" }, // FRA
	{  }, // POR
	{ "smallyes", "auto", "automapparty", "smallno" }, // ITA
	{ "perspective", "640x480", "automapfade", "blendshadow", "center", "everything", "full", "gamma", "low", "medium", "mini", "music" }, // JPN
	{ "smallyes", "smallno", "sound", "music", "3dsound", "eax", "audioonly", "textonly", "videooptions", "resolution", "640x480", "medium", "low", "blendshadow", "gamma", "contrast", "automapoptions", "automapmode", "full", "automapfade", "everything", "center", "auto", "automapcenter", "automapparty", "automappartynames", "cfgoptions" }, // KOR
	{  }, // SIN
	{ "previous", "resolution", "smallno", "smalloff", "smallyes", "soundoptions", "sprevious", "textonly", "videooptions", "3dbias", "640x480", "800x600", "audioonly", "audiotext", "auto", "automapcenter", "automapfade", "automapmode", "automapparty", "automappartynames", "blendshadow", "center", "contrast", "eax", "everything", "full", "gamma", "high", "lightquality", "low", "medium", "mini", "music", "perspective", "cfgoptions" }, // CHI
	{ "perspective", "medium", "center" }, // POL
	{ "3dbias", "perspective", "resolution", "smallon", "blendshadow", "auto", "mini", "full", "automapmode", "contrast" }, // RUS
};

inline std::vector<D2TextInfo> g_options_texts = {
	{ 0, { 160, 36 }, TextAlign::Center, L"OPTIONS" }, // options.dc6
	{ 1, { 178, 36 }, TextAlign::Center, L"SAVE AND EXIT GAME" }, // exit.dc6
	{ 1, { 100, 36 }, TextAlign::Center, L"RETURN TO GAME" }, // returntogame.dc6
	{ 1, {  57, 36 }, TextAlign::Center, L"PREVIOUS MENU" }, // previous.dc6
	{ 0, { 231, 26 }, TextAlign::Center, L"PREVIOUS MENU" }, // sprevious.dc6
	{ 0, {  41, 26 }, TextAlign::Right,  L"ON" }, // smallon.dc6
	{ 0, {  52, 26 }, TextAlign::Right,  L"OFF" }, // smalloff.dc6
	{ 0, {  51, 26 }, TextAlign::Right,  L"YES" }, // smallyes.dc6
	{ 0, {  40, 26 }, TextAlign::Right,  L"NO" }, // smallno.dc6
	{ 1, {  53, 36 }, TextAlign::Center, L"SOUND OPTIONS" }, // soundoptions.dc6
	{ 0, {  95, 26 }, TextAlign::Left,   L"SOUND" }, // sound.dc6
	{ 0, {  84, 26 }, TextAlign::Left,   L"MUSIC" }, // music.dc6
	{ 0, { 144, 26 }, TextAlign::Left,   L"3D SOUND" }, // 3dsound.dc6
	{ 1, { 106, 26 }, TextAlign::Left,   L"ENVIRONMENTAL EFFECTS" }, // eax.dc6
	{ 0, { 109, 26 }, TextAlign::Left,   L"3D BIAS" }, // 3dbias.dc6
	{ 0, { 157, 26 }, TextAlign::Left,   L"NPC SPEECH" }, // npcspeech.dc6
	{ 1, {   4, 26 }, TextAlign::Right,  L"AUDIO AND TEXT" }, // audiotext.dc6
	{ 0, { 184, 26 }, TextAlign::Right,  L"AUDIO ONLY" }, // audioonly.dc6
	{ 0, { 169, 26 }, TextAlign::Right,  L"TEXT ONLY" }, // textonly.dc6
	{ 1, {  39, 36 }, TextAlign::Center, L"VIDEO OPTIONS" }, // videooptions.dc6
	{ 0, { 175, 26 }, TextAlign::Left,   L"RESOLUTION" }, // resolution.dc6
	{ 0, { 135, 26 }, TextAlign::Right,  L"800x600" }, // 800x600.dc6
	{ 0, { 123, 26 }, TextAlign::Right,  L"640x480" }, // 640x480.dc6
	{ 1, {   9, 26 }, TextAlign::Left,   L"LIGHTING QUALITY" }, // lightquality.dc6
	{ 0, {  57, 26 }, TextAlign::Right,  L"HIGH" }, // high.dc6
	{ 0, { 108, 26 }, TextAlign::Right,  L"MEDIUM" }, // medium.dc6
	{ 0, {  66, 26 }, TextAlign::Right,  L"LOW" }, // low.dc6
	{ 1, {  10, 26 }, TextAlign::Left,   L"BLENDED SHADOWS" }, // blendshadow.dc6
	{ 0, { 174, 26 }, TextAlign::Left,   L"PERSPECTIVE" }, // perspective.dc6
	{ 0, { 105, 26 }, TextAlign::Left,   L"GAMMA" }, // gamma.dc6
	{ 0, { 151, 26 }, TextAlign::Left,   L"CONTRAST" }, // contrast.dc6
	{ 1, { 122, 36 }, TextAlign::Center, L"AUTOMAP OPTIONS" }, // automapoptions.dc6
	{ 0, { 213, 26 }, TextAlign::Left,   L"AUTOMAP SIZE" }, // automapmode.dc6
	{ 0, { 180, 26 }, TextAlign::Right,  L"FULL SCREEN" }, // full.dc6
	{ 0, { 131, 26 }, TextAlign::Right,  L"MINI MAP" }, // mini.dc6
	{ 0, {  70, 26 }, TextAlign::Left,   L"FADE" }, // automapfade.dc6
	{ 0, { 173, 26 }, TextAlign::Right,  L"EVERYTHING" }, // everything.dc6
	{ 0, { 104, 26 }, TextAlign::Right,  L"CENTER" }, // center.dc6
	{ 0, {  86, 26 }, TextAlign::Right,  L"AUTO" }, // auto.dc6
	{ 1, {  80, 26 }, TextAlign::Left,   L"CENTER WHEN CLEARED" }, // automapcenter.dc6
	{ 0, { 194, 26 }, TextAlign::Left,   L"SHOW PARTY" }, // automapparty.dc6
	{ 0, { 190, 26 }, TextAlign::Left,   L"SHOW NAMES" }, // automappartynames.dc6
	{ 1, { 167, 36 }, TextAlign::Center, L"CONFIGURE CONTROLS" }, // cfgoptions.dc6
};
inline const std::map<uint32_t, std::vector<D2TextInfo>> g_options_text_others = {
	{ LANG_ESP, {
		{ 0, { 175, 36 }, TextAlign::Center, L"OPCIONES" }, // OPTIONS
		{ 1, {  73, 36 }, TextAlign::Center, L"SALVAR Y SALIR" }, // SAVE AND EXIT GAME
		{ 1, { 208, 36 }, TextAlign::Center, L"VOLVER A PARTIDA" }, // RETURN TO GAME
		{ 1, {  65, 36 }, TextAlign::Center, L"MENÚ ANTERIOR" }, // PREVIOUS MENU
		{ 0, { 237, 26 }, TextAlign::Center, L"MENÚ ANTERIOR" }, // PREVIOUS MENU
		{ 0, { 152, 26 }, TextAlign::Right,  L"ACTIVADA" }, // ON
		{ 0, {  81, 26 }, TextAlign::Right,  L"DESéA" }, // OFF
		{ 0, {  39, 26 }, TextAlign::Right,  L"SÍ" }, // YES
		{ 0, {  40, 26 }, TextAlign::Right,  L"NO" }, // NO
		{ 1, {  47, 36 }, TextAlign::Center, L"OPCIONES DE SONIDO" }, // SOUND OPTIONS
		{ 0, { 101, 26 }, TextAlign::Left,   L"SONIDO" }, // SOUND
		{ 0, { 107, 26 }, TextAlign::Left,   L"MÚSICA" }, // MUSIC
		{ 0, { 150, 26 }, TextAlign::Left,   L"SONIDO 3D" }, // 3D SOUND
		{ 0, { 222, 26 }, TextAlign::Left,   L"EFEC. AMBIENTE" }, // ENVIRONMENTAL EFFECTS
		{ 0, { 108, 26 }, TextAlign::Left,   L"3D BIAS" }, // 3D BIAS
		{ 0, { 136, 26 }, TextAlign::Left,   L"DIÁLOGOS" }, // NPC SPEECH
		{ 0, { 253, 26 }, TextAlign::Right,  L"SONIDO Y TEXTO" }, // AUDIO AND TEXT
		{ 0, { 187, 26 }, TextAlign::Right,  L"SÓLO SONIDO" }, // AUDIO ONLY
		{ 0, { 184, 26 }, TextAlign::Right,  L"SÓLO TEXTO" }, // TEXT ONLY
		{ 1, {  27, 36 }, TextAlign::Center, L"OPCIONES DE VÍDEO" }, // VIDEO OPTIONS
		{ 0, { 149, 26 }, TextAlign::Left,   L"RESOLUCIÓN" }, // RESOLUTION
		{ 0, { 135, 26 }, TextAlign::Right,  L"800x600" },
		{ 0, { 123, 26 }, TextAlign::Right,  L"640x480" },
		{ 1, { 109, 26 }, TextAlign::Left,   L"CALIDAD DE ILUMINACIÓN" }, // LIGHTING QUALITY
		{ 0, {  78, 26 }, TextAlign::Right,  L"ALTA" }, // HIGH
		{ 0, {  85, 26 }, TextAlign::Right,  L"MEDIA" }, // MEDIUM
		{ 0, {  67, 26 }, TextAlign::Right,  L"BAJA" }, // LOW
		{ 0, { 225, 26 }, TextAlign::Left,   L"FUND. SOMBRAS" }, // BLENDED SHADOWS
		{ 0, { 180, 26 }, TextAlign::Left,   L"PERSPECTIVA" }, // PERSPECTIVE
		{ 0, { 105, 26 }, TextAlign::Left,   L"GAMMA" }, // GAMMA
		{ 0, { 151, 26 }, TextAlign::Left,   L"CONTRAST" }, // CONTRAST
		{ 1, {  21, 36 }, TextAlign::Center, L"OPCIONES DEL AUTOMAPA" }, // AUTOMAP OPTIONS
		{ 0, { 111, 26 }, TextAlign::Left,   L"TAMAÑO" }, // AUTOMAP SIZE
		{ 1, {  14, 26 }, TextAlign::Right,  L"PANTALLA COMPLETA" }, // FULL SCREEN
		{ 0, { 122, 26 }, TextAlign::Right,  L"MINI MAPA" }, // MINI MAP
		{ 0, { 124, 26 }, TextAlign::Left,   L"FUNDIDO" }, // FADE
		{ 0, {  80, 26 }, TextAlign::Right,  L"TODO" }, // EVERYTHING
		{ 0, { 110, 26 }, TextAlign::Right,  L"CENTRO" }, // CENTER
		{ 0, { 170, 26 }, TextAlign::Right,  L"AUTOMÁTICO" }, // AUTO
		{ 1, {  65, 26 }, TextAlign::Left,   L"CENTRAR AL DESPEJAR" }, // CENTER WHEN CLEARED
		{ 0, { 254, 26 }, TextAlign::Left,   L"MOSTRAR EQUIPO" }, // SHOW PARTY
		{ 1, {  19, 26 }, TextAlign::Left,   L"MOSTRAR NOMBRES" }, // SHOW NAMES
		{ 1, { 222, 36 }, TextAlign::Center, L"CONFIGURAR CONTROLES" }, // CONFIGURE CONTROLS
	} },
	{ LANG_DEU, {
		{ 0, { 187, 36 }, TextAlign::Center, L"OPTIONEN" }, // OPTIONS
		{ 1, { 208, 36 }, TextAlign::Center, L"SPEICHERN / SPIEL VERL." }, // SAVE AND EXIT GAME
		{ 1, { 114, 36 }, TextAlign::Center, L"ZURÜCK ZUM SPIEL" }, // RETURN TO GAME
		{ 1, {  30, 36 }, TextAlign::Center, L"VORIGES MENÜ" }, // PREVIOUS MENU
		{ 0, { 211, 26 }, TextAlign::Center, L"VORIGES MENÜ" }, // PREVIOUS MENU
		{ 0, {  42, 26 }, TextAlign::Right,  L"EIN" }, // ON
		{ 0, {  59, 26 }, TextAlign::Right,  L"AUS" }, // OFF
		{ 0, {  30, 26 }, TextAlign::Right,  L"JA" }, // YES
		{ 0, {  61, 26 }, TextAlign::Right,  L"NEIN" }, // NO
		{ 1, {  72, 36 }, TextAlign::Center, L"SOUND OPTIONEN" }, // SOUND OPTIONS
		{ 0, {  95, 26 }, TextAlign::Left,   L"SOUND" }, // SOUND
		{ 0, {  84, 26 }, TextAlign::Left,   L"MUSIK" }, // MUSIC
		{ 0, { 138, 26 }, TextAlign::Left,   L"3D SOUND" }, // 3D SOUND
		{ 1, {  21, 26 }, TextAlign::Left,   L"UMGEBUNGSEFFEKTE" }, // ENVIRONMENTAL EFFECTS
		{ 0, { 102, 26 }, TextAlign::Left,   L"3D BIAS" }, // 3D BIAS
		{ 0, { 176, 26 }, TextAlign::Left,   L"NPC SPRACHE" }, // NPC SPEECH
		{ 1, {   5, 26 }, TextAlign::Right,  L"AUDIO UND TEXT" }, // AUDIO AND TEXT
		{ 0, { 168, 26 }, TextAlign::Right,  L"NUR AUDIO" }, // AUDIO ONLY
		{ 0, { 153, 26 }, TextAlign::Right,  L"NUR TEXT" }, // TEXT ONLY
		{ 1, {  75, 36 }, TextAlign::Center, L"GRAFIK OPTIONEN" }, // VIDEO OPTIONS
		{ 0, { 169, 26 }, TextAlign::Left,   L"AUFLÖSUNG" }, // RESOLUTION
		{ 0, { 135, 26 }, TextAlign::Right,  L"800x600" },
		{ 0, { 123, 26 }, TextAlign::Right,  L"640x480" },
		{ 0, { 225, 26 }, TextAlign::Left,   L"LICHTQUALITÄT" }, // LIGHTING QUALITY
		{ 0, {  70, 26 }, TextAlign::Right,  L"HOCH" }, // HIGH
		{ 0, {  99, 26 }, TextAlign::Right,  L"MITTEL" }, // MEDIUM
		{ 0, { 103, 26 }, TextAlign::Right,  L"NIEDRIG" }, // LOW
		{ 1, {  17, 26 }, TextAlign::Left,   L"SCHATTENVERLAUF" }, // BLENDED SHADOWS
		{ 0, { 173, 26 }, TextAlign::Left,   L"PERSPEKTIVE" }, // PERSPECTIVE
		{ 0, { 105, 26 }, TextAlign::Left,   L"GAMMA" }, // GAMMA
		{ 0, { 151, 26 }, TextAlign::Left,   L"KONTRAST" }, // CONTRAST
		{ 1, { 240, 36 }, TextAlign::Center, L"AUTOM. KARTE OPTIONEN" }, // AUTOMAP OPTIONS
		{ 1, {  69, 26 }, TextAlign::Left,   L"GR. D. AUTOM. KARTE" }, // AUTOMAP SIZE
		{ 0, { 108, 26 }, TextAlign::Right,  L"VOLLBILD" }, // FULL SCREEN
		{ 0, { 128, 26 }, TextAlign::Right,  L"MINIKARTE" }, // MINI MAP
		{ 0, { 174, 26 }, TextAlign::Left,   L"AUSBLENDEN" }, // FADE
		{ 0, {  79, 26 }, TextAlign::Right,  L"ALLES" }, // EVERYTHING
		{ 0, { 163, 26 }, TextAlign::Right,  L"ZENTRIEREN" }, // CENTER
		{ 0, {  86, 26 }, TextAlign::Right,  L"AUTO" }, // AUTO
		{ 1, {  87, 26 }, TextAlign::Left,   L"ZENTR. NACH LÖSCHUNG" }, // CENTER WHEN CLEARED
		{ 0, { 202, 26 }, TextAlign::Left,   L"PARTY ZEIGEN" }, // SHOW PARTY
		{ 0, { 203, 26 }, TextAlign::Left,   L"NAMEN ZEIGEN" }, // SHOW NAMES
		{ 2, {  20, 36 }, TextAlign::Center, L"STEUERUNG KONFIGURIEREN" }, // CONFIGURE CONTROLS
	} },
	{ LANG_FRA, {
		{ 0, { 160, 36 }, TextAlign::Center, L"OPTIONS" }, // OPTIONS
		{ 1, { 161, 36 }, TextAlign::Center, L"SAUVER ET QUITTER" }, // SAVE AND EXIT GAME
		{ 1, { 178, 36 }, TextAlign::Center, L"REVENIR A LA PARTIE" }, // RETURN TO GAME
		{ 1, {  74, 36 }, TextAlign::Center, L"MENU PRÉCÉDENT" }, // PREVIOUS MENU
		{ 0, { 246, 26 }, TextAlign::Center, L"MENU PRÉCÉDENT" }, // PREVIOUS MENU
		{ 0, {  52, 26 }, TextAlign::Right,  L"OUI" }, // ON
		{ 0, {  59, 26 }, TextAlign::Right,  L"NON" }, // OFF
		{ 0, {  53, 26 }, TextAlign::Right,  L"OUI" }, // YES
		{ 0, {  60, 26 }, TextAlign::Right,  L"NON" }, // NO
		{ 1, {  49, 36 }, TextAlign::Center, L"OPTIONS AUDIO" }, // SOUND OPTIONS
		{ 0, {  54, 26 }, TextAlign::Left,   L"SON" }, // SOUND
		{ 0, { 126, 26 }, TextAlign::Left,   L"MUSIQUE" }, // MUSIC
		{ 0, { 103, 26 }, TextAlign::Left,   L"SON 3D" }, // 3D SOUND
		{ 1, {  87, 26 }, TextAlign::Left,   L"EFFETS ENVIRONNEMENT" }, // ENVIRONMENTAL EFFECTS
		{ 0, { 117, 26 }, TextAlign::Left,   L"DIST. 3D" }, // 3D BIAS
		{ 0, { 204, 26 }, TextAlign::Left,   L"DIALOGUE NPC" }, // NPC SPEECH
		{ 0, { 251, 26 }, TextAlign::Right,  L"AUDIO ET TEXTE" }, // AUDIO AND TEXT
		{ 0, {  92, 26 }, TextAlign::Right,  L"AUDIO" }, // AUDIO ONLY
		{ 0, {  94, 26 }, TextAlign::Right,  L"TEXTE" }, // TEXT ONLY
		{ 1, {  39, 36 }, TextAlign::Center, L"OPTIONS VIDEO" }, // VIDEO OPTIONS
		{ 0, { 173, 26 }, TextAlign::Left,   L"RÉSOLUTION" }, // RESOLUTION
		{ 0, { 135, 26 }, TextAlign::Right,  L"800x600" },
		{ 0, { 123, 26 }, TextAlign::Right,  L"640x480" },
		{ 1, {  25, 26 }, TextAlign::Left,   L"OPTIONS ÉCLAIRAGE" }, // LIGHTING QUALITY
		{ 0, {  96, 26 }, TextAlign::Right,  L"HAUTE" }, // HIGH
		{ 0, { 133, 26 }, TextAlign::Right,  L"MOYENNE" }, // MEDIUM
		{ 0, {  80, 26 }, TextAlign::Right,  L"BASSE" }, // LOW
		{ 0, { 247, 26 }, TextAlign::Left,   L"OMBRES FONDUES" }, // BLENDED SHADOWS
		{ 0, { 174, 26 }, TextAlign::Left,   L"PERSPECTIVE" }, // PERSPECTIVE
		{ 0, { 105, 26 }, TextAlign::Left,   L"GAMMA" }, // GAMMA
		{ 0, { 151, 26 }, TextAlign::Left,   L"CONTRAST" }, // CONTRAST
		{ 1, { 181, 36 }, TextAlign::Center, L"OPTIONS CARTE-AUTO" }, // AUTOMAP OPTIONS
		{ 0, {  93, 26 }, TextAlign::Left,   L"TAILLE" }, // AUTOMAP SIZE
		{ 0, { 177, 26 }, TextAlign::Right,  L"PLEIN ÉCRAN" }, // FULL SCREEN
		{ 0, { 149, 26 }, TextAlign::Right,  L"MINICARTE" }, // MINI MAP
		{ 0, {  97, 26 }, TextAlign::Left,   L"FONDU" }, // FADE
		{ 0, {  84, 26 }, TextAlign::Right,  L"TOUT" }, // EVERYTHING
		{ 0, { 104, 26 }, TextAlign::Right,  L"CENTRÉ" }, // CENTER
		{ 0, {  86, 26 }, TextAlign::Right,  L"AUTO" }, // AUTO
		{ 1, { 100, 26 }, TextAlign::Left,   L"CENTRER QUAND EFFACÉ" }, // CENTER WHEN CLEARED
		{ 0, { 256, 26 }, TextAlign::Left,   L"AFFICHER COTERIE" }, // SHOW PARTY
		{ 0, { 218, 26 }, TextAlign::Left,   L"AFFICHER NOMS" }, // SHOW NAMES
		{ 1, { 226, 36 }, TextAlign::Center, L"CONFIGURER COMMANDES" }, // CONFIGURE CONTROLS
	} },
	{ LANG_POR, {} },
	{ LANG_ITA, {
		{ 0, { 146, 36 }, TextAlign::Center, L"OPZIONI" }, // OPTIONS
		{ 2, {   4, 36 }, TextAlign::Center, L"SALVA ED ESCI DAL GIOCO" }, // SAVE AND EXIT GAME
		{ 1, {  85, 36 }, TextAlign::Center, L"TORNA AL GOCO" }, // RETURN TO GAME
		{ 1, {  98, 36 }, TextAlign::Center, L"MENU PRECEDENTE" }, // PREVIOUS MENU
		{ 1, {   5, 26 }, TextAlign::Center, L"MENU PRECEDENTE" }, // PREVIOUS MENU
		{ 0, {  39, 26 }, TextAlign::Right,  L"SÌ" }, // ON
		{ 0, {  40, 26 }, TextAlign::Right,  L"NO" }, // OFF
		{ 0, {  41, 26 }, TextAlign::Right,  L"SÌ" }, // YES
		{ 0, {  42, 26 }, TextAlign::Right,  L"NO" }, // NO
		{ 1, {  35, 36 }, TextAlign::Center, L"OPZIONI AUDIO" }, // SOUND OPTIONS
		{ 0, { 114, 26 }, TextAlign::Left,   L"SONORO" }, // SOUND
		{ 0, { 106, 26 }, TextAlign::Left,   L"MUSICA" }, // MUSIC
		{ 0, { 141, 26 }, TextAlign::Left,   L"AUDIO 3D" }, // 3D SOUND
		{ 1, {  33, 26 }, TextAlign::Left,   L"EFFETTI AMBIENTALI" }, // ENVIRONMENTAL EFFECTS
		{ 0, { 173, 26 }, TextAlign::Left,   L"DISTORCI 3D" }, // 3D BIAS
		{ 0, { 197, 26 }, TextAlign::Left,   L"PARLATO PNG" }, // NPC SPEECH
		{ 0, { 229, 26 }, TextAlign::Right,  L"AUDIO E TESTO" }, // AUDIO AND TEXT
		{ 0, { 178, 26 }, TextAlign::Right,  L"SOLO AUDIO" }, // AUDIO ONLY
		{ 0, { 176, 26 }, TextAlign::Right,  L"SOLO TESTO" }, // TEXT ONLY
		{ 1, {  25, 36 }, TextAlign::Center, L"OPZIONI VIDEO" }, // VIDEO OPTIONS
		{ 0, { 177, 26 }, TextAlign::Left,   L"RISOLUZIONE" }, // RESOLUTION
		{ 0, { 135, 26 }, TextAlign::Right,  L"800x600" },
		{ 0, { 123, 26 }, TextAlign::Right,  L"640x480" },
		{ 1, {  99, 26 }, TextAlign::Left,   L"QUALITYÀ ILLUMINAZIONE" }, // LIGHTING QUALITY
		{ 0, {  78, 26 }, TextAlign::Right,  L"ALTA" }, // HIGH
		{ 0, {  85, 26 }, TextAlign::Right,  L"MEDIA" }, // MEDIUM
		{ 0, {  87, 26 }, TextAlign::Right,  L"BASSA" }, // LOW
		{ 0, { 238, 26 }, TextAlign::Left,   L"OMBRE SFUMATE" }, // BLENDED SHADOWS
		{ 0, { 189, 26 }, TextAlign::Left,   L"PROSPETTIVA" }, // PERSPECTIVE
		{ 0, { 105, 26 }, TextAlign::Left,   L"GAMMA" }, // GAMMA
		{ 0, { 172, 26 }, TextAlign::Left,   L"CONTRASTO" }, // CONTRAST
		{ 1, { 219, 36 }, TextAlign::Center, L"OPZIONI MAPA AUTOM." }, // AUTOMAP OPTIONS
		{ 0, { 180, 26 }, TextAlign::Left,   L"AUTOMAPPA" }, // AUTOMAP SIZE
		{ 0, { 240, 26 }, TextAlign::Right,  L"SCHERMO INTERO" }, // FULL SCREEN
		{ 0, { 151, 26 }, TextAlign::Right,  L"MINIMAPPA" }, // MINI MAP
		{ 0, { 184, 26 }, TextAlign::Left,   L"DISSOLVENZA" }, // FADE
		{ 0, { 104, 26 }, TextAlign::Right,  L"TUTTO" }, // EVERYTHING
		{ 0, { 111, 26 }, TextAlign::Right,  L"CENTRA" }, // CENTER
		{ 0, { 198, 26 }, TextAlign::Right,  L"AUTOMATICA" }, // AUTO
		{ 1, { 102, 26 }, TextAlign::Left,   L"CENTRA QUANDO LIBERO" }, // CENTER WHEN CLEARED
		{ 0, { 241, 26 }, TextAlign::Left,   L"MOSTRA GRUPPO" }, // SHOW PARTY
		{ 0, { 203, 26 }, TextAlign::Left,   L"MOSTRA NOMI" }, // SHOW NAMES
		{ 1, { 188, 36 }, TextAlign::Center, L"CONFIGURA CONTROLLI" }, // CONFIGURE CONTROLS
	} },
	{ LANG_JPN, {
		{ 0, { 125, 44 }, TextAlign::Center, L"オプション" }, // OPTIONS
		{ 1, {  60, 44 }, TextAlign::Center, L"セーブしてゲームを終了" }, // SAVE AND EXIT GAME
		{ 0, { 171, 44 }, TextAlign::Center, L"ゲームに戻る" }, // RETURN TO GAME
		{ 0, { 196, 44 }, TextAlign::Center, L"前のメニューへ" }, // PREVIOUS MENU
		{ 0, { 134, 31 }, TextAlign::Center, L"前のメニューへ" }, // PREVIOUS MENU
		{ 0, {  33, 31 }, TextAlign::Right,  L"ON" }, // ON
		{ 0, {  48, 31 }, TextAlign::Right,  L"OFF" }, // OFF
		{ 0, {  39, 31 }, TextAlign::Right,  L"はい" }, // YES
		{ 0, {  57, 31 }, TextAlign::Right,  L"いいえ" }, // NO
		{ 0, { 221, 44 }, TextAlign::Center, L"サウンドオプション" }, // SOUND OPTIONS
		{ 0, {  66, 31 }, TextAlign::Left,   L"サウンド" }, // SOUND
		{ 0, {  49, 31 }, TextAlign::Left,   L"音楽" }, // MUSIC
		{ 0, {  98, 31 }, TextAlign::Left,   L"3Dサウンド" }, // 3D SOUND
		{ 0, { 120, 31 }, TextAlign::Left,   L"環境効果音" }, // ENVIRONMENTAL EFFECTS
		{ 0, { 106, 31 }, TextAlign::Left,   L"3Dバイアス" }, // 3D BIAS
		{ 0, { 113, 31 }, TextAlign::Left,   L"NPCの会話" }, // NPC SPEECH
		{ 0, { 132, 31 }, TextAlign::Right,  L"音声とテキスト" }, // AUDIO AND TEXT
		{ 0, {  86, 31 }, TextAlign::Right,  L"音声のみ" }, // AUDIO ONLY
		{ 0, { 107, 31 }, TextAlign::Right,  L"テキストのみ" }, // TEXT ONLY
		{ 0, { 212, 44 }, TextAlign::Center, L"ビデオオプション" }, // VIDEO OPTIONS
		{ 0, {  72, 31 }, TextAlign::Left,   L"解像度" }, // RESOLUTION
		{ 0, { 109, 31 }, TextAlign::Right,  L"800x600" },
		{ 0, { 110, 31 }, TextAlign::Right,  L"640x480" },
		{ 0, {  90, 31 }, TextAlign::Left,   L"照明の質" }, // LIGHTING QUALITY
		{ 0, {  24, 31 }, TextAlign::Right,  L"高" }, // HIGH
		{ 0, {  25, 31 }, TextAlign::Right,  L"中" }, // MEDIUM
		{ 0, {  26, 31 }, TextAlign::Right,  L"低" }, // LOW
		{ 0, {  91, 31 }, TextAlign::Left,   L"影の調整" }, // BLENDED SHADOWS
		{ 0, {  73, 31 }, TextAlign::Left,   L"遠近法" }, // PERSPECTIVE
		{ 0, {  50, 31 }, TextAlign::Left,   L"輝度" }, // GAMMA
		{ 0, {  88, 31 }, TextAlign::Left,   L"コントラスト" }, // CONTRAST
		{ 1, {  23, 44 }, TextAlign::Center, L"オートマップオプション" }, // AUTOMAP OPTIONS
		{ 0, { 176, 31 }, TextAlign::Left,   L"オートマップのサイズ" }, // AUTOMAP SIZE
		{ 0, {  74, 31 }, TextAlign::Right,  L"全画面" }, // FULL SCREEN
		{ 0, {  92, 31 }, TextAlign::Right,  L"ミニマップ" }, // MINI MAP
		{ 0, { 122, 31 }, TextAlign::Left,   L"透明度上昇" }, // FADE
		{ 0, {  51, 31 }, TextAlign::Right,  L"全体" }, // EVERYTHING
		{ 0, {  87, 31 }, TextAlign::Right,  L"中央のみ" }, // CENTER
		{ 0, {  52, 31 }, TextAlign::Right,  L"オート" }, // AUTO
		{ 0, { 248, 31 }, TextAlign::Left,   L"クリア後は自分中心に戻す" }, // CENTER WHEN CLEARED
		{ 0, { 121, 31 }, TextAlign::Left,   L"パーティ表示" }, // SHOW PARTY
		{ 0, {  96, 31 }, TextAlign::Left,   L"名前表示" }, // SHOW NAMES
		{ 1, { 134, 44 }, TextAlign::Center, L"操作系のコンフィグ（再設定）" }, // CONFIGURE CONTROLS
	} },
	{ LANG_KOR, {
		{ 0, {  58, 38 }, TextAlign::Center, L"옵션" }, // OPTIONS
		{ 0, { 194, 38 }, TextAlign::Center, L"게임 저장 후 종료" }, // SAVE AND EXIT GAME
		{ 0, { 242, 38 }, TextAlign::Center, L"게임으로 돌아가기" }, // RETURN TO GAME
		{ 0, { 126, 38 }, TextAlign::Center, L"이전 메뉴" }, // PREVIOUS MENU
		{ 0, { 117, 34 }, TextAlign::Center, L"이전 메뉴" }, // PREVIOUS MENU
		{ 0, {  27, 34 }, TextAlign::Right,  L"켜기" }, // ON
		{ 0, {  81, 34 }, TextAlign::Right,  L"끄기" }, // OFF
		{ 0, {  28, 34 }, TextAlign::Right,  L"예" }, // YES
		{ 0, {  82, 34 }, TextAlign::Right,  L"아니오" }, // NO
		{ 0, { 155, 38 }, TextAlign::Center, L"사운드 옵션" }, // SOUND OPTIONS
		{ 0, {  83, 34 }, TextAlign::Left,   L"사운드" }, // SOUND
		{ 0, {  84, 34 }, TextAlign::Left,   L"배경음" }, // MUSIC
		{ 0, { 119, 34 }, TextAlign::Left,   L"3D 사운드" }, // 3D SOUND
		{ 0, {  85, 34 }, TextAlign::Left,   L"효과음" }, // ENVIRONMENTAL EFFECTS
		{ 0, { 153, 34 }, TextAlign::Left,   L"3D 음향효과" }, // 3D BIAS
		{ 0, { 118, 34 }, TextAlign::Left,   L"NPC 음성" }, // NPC SPEECH
		{ 0, { 144, 34 }, TextAlign::Right,  L"음성과 글자" }, // AUDIO AND TEXT
		{ 0, {  86, 34 }, TextAlign::Right,  L"음성만" }, // AUDIO ONLY
		{ 0, {  87, 34 }, TextAlign::Right,  L"텍스트만" }, // TEXT ONLY
		{ 0, { 156, 38 }, TextAlign::Center, L"비디오 옵션" }, // VIDEO OPTIONS
		{ 0, {  88, 34 }, TextAlign::Left,   L"해상도" }, // RESOLUTION
		{ 0, { 109, 34 }, TextAlign::Right,  L"800x600" },
		{ 0, { 110, 34 }, TextAlign::Right,  L"640x480" },
		{ 0, { 171, 34 }, TextAlign::Left,   L"광원효과 화질" }, // LIGHTING QUALITY
		{ 0, {  54, 34 }, TextAlign::Right,  L"높음" }, // HIGH
		{ 0, {  55, 34 }, TextAlign::Right,  L"중간" }, // MEDIUM
		{ 0, {  56, 34 }, TextAlign::Right,  L"낮음" }, // LOW
		{ 0, { 145, 34 }, TextAlign::Left,   L"그림자 효과" }, // BLENDED SHADOWS
		{ 0, { 108, 34 }, TextAlign::Left,   L"시점(원근모드)" }, // PERSPECTIVE
		{ 0, {  57, 34 }, TextAlign::Left,   L"감마" }, // GAMMA
		{ 0, {  58, 34 }, TextAlign::Left,   L"명암(대비)" }, // CONTRAST
		{ 0, { 157, 38 }, TextAlign::Center, L"오토 맵 옵션" }, // AUTOMAP OPTIONS
		{ 0, { 146, 34 }, TextAlign::Left,   L"오토 맵 크기" }, // AUTOMAP SIZE
		{ 0, { 120, 34 }, TextAlign::Right,  L"전체 화면" }, // FULL SCREEN
		{ 0, {  90, 34 }, TextAlign::Right,  L"미니 맵" }, // MINI MAP
		{ 0, {  59, 34 }, TextAlign::Left,   L"밝기" }, // FADE
		{ 0, {  60, 34 }, TextAlign::Right,  L"모두" }, // EVERYTHING
		{ 0, {  61, 34 }, TextAlign::Right,  L"중앙" }, // CENTER
		{ 0, {  62, 34 }, TextAlign::Right,  L"자동" }, // AUTO
		{ 0, { 147, 34 }, TextAlign::Left,   L"중심 맞추기" }, // CENTER WHEN CLEARED
		{ 0, { 172, 34 }, TextAlign::Left,   L"파티 보여주기" }, // SHOW PARTY
		{ 0, { 173, 34 }, TextAlign::Left,   L"이름 보여주기" }, // SHOW NAMES
		{ 0, { 158, 38 }, TextAlign::Center, L"단축키 설정" }, // CONFIGURE CONTROLS
	} },
	{ LANG_SIN, {} },
	{ LANG_CHI, {
		{ 0, {  74, 54 }, TextAlign::Center, L"選項" }, // OPTIONS
		{ 1, {   3, 54 }, TextAlign::Center, L"储存并退出游戏" }, // SAVE AND EXIT GAME
		{ 0, { 148, 54 }, TextAlign::Center, L"返回游戏" }, // RETURN TO GAME
		{ 0, { 149, 54 }, TextAlign::Center, L"前一菜单" }, // PREVIOUS MENU
		{ 0, {  96, 34 }, TextAlign::Center, L"前一菜单" }, // PREVIOUS MENU
		{ 0, {  24, 34 }, TextAlign::Right,  L"开" }, // ON
		{ 0, {  25, 34 }, TextAlign::Right,  L"关" }, // OFF
		{ 0, {  26, 34 }, TextAlign::Right,  L"是" }, // YES
		{ 0, {  27, 34 }, TextAlign::Right,  L"否" }, // NO
		{ 0, { 150, 54 }, TextAlign::Center, L"声音选项" }, // SOUND OPTIONS
		{ 0, {  48, 34 }, TextAlign::Left,   L"声音" }, // SOUND
		{ 0, {  49, 34 }, TextAlign::Left,   L"音乐" }, // MUSIC
		{ 0, {  80, 34 }, TextAlign::Left,   L"3D音效" }, // 3D SOUND
		{ 0, {  97, 34 }, TextAlign::Left,   L"环场效果" }, // ENVIRONMENTAL EFFECTS
		{ 0, {  81, 34 }, TextAlign::Left,   L"3D偏向" }, // 3D BIAS
		{ 0, {  95, 34 }, TextAlign::Left,   L"NPC语音" }, // NPC SPEECH
		{ 0, { 120, 34 }, TextAlign::Right,  L"语音和文字" }, // AUDIO AND TEXT
		{ 0, {  98, 34 }, TextAlign::Right,  L"仅有语音" }, // AUDIO ONLY
		{ 0, {  99, 34 }, TextAlign::Right,  L"仅有文字" }, // TEXT ONLY
		{ 0, { 151, 54 }, TextAlign::Center, L"影像选项" }, // VIDEO OPTIONS
		{ 0, {  72, 34 }, TextAlign::Left,   L"分辨率" }, // RESOLUTION
		{ 0, {  71, 34 }, TextAlign::Right,  L"800x600" },
		{ 0, {  77, 34 }, TextAlign::Right,  L"640x480" },
		{ 0, { 100, 34 }, TextAlign::Left,   L"打光品质" }, // LIGHTING QUALITY
		{ 0, {  28, 34 }, TextAlign::Right,  L"高" }, // HIGH
		{ 0, {  29, 34 }, TextAlign::Right,  L"中" }, // MEDIUM
		{ 0, {  30, 34 }, TextAlign::Right,  L"低" }, // LOW
		{ 0, { 101, 34 }, TextAlign::Left,   L"混合影子" }, // BLENDED SHADOWS
		{ 0, {  50, 34 }, TextAlign::Left,   L"透视" }, // PERSPECTIVE
		{ 0, {  51, 34 }, TextAlign::Left,   L"亮度" }, // GAMMA
		{ 0, {  52, 34 }, TextAlign::Left,   L"对比度" }, // CONTRAST
		{ 0, { 222, 54 }, TextAlign::Center, L"自动地图选项" }, // AUTOMAP OPTIONS
		{ 0, { 168, 34 }, TextAlign::Left,   L"自动地图大小" }, // AUTOMAP SIZE
		{ 0, {  73, 34 }, TextAlign::Right,  L"全屏" }, // FULL SCREEN
		{ 0, { 102, 34 }, TextAlign::Right,  L"小型地图" }, // MINI MAP
		{ 0, {  53, 34 }, TextAlign::Left,   L"淡化" }, // FADE
		{ 0, {  54, 34 }, TextAlign::Right,  L"所有" }, // EVERYTHING
		{ 0, {  55, 34 }, TextAlign::Right,  L"中间" }, // CENTER
		{ 0, {  56, 34 }, TextAlign::Right,  L"自动" }, // AUTO
		{ 0, { 121, 34 }, TextAlign::Left,   L"清除时置中" }, // CENTER WHEN CLEARED
		{ 0, { 103, 34 }, TextAlign::Left,   L"显示队伍" }, // SHOW PARTY
		{ 0, { 104, 34 }, TextAlign::Left,   L"显示名称" }, // SHOW NAMES
		{ 0, { 152, 54 }, TextAlign::Center, L"设定控制" }, // CONFIGURE CONTROLS
	} },
	{ LANG_POL, {
		{ 0, {  91, 37 }, TextAlign::Center, L"OPCJE" }, // OPTIONS
		{ 1, {  87, 37 }, TextAlign::Center, L"ZAPIS I WYJŚCIE Z GRY" }, // SAVE AND EXIT GAME
		{ 1, {  10, 37 }, TextAlign::Center, L"POWRÓT DO GRY" }, // RETURN TO GAME
		{ 1, {  30, 37 }, TextAlign::Center, L"POPRZEDNIE MENU" }, // PREVIOUS MENU
		{ 0, { 204, 29 }, TextAlign::Center, L"POPRZEDNIE MENU" }, // PREVIOUS MENU
		{ 0, {  39, 29 }, TextAlign::Right,  L"WŁ." }, // ON
		{ 0, {  54, 29 }, TextAlign::Right,  L"WYŁ." }, // OFF
		{ 0, {  42, 29 }, TextAlign::Right,  L"TAK" }, // YES
		{ 0, {  34, 29 }, TextAlign::Right,  L"NIE" }, // NO
		{ 1, {  33, 37 }, TextAlign::Center, L"OPCJE DŹWIĘKOWE" }, // SOUND OPTIONS
		{ 0, {  82, 29 }, TextAlign::Left,   L"DŹWIĘK" }, // SOUND
		{ 0, {  90, 29 }, TextAlign::Left,   L"MUZYKA" }, // MUSIC
		{ 0, { 118, 29 }, TextAlign::Left,   L"DŹWIĘK 3D" }, // 3D SOUND
		{ 1, {   4, 29 }, TextAlign::Left,   L"EFEKTY SRODOWISKOWE" }, // ENVIRONMENTAL EFFECTS
		{ 0, { 117, 29 }, TextAlign::Left,   L"BALANS 3D" }, // 3D BIAS
		{ 0, { 180, 29 }, TextAlign::Left,   L"WYPOWIEDZI BN" }, // NPC SPEECH
		{ 0, { 154, 29 }, TextAlign::Right,  L"GŁOSY I TEKST" }, // AUDIO AND TEXT
		{ 0, { 144, 29 }, TextAlign::Right,  L"TYLKO GŁOSY" }, // AUDIO ONLY
		{ 0, { 141, 29 }, TextAlign::Right,  L"TYLKO TEKST" }, // TEXT ONLY
		{ 1, {   7, 37 }, TextAlign::Center, L"OPCJE GRAFICZNE" }, // VIDEO OPTIONS
		{ 0, { 168, 29 }, TextAlign::Left,   L"ROZDZIELCZOŚĆ" }, // RESOLUTION
		{ 0, { 115, 29 }, TextAlign::Right,  L"800x600" },
		{ 0, { 105, 29 }, TextAlign::Right,  L"640x480" },
		{ 0, { 226, 29 }, TextAlign::Left,   L"JAKOŚĆ OŚWIETLENIA" }, // LIGHTING QUALITY
		{ 0, {  91, 29 }, TextAlign::Right,  L"WYSOKA" }, // HIGH
		{ 0, {  92, 29 }, TextAlign::Right,  L"ŚREDNIA" }, // MEDIUM
		{ 0, {  61, 29 }, TextAlign::Right,  L"NISKA" }, // LOW
		{ 0, { 175, 29 }, TextAlign::Left,   L"ROZMYTE CIENIE" }, // BLENDED SHADOWS
		{ 0, { 158, 29 }, TextAlign::Left,   L"PERSPEKTYWA" }, // PERSPECTIVE
		{ 0, {  77, 29 }, TextAlign::Left,   L"GAMMA" },
		{ 0, { 112, 29 }, TextAlign::Left,   L"KONTRAST" }, // CONTRAST
		{ 1, {  24, 37 }, TextAlign::Center, L"OPCJE AUTOMAPY" }, // AUTOMAP OPTIONS
		{ 0, { 236, 29 }, TextAlign::Left,   L"WIELKOŚĆ AUTOMAPY" }, // AUTOMAP SIZE
		{ 0, { 132, 29 }, TextAlign::Right,  L"CAŁY EKRAN" }, // FULL SCREEN
		{ 0, { 108, 29 }, TextAlign::Right,  L"MINIMAPA" }, // MINI MAP
		{ 0, { 134, 29 }, TextAlign::Left,   L"WYGASZENIE" }, // FADE
		{ 0, { 114, 29 }, TextAlign::Right,  L"WSZYSTKO" }, // EVERYTHING
		{ 0, {  83, 29 }, TextAlign::Right,  L"ŚRODEK" }, // CENTER
		{ 0, {  70, 29 }, TextAlign::Right,  L"AUTO" },
		{ 0, { 216, 29 }, TextAlign::Left,   L"AUTOCENTROWANIE" }, // CENTER WHEN CLEARED
		{ 0, { 182, 29 }, TextAlign::Left,   L"POKAŻ DRUŻYNĘ" }, // SHOW PARTY
		{ 0, { 157, 29 }, TextAlign::Left,   L"POKAŻ IMIONA" }, // SHOW NAMES
		{ 1, { 181, 37 }, TextAlign::Center, L"KONFIGURACJA STEROWANIA" }, // CONFIGURE CONTROLS
	} },
	{ LANG_RUS, {
		{ 0, { 160, 36 }, TextAlign::Center, L"ОПЦИИ" },
		{ 0, { 123, 41 }, TextAlign::Center, L"ОПЦИИ" }, // OPTIONS
		{ 1, { 178, 36 }, TextAlign::Center, L"СОХРАНИТЬ И ВЫЙТИ" },
		{ 1, { 157, 41 }, TextAlign::Center, L"СОХРАНИТЬ И ВЫЙТИ" }, // SAVE AND EXIT GAME
		{ 1, { 100, 36 }, TextAlign::Center, L"ВОЗВРАТ В ИГРУ" },
		{ 1, {  91, 41 }, TextAlign::Center, L"ВОЗВРАТ В ИГРУ" }, // RETURN TO GAME
		{ 1, { 117, 36 }, TextAlign::Center, L"ПРЕДЫДУЩЕЕ МЕНЮ" },
		{ 1, { 109, 41 }, TextAlign::Center, L"ПРЕДЫДУЩЕЕ МЕНЮ" }, // PREVIOUS MENU
		{ 0, { 231, 26 }, TextAlign::Center, L"ПРЕДЫДУЩЕЕ МЕНЮ" },
		{ 1, {  14, 30 }, TextAlign::Center, L"ПРЕДЫДУЩЕЕ МЕНЮ" }, // PREVIOUS MENU
		{ 0, {  54, 26 }, TextAlign::Right,  L"ВКЛ." }, // ON
		{ 0, {  76, 26 }, TextAlign::Right,  L"ВЫКЛ." },
		{ 0, {  79, 30 }, TextAlign::Right,  L"ВЫКЛ." }, // OFF
		{ 0, {  51, 26 }, TextAlign::Right,  L"ДА" },
		{ 0, {  43, 30 }, TextAlign::Right,  L"ДА" }, // YES
		{ 0, {  53, 26 }, TextAlign::Right,  L"НЕТ" },
		{ 0, {  50, 30 }, TextAlign::Right,  L"НЕТ" }, // NO
		{ 1, {  53, 36 }, TextAlign::Center, L"ОПЦИИ ЗВУКА" },
		{ 1, {   3, 41 }, TextAlign::Center, L"ОПЦИИ ЗВУКА" }, // SOUND OPTIONS
		{ 0, {  95, 26 }, TextAlign::Left,   L"ЗВУК" },
		{ 0, {  61, 30 }, TextAlign::Left,   L"ЗВУК" }, // SOUND
		{ 0, { 138, 26 }, TextAlign::Left,   L"МУЗЫКА" },
		{ 0, { 111, 30 }, TextAlign::Left,   L"МУЗЫКА" }, // MUSIC
		{ 0, { 144, 26 }, TextAlign::Left,   L"3D ЗВУК" },
		{ 0, { 110, 30 }, TextAlign::Left,   L"3D ЗВУК" }, // 3D SOUND
		{ 1, { 106, 26 }, TextAlign::Left,   L"ЭФФЕКТЫ ОКРУЖЕНИЯ" },
		{ 1, {  44, 30 }, TextAlign::Left,   L"ЭФФЕКТЫ ОКРУЖЕНИЯ" }, // ENVIRONMENTAL EFFECTS
		{ 0, { 109, 26 }, TextAlign::Left,   L"3D СМЕЩ." }, // 3D BIAS
		{ 0, { 199, 26 }, TextAlign::Left,   L"РЕЧЬ NPC" },
		{ 0, { 124, 30 }, TextAlign::Left,   L"РЕЧЬ NPC" }, // NPC SPEECH
		{ 1, {   4, 26 }, TextAlign::Right,  L"ЗВУК И ТЕКСТ" },
		{ 0, { 237, 30 }, TextAlign::Right,  L"ЗВУК И ТЕКСТ" }, // AUDIO AND TEXT
		{ 0, { 201, 26 }, TextAlign::Right,  L"ТОЛЬКО ЗВУК" },
		{ 0, { 229, 30 }, TextAlign::Right,  L"ТОЛЬКО ЗВУК" }, // AUDIO ONLY
		{ 0, { 212, 26 }, TextAlign::Right,  L"ТОЛЬКО ТЕКСТ" },
		{ 0, { 216, 30 }, TextAlign::Right,  L"ТОЛЬКО ТЕКСТ" }, // TEXT ONLY
		{ 1, {  39, 36 }, TextAlign::Center, L"ОПЦИИ ВИДЕО" },
		{ 1, {   6, 41 }, TextAlign::Center, L"ОПЦИИ ВИДЕО" }, // VIDEO OPTIONS
		{ 0, { 171, 26 }, TextAlign::Left,   L"РАЗРЕШЕНИЕ" }, // RESOLUTION
		{ 0, { 135, 30 }, TextAlign::Right,  L"800x600" },
		{ 0, { 123, 30 }, TextAlign::Right,  L"640x480" },
		{ 1, {  68, 26 }, TextAlign::Left,   L"КАЧЕСТВО ОСВЕЩЕНИЯ" },
		{ 1, {  57, 30 }, TextAlign::Left,   L"КАЧЕСТВО ОСВЕЩЕНИЯ" }, // LIGHTING QUALITY
		{ 0, {  96, 26 }, TextAlign::Right,  L"ВЫСОК." },
		{ 0, {  96, 30 }, TextAlign::Right,  L"ВЫСОК." }, // HIGH
		{ 0, { 108, 26 }, TextAlign::Right,  L"СРЕДН." },
		{ 0, {  90, 30 }, TextAlign::Right,  L"СРЕДН." }, // MEDIUM
		{ 0, {  66, 26 }, TextAlign::Right,  L"НИЗК." },
		{ 0, {  71, 30 }, TextAlign::Right,  L"НИЗК." }, // LOW
		{ 1, {  72, 30 }, TextAlign::Left,   L"СГЛАЖЕННЫЕ ТЕНИ" }, // BLENDED SHADOWS
		{ 0, { 174, 26 }, TextAlign::Left,   L"ПЕРСПЕКТИВА" }, // PERSPECTIVE
		{ 0, { 105, 26 }, TextAlign::Left,   L"ГАММА" },
		{ 0, { 101, 30 }, TextAlign::Left,   L"ГАММА" }, // GAMMA
		{ 0, { 151, 26 }, TextAlign::Left,   L"КОНТРАСТ" }, // CONTRAST
		{ 1, { 154, 36 }, TextAlign::Center, L"ОПЦИИ АВТОКАРТЫ" },
		{ 1, { 120, 41 }, TextAlign::Center, L"ОПЦИИ АВТОКАРТЫ" }, // AUTOMAP OPTIONS
		{ 0, { 208, 26 }, TextAlign::Left,   L"РАЗМЕР КАРТЫ" }, // AUTOMAP SIZE
		{ 0, { 163, 26 }, TextAlign::Right,  L"ВЕСЬ ЭКРАН" }, // FULL SCREEN
		{ 0, { 170, 26 }, TextAlign::Right,  L"МИНИКАРТА" }, // MINI MAP
		{ 0, { 209, 26 }, TextAlign::Left,   L"ПРОЗРАЧНОСТЬ" },
		{ 0, { 204, 30 }, TextAlign::Left,   L"ПРОЗРАЧНОСТЬ" }, // FADE
		{ 0, { 173, 26 }, TextAlign::Right,  L"ВСЁ" },
		{ 0, {  45, 30 }, TextAlign::Right,  L"ВСЁ" }, // EVERYTHING
		{ 0, { 104, 26 }, TextAlign::Right,  L"ЦЕНТР" },
		{ 0, {  81, 30 }, TextAlign::Right,  L"ЦЕНТР" }, // CENTER
		{ 0, {  77, 26 }, TextAlign::Right,  L"АВТО" }, // AUTO
		{ 1, { 139, 26 }, TextAlign::Left,   L"НА ЦЕНТР ПОСЛЕ ОЧИСТКИ" },
		{ 1, {  70, 30 }, TextAlign::Left,   L"НА ЦЕНТР ПОСЛЕ ОЧИСТКИ" }, // CENTER WHEN CLEARED
		{ 0, { 206, 26 }, TextAlign::Left,   L"ПОКАЗ ГРУППЫ" },
		{ 1, {  53, 30 }, TextAlign::Left,   L"ПОКАЗ ГРУППЫ" }, // SHOW PARTY
		{ 0, { 190, 26 }, TextAlign::Left,   L"ПОКАЗ ИМЁН" },
		{ 1, {  26, 30 }, TextAlign::Left,   L"ПОКАЗ ИМЁН" }, // SHOW NAMES
		{ 1, { 255, 36 }, TextAlign::Center, L"НАСТРОЙКА УПРАВЛЕНИЯ" },
		{ 1, { 221, 41 }, TextAlign::Center, L"НАСТРОЙКА УПРАВЛЕНИЯ" }, // CONFIGURE CONTROLS
	} },
};

inline std::vector<D2TextInfo> g_death_texts = {
	{ 1, {  20, 36 }, TextAlign::Center, L"You have died" }, // youdiedhardcore.dc6
	{ 1, { 175, 36 }, TextAlign::Center, L"Press ESC to continue" }, // youdiedinst.dc6
	{ 1, {  23, 36 }, TextAlign::Center, L"You lost gold" }, // youdiedsoftcore.dc6
};
inline const std::map<uint32_t, std::vector<D2TextInfo>> g_death_text_others = {
	{ LANG_ESP, {
		{ 0, { 218, 36 }, TextAlign::Center, L"Sie sind gestorben" }, // You have died
		{ 2, {   7, 36 }, TextAlign::Center, L"Presiona ESCAPE para Continuar" }, // Press ESC to continue
		{ 1, {  47, 36 }, TextAlign::Center, L"Perdiste Oro" }, // You lost gold
	} },
	{ LANG_DEU, {
		{ 1, {  75, 36 }, TextAlign::Center, L"You have died" }, // You have died
		{ 2, {  18, 36 }, TextAlign::Center, L"Weiter mit Druck auf ESC" }, // Press ESC to continue
		{ 1, { 200, 36 }, TextAlign::Center, L"Sie haben Gold verloren" }, // You lost gold
	} },
	{ LANG_FRA, {
		{ 1, {  54, 36 }, TextAlign::Center, L"Vous étes mort" }, // You have died
		{ 2, { 124, 36 }, TextAlign::Center, L"Appuyez sur ECH pour continuer" }, // Press ESC to continue
		{ 1, { 220, 36 }, TextAlign::Center, L"Vous avez perdu de l'or" }, // You lost gold
	} },
	{ LANG_POR, {} },
	{ LANG_ITA, {
		{ 0, { 181, 36 }, TextAlign::Center, L"Sei morto" }, // You have died
		{ 1, { 233, 36 }, TextAlign::Center, L"Premi ESC per continuare" }, // Press ESC to continue
		{ 1, {  32, 36 }, TextAlign::Center, L"Hai perso l'oro" }, // You lost gold
	} },
	{ LANG_JPN, {
		{ 0, { 147, 44 }, TextAlign::Center, L"汝は死せり" }, // You have died
		{ 0, { 210, 44 }, TextAlign::Center, L"ESCキーを押せ" }, // Press ESC to continue
		{ 0, { 217, 44 }, TextAlign::Center, L"ゴールドを失った" }, // You lost gold
	} },
	{ LANG_KOR, {
		{ 0, { 253, 38 }, TextAlign::Center, L"당신은 죽었습니다" }, // You have died
		{ 2, {   3, 38 }, TextAlign::Center, L"계속하려면 ESC 키를 누르세요" }, // Press ESC to continue
		{ 1, {  94, 38 }, TextAlign::Center, L"골드를 잃었습니다" }, // You lost gold
	} },
	{ LANG_SIN, {} },
	{ LANG_CHI, {
		{ 0, { 111, 54 }, TextAlign::Center, L"你已经死亡" }, // You have died
		{ 0, { 248, 54 }, TextAlign::Center, L"按ESC继续" }, // Press ESC to continue
		{ 1, {  40, 54 }, TextAlign::Center, L"你丢失了一些金币" }, // You lost gold
	} },
	{ LANG_POL, {
		{ 1, { 117, 37 }, TextAlign::Center, L"TWOJA POSTAĆ ZGINĘŁA" }, // You have died
		{ 2, {   4, 37 }, TextAlign::Center, L"WCIŚNIJ ESC, ABY KONTYNUOWAĆ" }, // Press ESC to continue
		{ 1, { 250, 37 }, TextAlign::Center, L"TWOJA POSTAĆ STRACIŁA ZŁOTO" }, // You lost gold
	} },
	{ LANG_RUS, {
		{ 1, {  20, 36 }, TextAlign::Center, L"ВЫ ПОГИБЛИ" },
		{ 0, { 202, 41 }, TextAlign::Center, L"ВЫ ПОГИБЛИ" }, // You have died
		{ 1, { 245, 36 }, TextAlign::Center, L"Нажмите ESC для продолжения" },
		{ 2, {  73, 41 }, TextAlign::Center, L"Нажмите ESC для продолжения" }, // Press ESC to continue
		{ 1, { 106, 36 }, TextAlign::Center, L"Вы потеряли золото" },
		{ 1, { 124, 41 }, TextAlign::Center, L"Вы потеряли золото" }, // You lost gold
	} },
};

inline void initLangVariables(uint32_t lang_id)
{
	switch (lang_id) {
		case LANG_JPN: case LANG_KOR: case LANG_CHI:
			g_font_sizes = {
				{  1, { 0, 11.6f, 1.02f, 1.4f,  0.102f, } }, // All Normal texts
				{  9, { 1, 14.2f, 1.12f, 1.4f,  0.102f, L'\x02' } }, // Menu Button label
				{ 10, { 1, 10.0f, 1.12f, 1.4f,  0.102f, L'\x02' } }, // Menu Button label (smaller)
				{ 11, { 0, 11.2f, 1.06f, 1.4f,  0.082f, } }, // ?
				{ 99, { 0, 10.0f, 1.02f, 1.4f,  0.102f, } }, // FramedText in small size //Custom

				{  6, { 0,  9.0f, 1.02f, 1.0f,  0.082f } }, // All Small texts
				{ 98, { 0,  9.4f, 1.02f, 1.0f,  0.082f } }, // Custom

				{  0, { 0, 10.8f, 1.02f, 1.0f,  0.115f } }, // Char name on stats panel
				{  4, { 0, 10.2f, 1.04f, 1.0f,  0.115f } }, // Skill tree level number 2digits
				{  5, { 1, 11.0f, 1.12f, 1.0f,  0.115f } }, // Menu screen copyright text
				{  8, { 1, 11.6f, 1.14f, 1.0f,  0.220f } }, // Talking Text
				{ 12, { 0,  9.0f, 1.02f, 1.0f,  0.115f } }, // ?
				{ 13, { 0, 11.0f, 1.02f, 1.1f,  0.112f } }, // Message, Shrine, Keybind config

				{  2, { 1, 24.0f, 1.12f, 1.0f,  0.102f, L'\x01' } }, // Char create screen title, Resolution text eg:800x600
				{  3, { 1, 30.0f, 1.12f, 1.0f,  0.102f, L'\x01' } }, // Char selection title (selected char name)
				{  7, { 1, 20.9f, 1.20f, 1.0f,  0.102f, L'\x01' } }, // Menu screen popup title
			};
			if (lang_id == LANG_JPN)
				g_font_sizes[8] = { 1, 12.4f, 1.12f, 1.1f, 0.300f }; // Talking Text
			else if (lang_id == LANG_CHI)
				g_font_sizes[8] = { 1, 11.0f, 1.12f, 1.1f, 0.180f }; // Talking Text
		break;
		case LANG_POL:
			g_font_sizes[8] = { 2, 11.0f, 1.14f, 1.0f, 0.03f }; // Talking Text
		break;
		case LANG_RUS:
			g_font_sizes[ 4] = { 2, 11.2f, 1.14f, 1.0f, 0.115f }; // Skill tree level number 2digits
			g_font_sizes[ 5] = { 2, 12.8f, 1.14f, 1.0f, 0.115f }; // Menu screen copyright text
			g_font_sizes[ 8] = { 2, 12.8f, 1.14f, 1.0f, 0.185f }; // Talking Text
			g_font_sizes[13] = { 2, 12.2f, 1.14f, 1.0f, 0.120f }; // Message, Shrine, Keybind config
		break;
	}
	if (lang_id != LANG_ENG) {
		g_options_texts = g_options_text_others.at(lang_id);
		g_death_texts = g_death_text_others.at(lang_id);
	}
}
// clang-format on

}