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

namespace d2gl::d2 {

#define MERC_A1 0x010f
#define MERC_A2 0x0152
#define MERC_A3 0x0167
#define MERC_A4 0x0420 // PD2
#define MERC_A5 0x0231

#define LANG_ENG 0x00
#define LANG_ESP 0x01
#define LANG_DEU 0x02
#define LANG_FRA 0x03
#define LANG_POR 0x04
#define LANG_ITA 0x05
#define LANG_JPN 0x06
#define LANG_KOR 0x07
#define LANG_SIN 0x08
#define LANG_CHI 0x09
#define LANG_POL 0x0A
#define LANG_RUS 0x0B
#define LANG_DEF 0x0C

#define SCREENPANEL_NONE 0
#define SCREENPANEL_RIGHT 1
#define SCREENPANEL_LEFT 2
#define SCREENPANEL_BOTH 3

#define STAT_HP 6
#define STAT_MAXHP 7
#define STAT_DMGREDUCTIONPCT 36
#define STAT_MAGICDMGREDUCTIONPCT 37
#define STAT_FIRERESIST 39
#define STAT_LIGHTNINGRESIST 41
#define STAT_COLDRESIST 43
#define STAT_POISONRESIST 45
#define STAT_ITEMQUANTITY 70

enum class UnitType {
	Player,
	Monster,
	Object,
	Missile,
	Item,
	VisTile,
	Count,
};

enum class ItemQuality {
	None,
	Inferior,
	Normal,
	Superior,
	Magic,
	Set,
	Rare,
	Unique,
	Craft,
};

enum class MonsterType {
	Normal,
	SuperUnique,
	Boss,
	Champion,
	Possesed,
	Ghostly,
};

struct UnitAny;
struct Room1;

#pragma pack(push, 1)
struct DT1SubBlock {
	WORD xPos;
	WORD yPos;
	WORD _1;
	BYTE gridX;
	BYTE gridY;
	WORD wTileFormat;
	DWORD dwSubLength;
	WORD _2;
	DWORD* pDataOffset;
};
#pragma pack(pop)

struct TileContext {
	DWORD dwDirection;
	WORD wRoofIndex;
	BYTE bSound;
	BYTE bAnimated;
	DWORD dwSizeY;
	DWORD dwSizeX;
	DWORD dwZeros1;
	DWORD dwOrientation;
	DWORD dwMainIndex;
	DWORD dwSubIndex;
	DWORD dwFrame;
	BYTE _1[4];
	BYTE bFlags[25];
	BYTE _2;
	WORD sCacheIndex;
	DWORD _3;
	DWORD dwDataPtr;
	DWORD dwSize;
	DWORD dwSubBlocks;
	DT1SubBlock* pBlocks;
	char* szTileName;
	DWORD** ptBlock;
};

struct GFXLight {
	union {
		struct {
			BYTE intensity;
			BYTE r, g, b;
		};
		DWORD dwColor;
	};
};

struct GFXLightEx : GFXLight {
	int nXpos;
	int nYpos;
};

struct GfxCell {
	DWORD flags;
	DWORD width;
	DWORD height;
	DWORD xoffs;
	DWORD yoffs;
	DWORD _1;
	DWORD lpParent;
	DWORD length;
	BYTE cols;
};

struct CellFile {
	DWORD dwVersion;
	struct {
		WORD dwFlags;
		BYTE mylastcol;
		BYTE mytabno : 1;
	};
	DWORD eFormat;
	DWORD termination;
	DWORD numdirs;
	DWORD numcells;
	GfxCell* cells[255];
};

struct CellContext {
	union {
		struct {
			DWORD nCellNo;
			DWORD _1[12];
			CellFile* pCellFile;
			DWORD _2;
			GfxCell* pCurGfxCell;
		} v113;

		struct {
			DWORD _1[13];
			GfxCell* pCurGfxCell;
			DWORD _2;
			CellFile* pCellFile;
			DWORD nCellNo;
		} v112;

		struct {
			DWORD _1[2];
			CellFile* pCellFile;
			DWORD _2[2];
			GfxCell* pCurGfxCell;
			DWORD _3[11];
			DWORD nCellNo;
		} v111;

		struct {
			GfxCell* pCurGfxCell;
			CellFile* pCellFile;
			DWORD nCellNo;
		} v109;
	};
};

struct Path {
	DWORD x;
	DWORD y;
	DWORD xAutomap;
	DWORD yAutomap;
	short xTarget;
	short yTarget;
	DWORD _1[2];
	Room1* pRoom1;
	Room1* pRoomUnk;
	DWORD _2[3];
	UnitAny* pUnit;
	DWORD dwFlags;
	DWORD _3;
	DWORD dwPathType;
	DWORD dwPrevPathType;
	DWORD dwUnitSize;
	DWORD _4[2];
	DWORD dwCollisionFlag;
	DWORD _5;
	UnitAny* pTargetUnit;
	DWORD dwTargetType;
	DWORD dwTargetId;
	BYTE bDirection;
};

struct StaticPath {
	Room1* pRoom1;
	DWORD xOffset;
	DWORD yOffset;
	DWORD xPos;
	DWORD yPos;
	DWORD _1[2];
	DWORD dwFlags;
};

struct PlayerData {
	char szName[16];
};

struct MonStatsTxt {
	DWORD _1[19];
	BYTE bAlign;
};

struct MonsterData110 {
	MonStatsTxt* pMonStatsTxt;
	BYTE Components[16];
	WORD NameSeed;
	struct {
		BYTE fUnk : 1;
		BYTE fSuperUniq : 1;
		BYTE fChamp : 1;
		BYTE fBoss : 1;
		BYTE fMinion : 1;
		BYTE fPossesed : 1;
		BYTE fGhostly : 1;
		BYTE fMultishot : 1;
	};
	BYTE dwLastMode;
	DWORD dwDuriel;
	BYTE anEnchants[9];
	WORD wUniqueNo;
	DWORD _1;
	wchar_t* wName;
};

struct MonsterData109 {
	struct {
		WORD fBoss : 1;
		WORD fMinion : 1;
		WORD fChamp : 1;
	};
	wchar_t wName[1];
	DWORD _1[52];
	DWORD anEnchants[9];
};

struct ItemData110 {
	ItemQuality dwQuality;
	DWORD _1[2];
	DWORD dwItemFlags;
	DWORD _2[2];
	DWORD dwFlags;
	DWORD _3[3];
	DWORD dwFileIndex;
	DWORD dwItemLevel;
	WORD wVersion;
	WORD wRarePrefix;
	WORD wRareSuffix;
	WORD wAutoPrefix;
	WORD wPrefix[3];
	WORD wSuffix[3];
	BYTE BodyLocation;
	BYTE ItemLocation;
	BYTE EarLevel;
	BYTE VarGfx;
	CHAR personalizedName[16];
	WORD _4;
	void* pOwnerInventory;
	UnitAny* pPrevInvItem;
	UnitAny* pNextInvItem;
	BYTE _5;
	BYTE NodePage;
	WORD _6;
	DWORD _7[6];
	UnitAny* pOwner;
};

struct ItemData109 {
	ItemQuality dwQuality;
	DWORD _1[15];
	BYTE BodyLocation;
	BYTE ItemLocation;
};

struct UnitAny {
	UnitType dwType;
	union {
		struct {
			DWORD dwClassId;
			void* pMemPool;
			DWORD dwUnitId;
			DWORD dwMode;
			union {
				PlayerData* pPlayerData;
				ItemData110* pItemData;
				MonsterData110* pMonsterData;
				void* pObjectData;
			};
			DWORD dwAct;
			void* pAct;
			DWORD dwSeed[2];
			DWORD dwInitSeed;
			union {
				Path* pPath;
				StaticPath* pStaticPath;
			};
			DWORD _1[16];
			UnitAny* pUpdateUnit;
			DWORD _2[8];
			DWORD dwOwnerType;
			DWORD dwOwnerId;
			DWORD _3[2];
			void* pOMsg;
			void* pInfo;
			void* ptCombat;
			DWORD _4[5];
			DWORD dwFlags;
			DWORD dwFlags2;
			DWORD _5[5];
			UnitAny* pChangedNext;
			UnitAny* pRoomNext;
			UnitAny* pListNext;
		} v110;

		struct {
			DWORD _1;
			DWORD dwUnitId;
			DWORD _2[11];
			union {
				Path* pPath;
				StaticPath* pStaticPath;
			};
			DWORD _3[13];
			union {
				PlayerData* pPlayerData;
				ItemData109* pItemData;
				MonsterData109* pMonsterData;
				void* pObjectData;
			};
			DWORD _4[29];
			DWORD dwFlags;
			DWORD dwFlags2;
			DWORD _5[10];
			UnitAny* pListNext;
			UnitAny* pRoomNext;
		} v109;
	};
};

#pragma pack(push, 1)
struct Room1 {
	Room1** pRoomsNear;
	DWORD _1[3];
	void* pRoom2;
	DWORD _2[2];
	UnitAny** pUnitChanged;
	void* Coll;
	DWORD dwRoomsNear;
	DWORD nPlayerUnits;
	void* pAct;
	DWORD _3;
	DWORD nUnknown;
	DWORD _4[4];
	void** pClients;
	BYTE hCoords[32];
	int64_t hSeed;
	UnitAny* pUnitFirst;
	DWORD nNumClients;
	Room1* pRoomNext;
};
#pragma pack(pop)

}