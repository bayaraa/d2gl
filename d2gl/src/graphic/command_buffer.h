#pragma once

#include "frame_buffer.h"
#include "pipeline.h"

namespace d2gl {

enum class CommandType {
	BindFrameBuffer,
	ClearFrameBuffer,
	UnbindFrameBuffer,
	ClearBuffer,
	SetViewport,
	UseProgram,
	SetBlendState,
	DrawIndexed,
};

struct Command {
	CommandType type;
	union {
		FrameBuffer* framebuffer;

		struct {
			glm::ivec2 size;
			glm::ivec2 offset;
		} viewport;

		Pipeline* pipeline;
		uint32_t blend_index;

		struct {
			void* data;
			uint32_t count;
		} draw;
	};
};

class CommandBuffer {
	std::array<Command, 1024> m_commands;
	uint32_t m_count = 0;
	Command* m_command;

public:
	CommandBuffer() = default;
	~CommandBuffer() = default;

	void begin();
	inline void next();
	void execute();

	void bindFrameBuffer(FrameBuffer* framebuffer);
	void clearFrameBuffer(FrameBuffer* framebuffer);
	void unbindFrameBuffer();

	void clearBuffer();

	void setViewport(glm::ivec2 size, glm::ivec2 offset);

	void useProgram(Pipeline* pipeline);
	void setBlendState(Pipeline* pipeline, uint32_t index);

	void drawIndexed(void* data, uint32_t count);
};

}