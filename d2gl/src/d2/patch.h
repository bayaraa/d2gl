#pragma once

#include "helpers.h"

namespace d2gl::d2 {

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
};

}