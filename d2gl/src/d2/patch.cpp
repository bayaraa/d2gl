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
#include "patch.h"

namespace d2gl::d2 {

SubPatch::SubPatch(PatchType type, Offset offset, uint16_t len, uintptr_t ptr)
	: m_type(type), m_offset(offset), m_length(len), m_pointer(ptr) {}

SubPatch::SubPatch(Offset offset, uint16_t len, Bytes* bytes)
	: m_type(PatchType::Custom), m_offset(offset), m_length(len), m_bytes(bytes) {}

SubPatch::~SubPatch()
{
	delete[] m_old_code;
	delete[] m_new_code;
}

bool SubPatch::prepare()
{
	if (m_ready)
		return true;

	m_address = helpers::getProcOffset(m_offset);
	if (!m_address) {
		error("Patch failed: %s, %d(0x%.8X), %d(0x%.8X), 0x%.8X", m_offset.module, m_offset.pos, m_offset.pos, m_offset.add, m_offset.add, m_offset.og_4bytes);
		return false;
	}

	m_old_code = new uint8_t[m_length];
	m_new_code = new uint8_t[m_length];

	ReadProcessMemory(GetCurrentProcess(), (void*)m_address, m_old_code, m_length, NULL);
	memset(m_new_code, 0x90, m_length);

	if (m_type == PatchType::Auto) {
		uint8_t* gateway = (uint8_t*)VirtualAlloc(0, m_length + 10, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
		if (!gateway)
			return false;

		memset(gateway, 0x90, m_length + 10);

		*(gateway) = 0xE8; // call injected function
		*(uintptr_t*)(gateway + 1) = m_pointer - (uintptr_t)gateway - 5;

		memcpy_s(gateway + 5, m_length, (void*)m_address, m_length);

		*(gateway + m_length + 5) = 0xE9; // jump back and continue
		*(uintptr_t*)(gateway + m_length + 6) = m_address - (uintptr_t)gateway - m_length - 5;

		m_new_code[0] = 0xE9;
		*(uintptr_t*)&m_new_code[1] = *(uintptr_t*)&gateway - (m_address + 5);

	} else if (m_type == PatchType::Custom) {
		if (!m_bytes)
			return false;

		uint16_t code_len = 0;
		for (auto& byte : *m_bytes)
			code_len += !byte.type ? 1 : 4;

		if (!code_len)
			return false;

		uint8_t* gateway = (uint8_t*)VirtualAlloc(0, code_len + 5, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
		if (!gateway)
			return false;

		memset(gateway, 0x90, code_len + 5);

		uint32_t i = 0;
		for (auto& byte : *m_bytes) {
			if (byte.type == 1)
				*(uintptr_t*)(gateway + i) = byte.value;
			else
				*(gateway + i) = byte.value;
			i += !byte.type ? 1 : 4;
		}

		*(gateway + code_len) = 0xE9; // jump back and continue
		*(uintptr_t*)(gateway + code_len + 1) = m_address - (uintptr_t)gateway - code_len;

		m_new_code[0] = 0xE9;
		*(uintptr_t*)&m_new_code[1] = *(uintptr_t*)&gateway - (m_address + 5);

		delete m_bytes;

	} else if (m_type == PatchType::Jump) {
		m_new_code[0] = 0xE9;
		*(uintptr_t*)&m_new_code[1] = m_pointer - (m_address + 5);

	} else if (m_type == PatchType::Call) {
		m_new_code[0] = 0xE8;
		*(uintptr_t*)&m_new_code[1] = m_pointer - (m_address + 5);

	} else if (m_type == PatchType::Swap) {
		uint8_t* bytes = (uint8_t*)&m_pointer;
		for (uint32_t i = 0; i < m_length; i++)
			m_new_code[i] = bytes[3 - i];
	}

	m_ready = true;
	return true;
}

bool SubPatch::toggle(bool active)
{
	if (!prepare())
		return false;

	if (active && !m_installed) {
		DWORD protect;
		VirtualProtect((void*)m_address, m_length, PAGE_EXECUTE_READWRITE, &protect);
		memcpy_s((void*)m_address, m_length, m_new_code, m_length);
		VirtualProtect((void*)m_address, m_length, protect, &protect);

		m_installed = true;
	}

	if (!active && m_installed) {
		DWORD protect;
		VirtualProtect((void*)m_address, m_length, PAGE_EXECUTE_READWRITE, &protect);
		memcpy_s((void*)m_address, m_length, m_old_code, m_length);
		VirtualProtect((void*)m_address, m_length, protect, &protect);

		m_installed = false;
	}

	return true;
}

bool Patch::prepare()
{
	if (m_ready)
		return true;

	for (auto& sub_patch : m_sub_patches) {
		if (!sub_patch->prepare())
			return false;
	}

	m_ready = true;
	return true;
}

void Patch::add(PatchType type, Offset offset, uint16_t len, uintptr_t ptr)
{
	m_sub_patches.push_front(std::make_unique<SubPatch>(type, offset, len, ptr));
}

void Patch::add(Offset offset, uint16_t len, Bytes* bytes)
{
	m_sub_patches.push_front(std::make_unique<SubPatch>(offset, len, bytes));
}

bool Patch::toggle(bool active)
{
	if (!prepare())
		return false;

	if ((active && !m_installed) || (!active && m_installed)) {
		for (auto& sub_patch : m_sub_patches)
			sub_patch->toggle(active);

		m_installed = !m_installed;
	}

	return true;
}

}