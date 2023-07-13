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

struct D2PopupInfo {
	glm::vec<2, uint16_t> size0;
	glm::vec<2, uint16_t> size1;
};

struct D2TextInfo {
	uint8_t cell_num;
	glm::vec<2, uint16_t> size;
	const wchar_t* str;
	TextAlign align = TextAlign::Center;
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

inline const std::map<uint32_t, wchar_t> g_initial_colors {
	{ 2, L'\x01' }, {  3, L'\x01' }, { 7, L'\x01' },
	{ 9, L'\x02' }, { 10, L'\x02' },
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
	{ { 54, 34 }, { 31, 33 }, 18 }, // CHI
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
	{ 0, { 160, 36 }, L"OPTIONS" }, // options.dc6
	{ 1, { 178, 36 }, L"SAVE AND EXIT GAME" }, // exit.dc6
	{ 1, { 100, 36 }, L"RETURN TO GAME" }, // returntogame.dc6
	{ 1, {  57, 36 }, L"PREVIOUS MENU" }, // previous.dc6
	{ 0, { 231, 26 }, L"PREVIOUS MENU" }, // sprevious.dc6
	{ 0, {  41, 26 }, L"ON", TextAlign::Right }, // smallon.dc6
	{ 0, {  52, 26 }, L"OFF", TextAlign::Right }, // smalloff.dc6
	{ 0, {  51, 26 }, L"YES", TextAlign::Right }, // smallyes.dc6
	{ 0, {  40, 26 }, L"NO", TextAlign::Right }, // smallno.dc6
	{ 1, {  53, 36 }, L"SOUND OPTIONS" }, // soundoptions.dc6
	{ 0, {  95, 26 }, L"SOUND", TextAlign::Left }, // sound.dc6
	{ 0, {  84, 26 }, L"MUSIC", TextAlign::Left }, // music.dc6
	{ 0, { 144, 26 }, L"3D SOUND", TextAlign::Left }, // 3dsound.dc6
	{ 1, { 106, 26 }, L"ENVIRONMENTAL EFFECTS", TextAlign::Left }, // eax.dc6
	{ 0, { 109, 26 }, L"3D BIAS", TextAlign::Left }, // 3dbias.dc6
	{ 0, { 157, 26 }, L"NPC SPEECH", TextAlign::Left }, // npcspeech.dc6
	{ 1, {   4, 26 }, L"AUDIO AND TEXT", TextAlign::Right }, // audiotext.dc6
	{ 0, { 184, 26 }, L"AUDIO ONLY", TextAlign::Right }, // audioonly.dc6
	{ 0, { 169, 26 }, L"TEXT ONLY", TextAlign::Right }, // textonly.dc6
	{ 1, {  39, 36 }, L"VIDEO OPTIONS" }, // videooptions.dc6
	{ 0, { 175, 26 }, L"RESOLUTION", TextAlign::Left }, // resolution.dc6
	{ 0, { 135, 26 }, L"800x600", TextAlign::Right }, // 800x600.dc6
	{ 0, { 123, 26 }, L"640x480", TextAlign::Right }, // 640x480.dc6
	{ 1, {   9, 26 }, L"LIGHTING QUALITY", TextAlign::Left }, // lightquality.dc6
	{ 0, {  57, 26 }, L"HIGH", TextAlign::Right }, // high.dc6
	{ 0, { 108, 26 }, L"MEDIUM", TextAlign::Right }, // medium.dc6
	{ 0, {  66, 26 }, L"LOW", TextAlign::Right }, // low.dc6
	{ 1, {  10, 26 }, L"BLENDED SHADOWS", TextAlign::Left }, // blendshadow.dc6
	{ 0, { 174, 26 }, L"PERSPECTIVE", TextAlign::Left }, // perspective.dc6
	{ 0, { 105, 26 }, L"GAMMA", TextAlign::Left }, // gamma.dc6
	{ 0, { 151, 26 }, L"CONTRAST", TextAlign::Left }, // contrast.dc6
	{ 1, { 122, 36 }, L"AUTOMAP OPTIONS" }, // automapoptions.dc6
	{ 0, { 213, 26 }, L"AUTOMAP SIZE", TextAlign::Left }, // automapmode.dc6
	{ 0, { 180, 26 }, L"FULL SCREEN", TextAlign::Right }, // full.dc6
	{ 0, { 131, 26 }, L"MINI MAP", TextAlign::Right }, // mini.dc6
	{ 0, {  70, 26 }, L"FADE", TextAlign::Left }, // automapfade.dc6
	{ 0, { 173, 26 }, L"EVERYTHING", TextAlign::Right }, // everything.dc6
	{ 0, { 104, 26 }, L"CENTER", TextAlign::Right }, // center.dc6
	{ 0, {  86, 26 }, L"AUTO", TextAlign::Right }, // auto.dc6
	{ 1, {  80, 26 }, L"CENTER WHEN CLEARED", TextAlign::Left }, // automapcenter.dc6
	{ 0, { 194, 26 }, L"SHOW PARTY", TextAlign::Left }, // automapparty.dc6
	{ 0, { 190, 26 }, L"SHOW NAMES", TextAlign::Left }, // automappartynames.dc6
	{ 1, { 167, 36 }, L"CONFIGURE CONTROLS" }, // cfgoptions.dc6
};
inline std::map<uint32_t, std::vector<D2TextInfo>> g_options_text_others = {
	{ LANG_ESP, {
		{ 0, { 175, 36 }, L"OPCIONES" }, // OPTIONS
		{ 1, {  73, 36 }, L"SALVAR Y SALIR" }, // SAVE AND EXIT GAME
		{ 1, { 208, 36 }, L"VOLVER A PARTIDA" }, // RETURN TO GAME
		{ 1, {  65, 36 }, L"MENÚ ANTERIOR" }, // PREVIOUS MENU
		{ 0, { 237, 26 }, L"MENÚ ANTERIOR" }, // PREVIOUS MENU
		{ 0, { 152, 26 }, L"ACTIVADA" }, // ON
		{ 0, {  81, 26 }, L"DESéA" }, // OFF
		{ 0, {  39, 26 }, L"SÍ" }, // YES
		{ 0, {  40, 26 }, L"NO" }, // NO
		{ 1, {  47, 36 }, L"OPCIONES DE SONIDO" }, // SOUND OPTIONS
		{ 0, { 101, 26 }, L"SONIDO" }, // SOUND
		{ 0, { 107, 26 }, L"MÚSICA" }, // MUSIC
		{ 0, { 150, 26 }, L"SONIDO 3D" }, // 3D SOUND
		{ 0, { 222, 26 }, L"EFEC. AMBIENTE" }, // ENVIRONMENTAL EFFECTS
		{ 0, { 108, 26 }, L"3D BIAS" }, // 3D BIAS
		{ 0, { 136, 26 }, L"DIÁLOGOS" }, // NPC SPEECH
		{ 0, { 253, 26 }, L"SONIDO Y TEXTO" }, // AUDIO AND TEXT
		{ 0, { 187, 26 }, L"SÓLO SONIDO" }, // AUDIO ONLY
		{ 0, { 184, 26 }, L"SÓLO TEXTO" }, // TEXT ONLY
		{ 1, {  27, 36 }, L"OPCIONES DE VÍDEO" }, // VIDEO OPTIONS
		{ 0, { 149, 26 }, L"RESOLUCIÓN" }, // RESOLUTION
		{ 0, { 135, 26 }, L"800x600" },
		{ 0, { 123, 26 }, L"640x480" },
		{ 1, { 109, 26 }, L"CALIDAD DE ILUMINACIÓN" }, // LIGHTING QUALITY
		{ 0, {  78, 26 }, L"ALTA" }, // HIGH
		{ 0, {  85, 26 }, L"MEDIA" }, // MEDIUM
		{ 0, {  67, 26 }, L"BAJA" }, // LOW
		{ 0, { 225, 26 }, L"FUND. SOMBRAS" }, // BLENDED SHADOWS
		{ 0, { 180, 26 }, L"PERSPECTIVA" }, // PERSPECTIVE
		{ 0, { 105, 26 }, L"GAMMA" }, // GAMMA
		{ 0, { 151, 26 }, L"CONTRAST" }, // CONTRAST
		{ 1, {  21, 36 }, L"OPCIONES DEL AUTOMAPA" }, // AUTOMAP OPTIONS
		{ 0, { 111, 26 }, L"TAMAÑO" }, // AUTOMAP SIZE
		{ 1, {  14, 26 }, L"PANTALLA COMPLETA" }, // FULL SCREEN
		{ 0, { 122, 26 }, L"MINI MAPA" }, // MINI MAP
		{ 0, { 124, 26 }, L"FUNDIDO" }, // FADE
		{ 0, {  80, 26 }, L"TODO" }, // EVERYTHING
		{ 0, { 110, 26 }, L"CENTRO" }, // CENTER
		{ 0, { 170, 26 }, L"AUTOMÁTICO" }, // AUTO
		{ 1, {  65, 26 }, L"CENTRAR AL DESPEJAR" }, // CENTER WHEN CLEARED
		{ 0, { 254, 26 }, L"MOSTRAR EQUIPO" }, // SHOW PARTY
		{ 1, {  19, 26 }, L"MOSTRAR NOMBRES" }, // SHOW NAMES
		{ 1, { 222, 36 }, L"CONFIGURAR CONTROLES" }, // CONFIGURE CONTROLS
	} },
	{ LANG_DEU, {
		{ 0, { 187, 36 }, L"OPTIONEN" }, // OPTIONS
		{ 1, { 208, 36 }, L"SPEICHERN / SPIEL VERL." }, // SAVE AND EXIT GAME
		{ 1, { 114, 36 }, L"ZURÜCK ZUM SPIEL" }, // RETURN TO GAME
		{ 1, {  30, 36 }, L"VORIGES MENÜ" }, // PREVIOUS MENU
		{ 0, { 211, 26 }, L"VORIGES MENÜ" }, // PREVIOUS MENU
		{ 0, {  42, 26 }, L"EIN" }, // ON
		{ 0, {  59, 26 }, L"AUS" }, // OFF
		{ 0, {  30, 26 }, L"JA" }, // YES
		{ 0, {  61, 26 }, L"NEIN" }, // NO
		{ 1, {  72, 36 }, L"SOUND OPTIONEN" }, // SOUND OPTIONS
		{ 0, {  95, 26 }, L"SOUND" }, // SOUND
		{ 0, {  84, 26 }, L"MUSIK" }, // MUSIC
		{ 0, { 138, 26 }, L"3D SOUND" }, // 3D SOUND
		{ 1, {  21, 26 }, L"UMGEBUNGSEFFEKTE" }, // ENVIRONMENTAL EFFECTS
		{ 0, { 102, 26 }, L"3D BIAS" }, // 3D BIAS
		{ 0, { 176, 26 }, L"NPC SPRACHE" }, // NPC SPEECH
		{ 1, {   5, 26 }, L"AUDIO UND TEXT" }, // AUDIO AND TEXT
		{ 0, { 168, 26 }, L"NUR AUDIO" }, // AUDIO ONLY
		{ 0, { 153, 26 }, L"NUR TEXT" }, // TEXT ONLY
		{ 1, {  75, 36 }, L"GRAFIK OPTIONEN" }, // VIDEO OPTIONS
		{ 0, { 169, 26 }, L"AUFLÖSUNG" }, // RESOLUTION
		{ 0, { 135, 26 }, L"800x600" },
		{ 0, { 123, 26 }, L"640x480" },
		{ 0, { 225, 26 }, L"LICHTQUALITÄT" }, // LIGHTING QUALITY
		{ 0, {  70, 26 }, L"HOCH" }, // HIGH
		{ 0, {  99, 26 }, L"MITTEL" }, // MEDIUM
		{ 0, { 103, 26 }, L"NIEDRIG" }, // LOW
		{ 1, {  17, 26 }, L"SCHATTENVERLAUF" }, // BLENDED SHADOWS
		{ 0, { 173, 26 }, L"PERSPEKTIVE" }, // PERSPECTIVE
		{ 0, { 105, 26 }, L"GAMMA" }, // GAMMA
		{ 0, { 151, 26 }, L"KONTRAST" }, // CONTRAST
		{ 1, { 240, 36 }, L"AUTOM. KARTE OPTIONEN" }, // AUTOMAP OPTIONS
		{ 1, {  69, 26 }, L"GR. D. AUTOM. KARTE" }, // AUTOMAP SIZE
		{ 0, { 108, 26 }, L"VOLLBILD" }, // FULL SCREEN
		{ 0, { 128, 26 }, L"MINIKARTE" }, // MINI MAP
		{ 0, { 174, 26 }, L"AUSBLENDEN" }, // FADE
		{ 0, {  79, 26 }, L"ALLES" }, // EVERYTHING
		{ 0, { 163, 26 }, L"ZENTRIEREN" }, // CENTER
		{ 0, {  86, 26 }, L"AUTO" }, // AUTO
		{ 1, {  87, 26 }, L"ZENTR. NACH LÖSCHUNG" }, // CENTER WHEN CLEARED
		{ 0, { 202, 26 }, L"PARTY ZEIGEN" }, // SHOW PARTY
		{ 0, { 203, 26 }, L"NAMEN ZEIGEN" }, // SHOW NAMES
		{ 2, {  20, 36 }, L"STEUERUNG KONFIGURIEREN" }, // CONFIGURE CONTROLS
	} },
	{ LANG_FRA, {
		{ 0, { 160, 36 }, L"OPTIONS" }, // OPTIONS
		{ 1, { 161, 36 }, L"SAUVER ET QUITTER" }, // SAVE AND EXIT GAME
		{ 1, { 178, 36 }, L"REVENIR A LA PARTIE" }, // RETURN TO GAME
		{ 1, {  74, 36 }, L"MENU PRÉCÉDENT" }, // PREVIOUS MENU
		{ 0, { 246, 26 }, L"MENU PRÉCÉDENT" }, // PREVIOUS MENU
		{ 0, {  52, 26 }, L"OUI" }, // ON
		{ 0, {  59, 26 }, L"NON" }, // OFF
		{ 0, {  53, 26 }, L"OUI" }, // YES
		{ 0, {  60, 26 }, L"NON" }, // NO
		{ 1, {  49, 36 }, L"OPTIONS AUDIO" }, // SOUND OPTIONS
		{ 0, {  54, 26 }, L"SON" }, // SOUND
		{ 0, { 126, 26 }, L"MUSIQUE" }, // MUSIC
		{ 0, { 103, 26 }, L"SON 3D" }, // 3D SOUND
		{ 1, {  87, 26 }, L"EFFETS ENVIRONNEMENT" }, // ENVIRONMENTAL EFFECTS
		{ 0, { 117, 26 }, L"DIST. 3D" }, // 3D BIAS
		{ 0, { 204, 26 }, L"DIALOGUE NPC" }, // NPC SPEECH
		{ 0, { 251, 26 }, L"AUDIO ET TEXTE" }, // AUDIO AND TEXT
		{ 0, {  92, 26 }, L"AUDIO" }, // AUDIO ONLY
		{ 0, {  94, 26 }, L"TEXTE" }, // TEXT ONLY
		{ 1, {  39, 36 }, L"OPTIONS VIDEO" }, // VIDEO OPTIONS
		{ 0, { 173, 26 }, L"RÉSOLUTION" }, // RESOLUTION
		{ 0, { 135, 26 }, L"800x600" },
		{ 0, { 123, 26 }, L"640x480" },
		{ 1, {  25, 26 }, L"OPTIONS ÉCLAIRAGE" }, // LIGHTING QUALITY
		{ 0, {  96, 26 }, L"HAUTE" }, // HIGH
		{ 0, { 133, 26 }, L"MOYENNE" }, // MEDIUM
		{ 0, {  80, 26 }, L"BASSE" }, // LOW
		{ 0, { 247, 26 }, L"OMBRES FONDUES" }, // BLENDED SHADOWS
		{ 0, { 174, 26 }, L"PERSPECTIVE" }, // PERSPECTIVE
		{ 0, { 105, 26 }, L"GAMMA" }, // GAMMA
		{ 0, { 151, 26 }, L"CONTRAST" }, // CONTRAST
		{ 1, { 181, 36 }, L"OPTIONS CARTE-AUTO" }, // AUTOMAP OPTIONS
		{ 0, {  93, 26 }, L"TAILLE" }, // AUTOMAP SIZE
		{ 0, { 177, 26 }, L"PLEIN ÉCRAN" }, // FULL SCREEN
		{ 0, { 149, 26 }, L"MINICARTE" }, // MINI MAP
		{ 0, {  97, 26 }, L"FONDU" }, // FADE
		{ 0, {  84, 26 }, L"TOUT" }, // EVERYTHING
		{ 0, { 104, 26 }, L"CENTRÉ" }, // CENTER
		{ 0, {  86, 26 }, L"AUTO" }, // AUTO
		{ 1, { 100, 26 }, L"CENTRER QUAND EFFACÉ" }, // CENTER WHEN CLEARED
		{ 0, { 256, 26 }, L"AFFICHER COTERIE" }, // SHOW PARTY
		{ 0, { 218, 26 }, L"AFFICHER NOMS" }, // SHOW NAMES
		{ 1, { 226, 36 }, L"CONFIGURER COMMANDES" }, // CONFIGURE CONTROLS
	} },
	{ LANG_POR, {} },
	{ LANG_ITA, {
		{ 0, { 146, 36 }, L"OPZIONI" }, // OPTIONS
		{ 2, {   4, 36 }, L"SALVA ED ESCI DAL GIOCO" }, // SAVE AND EXIT GAME
		{ 1, {  85, 36 }, L"TORNA AL GOCO" }, // RETURN TO GAME
		{ 1, {  98, 36 }, L"MENU PRECEDENTE" }, // PREVIOUS MENU
		{ 1, {   5, 26 }, L"MENU PRECEDENTE" }, // PREVIOUS MENU
		{ 0, {  39, 26 }, L"SÌ" }, // ON
		{ 0, {  40, 26 }, L"NO" }, // OFF
		{ 0, {  41, 26 }, L"SÌ" }, // YES
		{ 0, {  42, 26 }, L"NO" }, // NO
		{ 1, {  35, 36 }, L"OPZIONI AUDIO" }, // SOUND OPTIONS
		{ 0, { 114, 26 }, L"SONORO" }, // SOUND
		{ 0, { 106, 26 }, L"MUSICA" }, // MUSIC
		{ 0, { 141, 26 }, L"AUDIO 3D" }, // 3D SOUND
		{ 1, {  33, 26 }, L"EFFETTI AMBIENTALI" }, // ENVIRONMENTAL EFFECTS
		{ 0, { 173, 26 }, L"DISTORCI 3D" }, // 3D BIAS
		{ 0, { 197, 26 }, L"PARLATO PNG" }, // NPC SPEECH
		{ 0, { 229, 26 }, L"AUDIO E TESTO" }, // AUDIO AND TEXT
		{ 0, { 178, 26 }, L"SOLO AUDIO" }, // AUDIO ONLY
		{ 0, { 176, 26 }, L"SOLO TESTO" }, // TEXT ONLY
		{ 1, {  25, 36 }, L"OPZIONI VIDEO" }, // VIDEO OPTIONS
		{ 0, { 177, 26 }, L"RISOLUZIONE" }, // RESOLUTION
		{ 0, { 135, 26 }, L"800x600" },
		{ 0, { 123, 26 }, L"640x480" },
		{ 1, {  99, 26 }, L"QUALITYÀ ILLUMINAZIONE" }, // LIGHTING QUALITY
		{ 0, {  78, 26 }, L"ALTA" }, // HIGH
		{ 0, {  85, 26 }, L"MEDIA" }, // MEDIUM
		{ 0, {  87, 26 }, L"BASSA" }, // LOW
		{ 0, { 238, 26 }, L"OMBRE SFUMATE" }, // BLENDED SHADOWS
		{ 0, { 189, 26 }, L"PROSPETTIVA" }, // PERSPECTIVE
		{ 0, { 105, 26 }, L"GAMMA" }, // GAMMA
		{ 0, { 172, 26 }, L"CONTRASTO" }, // CONTRAST
		{ 1, { 219, 36 }, L"OPZIONI MAPA AUTOM." }, // AUTOMAP OPTIONS
		{ 0, { 180, 26 }, L"AUTOMAPPA" }, // AUTOMAP SIZE
		{ 0, { 240, 26 }, L"SCHERMO INTERO" }, // FULL SCREEN
		{ 0, { 151, 26 }, L"MINIMAPPA" }, // MINI MAP
		{ 0, { 184, 26 }, L"DISSOLVENZA" }, // FADE
		{ 0, { 104, 26 }, L"TUTTO" }, // EVERYTHING
		{ 0, { 111, 26 }, L"CENTRA" }, // CENTER
		{ 0, { 198, 26 }, L"AUTOMATICA" }, // AUTO
		{ 1, { 102, 26 }, L"CENTRA QUANDO LIBERO" }, // CENTER WHEN CLEARED
		{ 0, { 241, 26 }, L"MOSTRA GRUPPO" }, // SHOW PARTY
		{ 0, { 203, 26 }, L"MOSTRA NOMI" }, // SHOW NAMES
		{ 1, { 188, 36 }, L"CONFIGURA CONTROLLI" }, // CONFIGURE CONTROLS
	} },
	{ LANG_JPN, {
		{ 0, { 125, 44 }, L"オプション" }, // OPTIONS
		{ 1, {  60, 44 }, L"セーブしてゲームを終了" }, // SAVE AND EXIT GAME
		{ 0, { 171, 44 }, L"ゲームに戻る" }, // RETURN TO GAME
		{ 0, { 196, 44 }, L"前のメニューへ" }, // PREVIOUS MENU
		{ 0, { 134, 31 }, L"前のメニューへ" }, // PREVIOUS MENU
		{ 0, {  33, 31 }, L"ON" }, // ON
		{ 0, {  48, 31 }, L"OFF" }, // OFF
		{ 0, {  39, 31 }, L"はい" }, // YES
		{ 0, {  57, 31 }, L"いいえ" }, // NO
		{ 0, { 221, 44 }, L"サウンドオプション" }, // SOUND OPTIONS
		{ 0, {  66, 31 }, L"サウンド" }, // SOUND
		{ 0, {  49, 31 }, L"音楽" }, // MUSIC
		{ 0, {  98, 31 }, L"3Dサウンド" }, // 3D SOUND
		{ 0, { 120, 31 }, L"環境効果音" }, // ENVIRONMENTAL EFFECTS
		{ 0, { 106, 31 }, L"3Dバイアス" }, // 3D BIAS
		{ 0, { 113, 31 }, L"NPCの会話" }, // NPC SPEECH
		{ 0, { 132, 31 }, L"音声とテキスト" }, // AUDIO AND TEXT
		{ 0, {  86, 31 }, L"音声のみ" }, // AUDIO ONLY
		{ 0, { 107, 31 }, L"テキストのみ" }, // TEXT ONLY
		{ 0, { 212, 44 }, L"ビデオオプション" }, // VIDEO OPTIONS
		{ 0, {  72, 31 }, L"解像度" }, // RESOLUTION
		{ 0, { 109, 31 }, L"800x600" },
		{ 0, { 110, 31 }, L"640x480" },
		{ 0, {  90, 31 }, L"照明の質" }, // LIGHTING QUALITY
		{ 0, {  24, 31 }, L"高" }, // HIGH
		{ 0, {  25, 31 }, L"中" }, // MEDIUM
		{ 0, {  26, 31 }, L"低" }, // LOW
		{ 0, {  91, 31 }, L"影の調整" }, // BLENDED SHADOWS
		{ 0, {  73, 31 }, L"遠近法" }, // PERSPECTIVE
		{ 0, {  50, 31 }, L"輝度" }, // GAMMA
		{ 0, {  88, 31 }, L"コントラスト" }, // CONTRAST
		{ 1, {  23, 44 }, L"オートマップオプション" }, // AUTOMAP OPTIONS
		{ 0, { 176, 31 }, L"オートマップのサイズ" }, // AUTOMAP SIZE
		{ 0, {  74, 31 }, L"全画面" }, // FULL SCREEN
		{ 0, {  92, 31 }, L"ミニマップ" }, // MINI MAP
		{ 0, { 122, 31 }, L"透明度上昇" }, // FADE
		{ 0, {  51, 31 }, L"全体" }, // EVERYTHING
		{ 0, {  87, 31 }, L"中央のみ" }, // CENTER
		{ 0, {  52, 31 }, L"オート" }, // AUTO
		{ 0, { 248, 31 }, L"クリア後は自分中心に戻す" }, // CENTER WHEN CLEARED
		{ 0, { 121, 31 }, L"パーティ表示" }, // SHOW PARTY
		{ 0, {  96, 31 }, L"名前表示" }, // SHOW NAMES
		{ 1, { 134, 44 }, L"操作系のコンフィグ（再設定）" }, // CONFIGURE CONTROLS
	} },
	{ LANG_KOR, {
		{ 0, {  58, 38 }, L"옵션" }, // OPTIONS
		{ 0, { 194, 38 }, L"게임 저장 후 종료" }, // SAVE AND EXIT GAME
		{ 0, { 242, 38 }, L"게임으로 돌아가기" }, // RETURN TO GAME
		{ 0, { 126, 38 }, L"이전 메뉴" }, // PREVIOUS MENU
		{ 0, { 117, 34 }, L"이전 메뉴" }, // PREVIOUS MENU
		{ 0, {  27, 34 }, L"켜기" }, // ON
		{ 0, {  81, 34 }, L"끄기" }, // OFF
		{ 0, {  28, 34 }, L"예" }, // YES
		{ 0, {  82, 34 }, L"아니오" }, // NO
		{ 0, { 155, 38 }, L"사운드 옵션" }, // SOUND OPTIONS
		{ 0, {  83, 34 }, L"사운드" }, // SOUND
		{ 0, {  84, 34 }, L"배경음" }, // MUSIC
		{ 0, { 119, 34 }, L"3D 사운드" }, // 3D SOUND
		{ 0, {  85, 34 }, L"효과음" }, // ENVIRONMENTAL EFFECTS
		{ 0, { 153, 34 }, L"3D 음향효과" }, // 3D BIAS
		{ 0, { 118, 34 }, L"NPC 음성" }, // NPC SPEECH
		{ 0, { 144, 34 }, L"음성과 글자" }, // AUDIO AND TEXT
		{ 0, {  86, 34 }, L"음성만" }, // AUDIO ONLY
		{ 0, {  87, 34 }, L"텍스트만" }, // TEXT ONLY
		{ 0, { 156, 38 }, L"비디오 옵션" }, // VIDEO OPTIONS
		{ 0, {  88, 34 }, L"해상도" }, // RESOLUTION
		{ 0, { 109, 34 }, L"800x600" },
		{ 0, { 110, 34 }, L"640x480" },
		{ 0, { 171, 34 }, L"광원효과 화질" }, // LIGHTING QUALITY
		{ 0, {  54, 34 }, L"높음" }, // HIGH
		{ 0, {  55, 34 }, L"중간" }, // MEDIUM
		{ 0, {  56, 34 }, L"낮음" }, // LOW
		{ 0, { 145, 34 }, L"그림자 효과" }, // BLENDED SHADOWS
		{ 0, { 108, 34 }, L"시점(원근모드)" }, // PERSPECTIVE
		{ 0, {  57, 34 }, L"감마" }, // GAMMA
		{ 0, {  58, 34 }, L"명암(대비)" }, // CONTRAST
		{ 0, { 157, 38 }, L"오토 맵 옵션" }, // AUTOMAP OPTIONS
		{ 0, { 146, 34 }, L"오토 맵 크기" }, // AUTOMAP SIZE
		{ 0, { 120, 34 }, L"전체 화면" }, // FULL SCREEN
		{ 0, {  90, 34 }, L"미니 맵" }, // MINI MAP
		{ 0, {  59, 34 }, L"밝기" }, // FADE
		{ 0, {  60, 34 }, L"모두" }, // EVERYTHING
		{ 0, {  61, 34 }, L"중앙" }, // CENTER
		{ 0, {  62, 34 }, L"자동" }, // AUTO
		{ 0, { 147, 34 }, L"중심 맞추기" }, // CENTER WHEN CLEARED
		{ 0, { 172, 34 }, L"파티 보여주기" }, // SHOW PARTY
		{ 0, { 173, 34 }, L"이름 보여주기" }, // SHOW NAMES
		{ 0, { 158, 38 }, L"단축키 설정" }, // CONFIGURE CONTROLS
	} },
	{ LANG_SIN, {
		{ 0, {  74, 54 }, L"选项" }, // OPTIONS
		{ 1, {   3, 54 }, L"保存并退出" }, // SAVE AND EXIT GAME
		{ 0, { 148, 54 }, L"返回游戏" }, // RETURN TO GAME
		{ 0, { 149, 54 }, L"前一个菜单" }, // PREVIOUS MENU
		{ 0, {  96, 34 }, L"前一个菜单" }, // PREVIOUS MENU
		{ 0, {  24, 34 }, L"开启" }, // ON
		{ 0, {  25, 34 }, L"关闭" }, // OFF
		{ 0, {  26, 34 }, L"是" }, // YES
		{ 0, {  27, 34 }, L"否" }, // NO
		{ 0, { 150, 54 }, L"音效选项" }, // SOUND OPTIONS
		{ 0, {  48, 34 }, L"音效" }, // SOUND
		{ 0, {  49, 34 }, L"音乐" }, // MUSIC
		{ 0, {  80, 34 }, L"3D音效" }, // 3D SOUND
		{ 0, {  97, 34 }, L"环场效果" }, // ENVIRONMENTAL EFFECTS
		{ 0, {  81, 34 }, L"3D偏向" }, // 3D BIAS
		{ 0, {  95, 34 }, L"NPC语音" }, // NPC SPEECH
		{ 0, { 120, 34 }, L"语音和文本" }, // AUDIO AND TEXT 音效和文本
		{ 0, {  98, 34 }, L"仅语音" }, // AUDIO ONLY 仅音效
		{ 0, {  99, 34 }, L"仅文本" }, // TEXT ONLY
		{ 0, { 151, 54 }, L"视频选项" }, // VIDEO OPTIONS
		{ 0, {  72, 34 }, L"分辨率" }, // RESOLUTION
		{ 0, {  71, 34 }, L"800x600" },
		{ 0, {  77, 34 }, L"640x480" },
		{ 0, { 100, 34 }, L"光照质量" }, // LIGHTING QUALITY
		{ 0, {  28, 34 }, L"高" }, // HIGH
		{ 0, {  29, 34 }, L"中" }, // MEDIUM
		{ 0, {  30, 34 }, L"低" }, // LOW
		{ 0, { 101, 34 }, L"混合阴影" }, // BLENDED SHADOWS
		{ 0, {  50, 34 }, L"远景" }, // PERSPECTIVE
		{ 0, {  51, 34 }, L"亮度" }, // GAMMA
		{ 0, {  52, 34 }, L"对比度" }, // CONTRAST
		{ 0, { 222, 54 }, L"透视地图选项" }, // AUTOMAP OPTIONS
		{ 0, { 168, 34 }, L"透视地图尺寸" }, // AUTOMAP SIZE
		{ 0, {  73, 34 }, L"全屏" }, // FULL SCREEN
		{ 0, { 102, 34 }, L"小地图" }, // MINI MAP
		{ 0, {  53, 34 }, L"渐隐" }, // FADE
		{ 0, {  54, 34 }, L"全部" }, // EVERYTHING
		{ 0, {  55, 34 }, L"居中" }, // CENTER
		{ 0, {  56, 34 }, L"自动" }, // AUTO
		{ 0, { 121, 34 }, L"打开时居中" }, // CENTER WHEN CLEARED
		{ 0, { 103, 34 }, L"显示队伍" }, // SHOW PARTY
		{ 0, { 104, 34 }, L"显示名字" }, // SHOW NAMES
		{ 0, { 152, 54 }, L"操作设置" }, // CONFIGURE CONTROLS 设置控制
	} },
	{ LANG_CHI, {
		{ 0, {  74, 54 }, L"選項" }, // OPTIONS
		{ 1, {   3, 54 }, L"儲存並離開遊戲" }, // SAVE AND EXIT GAME
		{ 0, { 148, 54 }, L"返回遊戲" }, // RETURN TO GAME
		{ 0, { 149, 54 }, L"前一選單" }, // PREVIOUS MENU
		{ 0, {  96, 34 }, L"前一選單" }, // PREVIOUS MENU
		{ 0, {  24, 34 }, L"開" }, // ON
		{ 0, {  25, 34 }, L"關" }, // OFF
		{ 0, {  26, 34 }, L"是" }, // YES
		{ 0, {  27, 34 }, L"不" }, // NO
		{ 0, { 150, 54 }, L"聲音選項" }, // SOUND OPTIONS
		{ 0, {  48, 34 }, L"聲音" }, // SOUND
		{ 0, {  49, 34 }, L"音樂" }, // MUSIC
		{ 0, {  80, 34 }, L"3D聲音" }, // 3D SOUND
		{ 0, {  97, 34 }, L"環場效果" }, // ENVIRONMENTAL EFFECTS
		{ 0, {  81, 34 }, L"3D偏向" }, // 3D BIAS
		{ 0, {  95, 34 }, L"NPC語音" }, // NPC SPEECH
		{ 0, { 120, 34 }, L"語音及文字" }, // AUDIO AND TEXT
		{ 0, {  98, 34 }, L"只有語音" }, // AUDIO ONLY
		{ 0, {  99, 34 }, L"只有文字" }, // TEXT ONLY
		{ 0, { 151, 54 }, L"影像選項" }, // VIDEO OPTIONS
		{ 0, {  72, 34 }, L"解析度" }, // RESOLUTION
		{ 0, {  71, 34 }, L"800x600" },
		{ 0, {  77, 34 }, L"640x480" },
		{ 0, { 100, 34 }, L"打光品質" }, // LIGHTING QUALITY
		{ 0, {  28, 34 }, L"高" }, // HIGH
		{ 0, {  29, 34 }, L"中" }, // MEDIUM
		{ 0, {  30, 34 }, L"低" }, // LOW
		{ 0, { 101, 34 }, L"混合影子" }, // BLENDED SHADOWS
		{ 0, {  50, 34 }, L"透視" }, // PERSPECTIVE
		{ 0, {  51, 34 }, L"亮度" }, // GAMMA
		{ 0, {  52, 34 }, L"對比" }, // CONTRAST
		{ 0, { 222, 54 }, L"自動地圖選項" }, // AUTOMAP OPTIONS
		{ 0, { 168, 34 }, L"自動地圖的大小" }, // AUTOMAP SIZE
		{ 0, {  73, 34 }, L"全畫面" }, // FULL SCREEN
		{ 0, { 102, 34 }, L"小型地圖" }, // MINI MAP
		{ 0, {  53, 34 }, L"淡化" }, // FADE
		{ 0, {  54, 34 }, L"所有" }, // EVERYTHING
		{ 0, {  55, 34 }, L"中間" }, // CENTER
		{ 0, {  56, 34 }, L"自動" }, // AUTO
		{ 0, { 121, 34 }, L"清除時置中" }, // CENTER WHEN CLEARED
		{ 0, { 103, 34 }, L"顯示隊伍" }, // SHOW PARTY
		{ 0, { 104, 34 }, L"顯示名稱" }, // SHOW NAMES
		{ 0, { 152, 54 }, L"設定控制" }, // CONFIGURE CONTROLS
	} },
	{ LANG_POL, {
		{ 0, {  91, 37 }, L"OPCJE" }, // OPTIONS
		{ 1, {  87, 37 }, L"ZAPIS I WYJŚCIE Z GRY" }, // SAVE AND EXIT GAME
		{ 1, {  10, 37 }, L"POWRÓT DO GRY" }, // RETURN TO GAME
		{ 1, {  30, 37 }, L"POPRZEDNIE MENU" }, // PREVIOUS MENU
		{ 0, { 204, 29 }, L"POPRZEDNIE MENU" }, // PREVIOUS MENU
		{ 0, {  39, 29 }, L"WŁ." }, // ON
		{ 0, {  54, 29 }, L"WYŁ." }, // OFF
		{ 0, {  42, 29 }, L"TAK" }, // YES
		{ 0, {  34, 29 }, L"NIE" }, // NO
		{ 1, {  33, 37 }, L"OPCJE DŹWIĘKOWE" }, // SOUND OPTIONS
		{ 0, {  82, 29 }, L"DŹWIĘK" }, // SOUND
		{ 0, {  90, 29 }, L"MUZYKA" }, // MUSIC
		{ 0, { 118, 29 }, L"DŹWIĘK 3D" }, // 3D SOUND
		{ 1, {   4, 29 }, L"EFEKTY SRODOWISKOWE" }, // ENVIRONMENTAL EFFECTS
		{ 0, { 117, 29 }, L"BALANS 3D" }, // 3D BIAS
		{ 0, { 180, 29 }, L"WYPOWIEDZI BN" }, // NPC SPEECH
		{ 0, { 154, 29 }, L"GŁOSY I TEKST" }, // AUDIO AND TEXT
		{ 0, { 144, 29 }, L"TYLKO GŁOSY" }, // AUDIO ONLY
		{ 0, { 141, 29 }, L"TYLKO TEKST" }, // TEXT ONLY
		{ 1, {   7, 37 }, L"OPCJE GRAFICZNE" }, // VIDEO OPTIONS
		{ 0, { 168, 29 }, L"ROZDZIELCZOŚĆ" }, // RESOLUTION
		{ 0, { 115, 29 }, L"800x600" },
		{ 0, { 105, 29 }, L"640x480" },
		{ 0, { 226, 29 }, L"JAKOŚĆ OŚWIETLENIA" }, // LIGHTING QUALITY
		{ 0, {  91, 29 }, L"WYSOKA" }, // HIGH
		{ 0, {  92, 29 }, L"ŚREDNIA" }, // MEDIUM
		{ 0, {  61, 29 }, L"NISKA" }, // LOW
		{ 0, { 175, 29 }, L"ROZMYTE CIENIE" }, // BLENDED SHADOWS
		{ 0, { 158, 29 }, L"PERSPEKTYWA" }, // PERSPECTIVE
		{ 0, {  77, 29 }, L"GAMMA" },
		{ 0, { 112, 29 }, L"KONTRAST" }, // CONTRAST
		{ 1, {  24, 37 }, L"OPCJE AUTOMAPY" }, // AUTOMAP OPTIONS
		{ 0, { 236, 29 }, L"WIELKOŚĆ AUTOMAPY" }, // AUTOMAP SIZE
		{ 0, { 132, 29 }, L"CAŁY EKRAN" }, // FULL SCREEN
		{ 0, { 108, 29 }, L"MINIMAPA" }, // MINI MAP
		{ 0, { 134, 29 }, L"WYGASZENIE" }, // FADE
		{ 0, { 114, 29 }, L"WSZYSTKO" }, // EVERYTHING
		{ 0, {  83, 29 }, L"ŚRODEK" }, // CENTER
		{ 0, {  70, 29 }, L"AUTO" },
		{ 0, { 216, 29 }, L"AUTOCENTROWANIE" }, // CENTER WHEN CLEARED
		{ 0, { 182, 29 }, L"POKAŻ DRUŻYNĘ" }, // SHOW PARTY
		{ 0, { 157, 29 }, L"POKAŻ IMIONA" }, // SHOW NAMES
		{ 1, { 181, 37 }, L"KONFIGURACJA STEROWANIA" }, // CONFIGURE CONTROLS
	} },
	{ LANG_RUS, {
		{ 0, { 160, 36 }, L"ОПЦИИ" },
		{ 0, { 123, 41 }, L"ОПЦИИ" }, // OPTIONS
		{ 1, { 178, 36 }, L"СОХРАНИТЬ И ВЫЙТИ" },
		{ 1, { 157, 41 }, L"СОХРАНИТЬ И ВЫЙТИ" }, // SAVE AND EXIT GAME
		{ 1, { 100, 36 }, L"ВОЗВРАТ В ИГРУ" },
		{ 1, {  91, 41 }, L"ВОЗВРАТ В ИГРУ" }, // RETURN TO GAME
		{ 1, { 117, 36 }, L"ПРЕДЫДУЩЕЕ МЕНЮ" },
		{ 1, { 109, 41 }, L"ПРЕДЫДУЩЕЕ МЕНЮ" }, // PREVIOUS MENU
		{ 0, { 231, 26 }, L"ПРЕДЫДУЩЕЕ МЕНЮ" },
		{ 1, {  14, 30 }, L"ПРЕДЫДУЩЕЕ МЕНЮ" }, // PREVIOUS MENU
		{ 0, {  54, 26 }, L"ВКЛ.", TextAlign::Right }, // ON
		{ 0, {  76, 26 }, L"ВЫКЛ.", TextAlign::Right },
		{ 0, {  79, 30 }, L"ВЫКЛ.", TextAlign::Right }, // OFF
		{ 0, {  51, 26 }, L"ДА", TextAlign::Right },
		{ 0, {  43, 30 }, L"ДА", TextAlign::Right }, // YES
		{ 0, {  53, 26 }, L"НЕТ", TextAlign::Right },
		{ 0, {  50, 30 }, L"НЕТ", TextAlign::Right }, // NO
		{ 1, {  53, 36 }, L"ОПЦИИ ЗВУКА" },
		{ 1, {   3, 41 }, L"ОПЦИИ ЗВУКА" }, // SOUND OPTIONS
		{ 0, {  95, 26 }, L"ЗВУК", TextAlign::Left },
		{ 0, {  61, 30 }, L"ЗВУК", TextAlign::Left }, // SOUND
		{ 0, { 138, 26 }, L"МУЗЫКА", TextAlign::Left },
		{ 0, { 111, 30 }, L"МУЗЫКА", TextAlign::Left }, // MUSIC
		{ 0, { 144, 26 }, L"3D ЗВУК", TextAlign::Left },
		{ 0, { 110, 30 }, L"3D ЗВУК", TextAlign::Left }, // 3D SOUND
		{ 1, { 106, 26 }, L"ЭФФЕКТЫ ОКРУЖЕНИЯ", TextAlign::Left },
		{ 1, {  44, 30 }, L"ЭФФЕКТЫ ОКРУЖЕНИЯ", TextAlign::Left }, // ENVIRONMENTAL EFFECTS
		{ 0, { 109, 26 }, L"3D СМЕЩ.", TextAlign::Left }, // 3D BIAS
		{ 0, { 199, 26 }, L"РЕЧЬ NPC", TextAlign::Left },
		{ 0, { 124, 30 }, L"РЕЧЬ NPC", TextAlign::Left }, // NPC SPEECH
		{ 1, {   4, 26 }, L"ЗВУК И ТЕКСТ", TextAlign::Right },
		{ 0, { 237, 30 }, L"ЗВУК И ТЕКСТ", TextAlign::Right }, // AUDIO AND TEXT
		{ 0, { 201, 26 }, L"ТОЛЬКО ЗВУК", TextAlign::Right },
		{ 0, { 229, 30 }, L"ТОЛЬКО ЗВУК", TextAlign::Right }, // AUDIO ONLY
		{ 0, { 212, 26 }, L"ТОЛЬКО ТЕКСТ", TextAlign::Right },
		{ 0, { 216, 30 }, L"ТОЛЬКО ТЕКСТ", TextAlign::Right }, // TEXT ONLY
		{ 1, {  39, 36 }, L"ОПЦИИ ВИДЕО" },
		{ 1, {   6, 41 }, L"ОПЦИИ ВИДЕО" }, // VIDEO OPTIONS
		{ 0, { 171, 26 }, L"РАЗРЕШЕНИЕ", TextAlign::Left }, // RESOLUTION
		{ 0, { 135, 30 }, L"800x600", TextAlign::Right },
		{ 0, { 123, 30 }, L"640x480", TextAlign::Right },
		{ 1, {  68, 26 }, L"КАЧЕСТВО ОСВЕЩЕНИЯ", TextAlign::Left },
		{ 1, {  57, 30 }, L"КАЧЕСТВО ОСВЕЩЕНИЯ", TextAlign::Left }, // LIGHTING QUALITY
		{ 0, {  96, 26 }, L"ВЫСОК.", TextAlign::Right },
		{ 0, {  96, 30 }, L"ВЫСОК.", TextAlign::Right }, // HIGH
		{ 0, { 108, 26 }, L"СРЕДН.", TextAlign::Right },
		{ 0, {  90, 30 }, L"СРЕДН.", TextAlign::Right }, // MEDIUM
		{ 0, {  66, 26 }, L"НИЗК.", TextAlign::Right },
		{ 0, {  71, 30 }, L"НИЗК.", TextAlign::Right }, // LOW
		{ 1, {  72, 30 }, L"СГЛАЖЕННЫЕ ТЕНИ", TextAlign::Left }, // BLENDED SHADOWS
		{ 0, { 174, 26 }, L"ПЕРСПЕКТИВА", TextAlign::Left }, // PERSPECTIVE
		{ 0, { 105, 26 }, L"ГАММА", TextAlign::Left },
		{ 0, { 101, 30 }, L"ГАММА", TextAlign::Left }, // GAMMA
		{ 0, { 151, 26 }, L"КОНТРАСТ", TextAlign::Left }, // CONTRAST
		{ 1, { 154, 36 }, L"ОПЦИИ АВТОКАРТЫ" },
		{ 1, { 120, 41 }, L"ОПЦИИ АВТОКАРТЫ" }, // AUTOMAP OPTIONS
		{ 0, { 208, 26 }, L"РАЗМЕР КАРТЫ", TextAlign::Left }, // AUTOMAP SIZE
		{ 0, { 163, 26 }, L"ВЕСЬ ЭКРАН", TextAlign::Right }, // FULL SCREEN
		{ 0, { 170, 26 }, L"МИНИКАРТА", TextAlign::Right }, // MINI MAP
		{ 0, { 209, 26 }, L"ПРОЗРАЧНОСТЬ", TextAlign::Left },
		{ 0, { 204, 30 }, L"ПРОЗРАЧНОСТЬ", TextAlign::Left }, // FADE
		{ 0, { 173, 26 }, L"ВСЁ", TextAlign::Right },
		{ 0, {  45, 30 }, L"ВСЁ", TextAlign::Right }, // EVERYTHING
		{ 0, { 104, 26 }, L"ЦЕНТР", TextAlign::Right },
		{ 0, {  81, 30 }, L"ЦЕНТР", TextAlign::Right }, // CENTER
		{ 0, {  77, 26 }, L"АВТО", TextAlign::Right }, // AUTO
		{ 1, { 139, 26 }, L"НА ЦЕНТР ПОСЛЕ ОЧИСТКИ", TextAlign::Left },
		{ 1, {  70, 30 }, L"НА ЦЕНТР ПОСЛЕ ОЧИСТКИ", TextAlign::Left }, // CENTER WHEN CLEARED
		{ 0, { 206, 26 }, L"ПОКАЗ ГРУППЫ", TextAlign::Left },
		{ 1, {  53, 30 }, L"ПОКАЗ ГРУППЫ", TextAlign::Left }, // SHOW PARTY
		{ 0, { 190, 26 }, L"ПОКАЗ ИМЁН", TextAlign::Left },
		{ 1, {  26, 30 }, L"ПОКАЗ ИМЁН", TextAlign::Left }, // SHOW NAMES
		{ 1, { 255, 36 }, L"НАСТРОЙКА УПРАВЛЕНИЯ" },
		{ 1, { 221, 41 }, L"НАСТРОЙКА УПРАВЛЕНИЯ" }, // CONFIGURE CONTROLS
	} },
};


inline std::vector<D2TextInfo> g_death_texts = {
	{ 1, {  20, 36 }, L"You have died" }, // youdiedhardcore.dc6
	{ 1, { 175, 36 }, L"Press ESC to continue" }, // youdiedinst.dc6
	{ 1, {  23, 36 }, L"You lost gold" }, // youdiedsoftcore.dc6
};
inline const std::map<uint32_t, std::vector<D2TextInfo>> g_death_text_others = {
	{ LANG_ESP, {
		{ 0, { 218, 36 }, L"Sie sind gestorben" }, // You have died
		{ 2, {   7, 36 }, L"Presiona ESCAPE para Continuar" }, // Press ESC to continue
		{ 1, {  47, 36 }, L"Perdiste Oro" }, // You lost gold
	} },
	{ LANG_DEU, {
		{ 1, {  75, 36 }, L"You have died" }, // You have died
		{ 2, {  18, 36 }, L"Weiter mit Druck auf ESC" }, // Press ESC to continue
		{ 1, { 200, 36 }, L"Sie haben Gold verloren" }, // You lost gold
	} },
	{ LANG_FRA, {
		{ 1, {  54, 36 }, L"Vous étes mort" }, // You have died
		{ 2, { 124, 36 }, L"Appuyez sur ECH pour continuer" }, // Press ESC to continue
		{ 1, { 220, 36 }, L"Vous avez perdu de l'or" }, // You lost gold
	} },
	{ LANG_POR, {
		{ 1, {  20, 36 }, L"Você morreu" }, // youdiedhardcore.dc6
		{ 1, { 175, 36 }, L"Pressione ESC para continuar." }, // youdiedinst.dc6
		{ 1, {  23, 36 }, L"You lost gold" }, // youdiedsoftcore.dc6
	} },
	{ LANG_ITA, {
		{ 0, { 181, 36 }, L"Sei morto" }, // You have died
		{ 1, { 233, 36 }, L"Premi ESC per continuare" }, // Press ESC to continue
		{ 1, {  32, 36 }, L"Hai perso l'oro" }, // You lost gold
	} },
	{ LANG_JPN, {
		{ 0, { 147, 44 }, L"汝は死せり" }, // You have died
		{ 0, { 210, 44 }, L"ESCキーを押せ" }, // Press ESC to continue
		{ 0, { 217, 44 }, L"ゴールドを失った" }, // You lost gold
	} },
	{ LANG_KOR, {
		{ 0, { 253, 38 }, L"당신은 죽었습니다" }, // You have died
		{ 2, {   3, 38 }, L"계속하려면 ESC 키를 누르세요" }, // Press ESC to continue
		{ 1, {  94, 38 }, L"골드를 잃었습니다" }, // You lost gold
	} },
	{ LANG_SIN, {
		{ 0, { 111, 54 }, L"你挂了" }, // You have died
		{ 0, { 248, 54 }, L"按ESC继续" }, // Press ESC to continue
		{ 1, {  40, 54 }, L"你丢失了金币" }, // You lost gold
	} },
	{ LANG_CHI, {
		{ 0, { 111, 54 }, L"你掛了" }, // You have died
		{ 0, { 248, 54 }, L"按下ESC鍵繼續" }, // Press ESC to continue
		{ 1, {  40, 54 }, L"你損失金錢數量" }, // You lost gold
	} },
	{ LANG_POL, {
		{ 1, { 117, 37 }, L"TWOJA POSTAĆ ZGINĘŁA" }, // You have died
		{ 2, {   4, 37 }, L"WCIŚNIJ ESC, ABY KONTYNUOWAĆ" }, // Press ESC to continue
		{ 1, { 250, 37 }, L"TWOJA POSTAĆ STRACIŁA ZŁOTO" }, // You lost gold
	} },
	{ LANG_RUS, {
		{ 1, {  20, 36 }, L"Вы умерли." },
		{ 0, { 202, 41 }, L"Вы умерли." }, // You have died
		{ 1, { 245, 36 }, L"Нажмите ESC, чтобы продолжить." },
		{ 2, {  73, 41 }, L"Нажмите ESC, чтобы продолжить." }, // Press ESC to continue
		{ 1, { 106, 36 }, L"You lost gold" },
		{ 1, { 124, 41 }, L"You lost gold" }, // You lost gold
	} },
};

#ifdef _HDTEXT
struct D2SampleText {
	const wchar_t* txt1;
	const wchar_t* txt2;
	const wchar_t* txt3;
};

inline const std::map<uint32_t, D2SampleText> g_sample_text = {
	{ LANG_ENG, {
		L"Single line text ABCDWXYZ 0123456789",
		L"Lorem ipsum dolor sit amet, consectetur\nDouble line text ABCDWXYZ 0123456789",
		L"magna aliqua.\nincididunt ut labore et dolore\nadipiscing elit, sed do eiusmod tempor\nLorem ipsum dolor sit amet, consectetur\nMulti line text ABCDWXYZ 0123456789",
	} },
	{ LANG_JPN, {
		L"単一行のテキスト ABCDWXYZ 0123456789",
		L"財ニフタ任社応人ヨ地熊いらどぱ歩実コユワタ\n二重線のテキスト ABCDWXYZ 0123456789",
		L"傷おる皇7指15康セコ児悪に永峠そへ。\n局国ケエク昇強大就仏っなんフ。\n物載ぜもりは氏映まべ図療ほつずへ編思つへ\n京著認ぼ橋償やづ保者アマハ稔文マウス朝催むトス\n複数行のテキスト ABCDWXYZ 0123456789",
	} },
	{ LANG_KOR, {
		L"한 줄 텍스트 ABCDWXYZ 0123456789",
		L"로렘 입숨은 전통 라틴어와 닮은 점 때문에 종종\n이중선 문자 ABCDWXYZ 0123456789",
		L"텍스트에 담긴 뜻에.\n문서에서 텍스트가 보이면 사람들은 전체적인 프레젠테이션보다는\n유발하기도 하지만 그 이상의 의미를 담지는 않는다.\n로렘 입숨은 전통 라틴어와 닮은 점 때문에 종종 호기심을\n여러 줄 텍스트 ABCDWXYZ 0123456789",
	} },
	{ LANG_CHI, {
		L"单行文本 ABCDWXYZ 0123456789",
		L"迷読舞新収氷問支出簡証謙研吉分治連。\n双行文字 ABCDWXYZ 0123456789",
		L"力然春就最韓防町政道禁話禁毎暮就。\n出食避能側減間大務勝性日愛史打込性陸。\n続職午井予活単暮質今言無害測。\n画年渡労読態関実計視申同衆意。\n多行文本 ABCDWXYZ 0123456789",
	} },
	{ LANG_RUS, {
		L"Однострочный текст ABCDWXYZ 0123456789",
		L"Lorem ipsum dolor sit amet, consectetur\nТекст в две строки ABCDWXYZ 0123456789",
		L"сенсибус дефинитионес яуо ад.\nНе путент поссит петентиум при, атяуи\nдиспутандо, яуо диам граеци улламцорпер еу.\nЛорем ипсум долор сит амет, мел еу виси\nМногострочный текст ABCDWXYZ 0123456789",
	} },
};
#endif
// clang-format on

}