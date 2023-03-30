#pragma once

namespace d2gl {

enum class CommandType {
	None,
	Begin,
	ColorUpdate,
	TextureUpdate,
	SetBlendState,
	DrawIndexed,
	PreFx,
	Submit,
};

enum class UBOType {
	Gamma,
	Palette,
};

struct UBOData {
	UBOType type = UBOType::Gamma;
	glm::vec4 value[256] = { glm::vec4(0.0f) };
};

struct Command {
	CommandType type = CommandType::None;
	union {
		struct {
			uint32_t storage_offset;
			uint16_t tex_num;
			glm::vec<2, uint16_t> size;
			glm::vec<2, uint16_t> offset;
		} texture;

		struct {
			void* data;
			UBOType type;
		} color;

		uint32_t blend_index;

		struct {
			void* data;
			uint32_t count;
		} draw;
	};
};

struct UBOStorage {
	uint32_t count = 0;
	std::array<UBOData, 10> data;
};

struct TextureStorage {
	uint32_t offset = 0;
	uint8_t* data = nullptr;
};

class CommandBuffer {
	uint32_t m_count = 0;
	Command* m_command = nullptr;
	std::array<Command, 1200> m_commands;
	UBOStorage m_ubo_storage;
	TextureStorage m_texture_storage;

	friend class Context;

public:
	CommandBuffer();
	~CommandBuffer();

	void reset();
	void next();

	void pushCommand(CommandType type);
	void colorUpdate(UBOType type, const void* data);
	void textureUpdate(uint8_t* data, uint16_t tex_num, glm::vec<2, uint16_t> size, glm::vec<2, uint16_t> offset);
	void setBlendState(uint32_t index);
	void drawIndexed(void* data, uint32_t count);
};

}