#pragma once

namespace d2gl {

enum class CommandType {
	None,
	Begin,
	UBOUpdate,
	SetBlendState,
	DrawIndexed,
	PreFx,
	Submit,
	Resize,
};

struct Command {
	CommandType type = CommandType::None;
	union {
		uint32_t index = 0;
		struct {
			uint32_t start;
			uint32_t count;
		} draw;
	};
};

struct TexData {
	uint32_t offset;
	uint16_t tex_num;
	glm::vec<2, uint16_t> tex_size;
	glm::vec<2, uint16_t> tex_offset;
};

struct TexUpdateQueue {
	uint32_t count = 0;
	uint8_t* data = nullptr;
	uint32_t data_offset = 0;
	std::array<TexData, 1280> tex_data = {};
};

enum class UBOType {
	Gamma,
	Palette,
};

struct UBOData {
	UBOType type = UBOType::Gamma;
	glm::vec4 value[256] = { glm::vec4(0.0f) };
};

struct UBOUpdateQueue {
	uint32_t count = 0;
	std::array<UBOData, 8> data;
};

class CommandBuffer {
	uint32_t m_count = 0;
	Command* m_command = nullptr;
	std::array<Command, 256> m_commands;

	UBOUpdateQueue m_ubo_update_queue;
	TexUpdateQueue m_tex_update_queue;
	uint32_t m_vertex_count = 0;

	friend class Context;

public:
	CommandBuffer();
	~CommandBuffer();

	void reset();
	void next();

	void pushCommand(CommandType type);
	void setBlendState(uint32_t index);
	void drawIndexed(uint32_t start, uint32_t count);

	void colorUpdate(UBOType type, const void* data);
	void textureUpdate(uint8_t* data, uint16_t tex_num, glm::vec<2, uint16_t> size, glm::vec<2, uint16_t> offset);
};

}