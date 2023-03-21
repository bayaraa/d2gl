#include "pch.h"
#include "command_buffer.h"

namespace d2gl {

void CommandBuffer::begin()
{
	m_command = &m_commands[0];
	m_count = 0;
}

inline void CommandBuffer::next()
{
	m_command++;
	m_count++;
}

void CommandBuffer::execute()
{
	for (uint32_t i = 0; i < m_count; i++) {
		const auto command = m_commands[i];

		switch (command.type) {
			case CommandType::BindFrameBuffer:
				if (command.framebuffer)
					glBindFramebuffer(GL_FRAMEBUFFER, command.framebuffer->getId());
				break;
			case CommandType::ClearFrameBuffer:
				if (command.framebuffer)
					command.framebuffer->clearBuffer();
				break;
			case CommandType::UnbindFrameBuffer:
				glBindFramebuffer(GL_FRAMEBUFFER, 0);
				break;
			case CommandType::ClearBuffer:
				glClearColor(0.0f, 0.0f, 0.2f, 1.0f);
				glClear(GL_COLOR_BUFFER_BIT);
				break;
			case CommandType::SetViewport:
				glViewport(command.viewport.offset.x, command.viewport.offset.y, command.viewport.size.x, command.viewport.size.y);
				break;
			case CommandType::UseProgram:
				if (command.pipeline)
					glUseProgram(command.pipeline->getId());
				break;
			case CommandType::SetBlendState:
				// if (command.pipeline)
				// command.pipeline->setBlendState(command.blend_index);
				break;
			case CommandType::DrawIndexed:
				glBufferSubData(GL_ARRAY_BUFFER, 0, command.draw.count * sizeof(Vertex), command.draw.data);
				glDrawElements(GL_TRIANGLES, command.draw.count / 4 * 6, GL_UNSIGNED_INT, 0);
				break;
		}
	}
}

void CommandBuffer::bindFrameBuffer(FrameBuffer* framebuffer)
{
	m_command->type = CommandType::BindFrameBuffer;
	m_command->framebuffer = framebuffer;
	next();
}

void CommandBuffer::clearFrameBuffer(FrameBuffer* framebuffer)
{
	m_command->type = CommandType::ClearFrameBuffer;
	m_command->framebuffer = framebuffer;
	next();
}

void CommandBuffer::unbindFrameBuffer()
{
	m_command->type = CommandType::UnbindFrameBuffer;
	next();
}

void CommandBuffer::clearBuffer()
{
	m_command->type = CommandType::ClearBuffer;
	next();
}

void CommandBuffer::setViewport(glm::ivec2 size, glm::ivec2 offset)
{
	m_command->type = CommandType::SetViewport;
	m_command->viewport.size = size;
	m_command->viewport.offset = offset;
	next();
}

void CommandBuffer::useProgram(Pipeline* pipeline)
{
	m_command->type = CommandType::UseProgram;
	m_command->pipeline = pipeline;
	next();
}

void CommandBuffer::setBlendState(Pipeline* pipeline, uint32_t index)
{
	m_command->type = CommandType::SetBlendState;
	m_command->pipeline = pipeline;
	m_command->blend_index = index;
	next();
}

void CommandBuffer::drawIndexed(void* data, uint32_t count)
{
	m_command->type = CommandType::DrawIndexed;
	m_command->draw.data = data;
	m_command->draw.count = count;
	next();
}

}