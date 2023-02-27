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

#include "helpers.h"

namespace d2gl {

enum class PatchType {
	Nop,
	Auto,
	Jump,
	Call,
	Swap,
	Custom,
};

struct PatchByte {
	int value = 0;
	uint8_t type = 0;
};
typedef std::vector<PatchByte> Bytes;

class SubPatch {
	PatchType m_type;
	Offset m_offset;
	uint16_t m_length;
	uintptr_t m_pointer = 0, m_address = 0;
	Bytes* m_bytes = nullptr;

	uint8_t* m_old_code = nullptr;
	uint8_t* m_new_code = nullptr;

	bool m_ready = false;
	bool m_installed = false;

public:
	SubPatch(PatchType type, Offset offset, uint16_t len, uintptr_t ptr = 0);
	SubPatch(Offset offset, uint16_t len, Bytes* bytes);
	~SubPatch();

	bool prepare();
	bool toggle(bool active = true);
};

class Patch {
	bool m_ready = false;
	bool m_installed = false;
	std::deque<std::unique_ptr<SubPatch>> m_sub_patches;

public:
	Patch() = default;
	~Patch() = default;

	bool prepare();
	void add(PatchType type, Offset offset, uint16_t len, uintptr_t ptr = 0);
	void add(Offset offset, uint16_t len, Bytes* bytes);
	bool toggle(bool active = true);

	inline const bool isActive() { return m_installed; }

	static void getBytes(uintptr_t address, size_t len, uint8_t** dst);
	static void setBytes(uintptr_t address, size_t len, uint8_t* src);
};

}