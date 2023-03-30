#include "pch.h"
#include "command_buffer.h"

namespace d2gl {

CommandBuffer::CommandBuffer()
{
	m_texture_storage.data = new uint8_t[256 * 256 * 1024];
	reset();
}

CommandBuffer::~CommandBuffer()
{
	// delete[] m_texture_storage.data;
}

// void CommandBuffer::execute()
//{
//	for (uint32_t i = 0; i < m_count; i++) {
//		const auto command = m_commands[i];
//
//		switch (command.type) {
//			case CommandType::BindFrameBuffer:
//				if (command.framebuffer)
//					glBindFramebuffer(GL_FRAMEBUFFER, command.framebuffer->getId());
//				break;
//			case CommandType::ClearFrameBuffer:
//				if (command.framebuffer)
//					command.framebuffer->clearBuffer();
//				break;
//			case CommandType::UnbindFrameBuffer:
//				glBindFramebuffer(GL_FRAMEBUFFER, 0);
//				break;
//			case CommandType::ClearBuffer:
//				glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
//				glClear(GL_COLOR_BUFFER_BIT);
//				break;
//			case CommandType::SetViewport:
//				glViewport(command.viewport.offset.x, command.viewport.offset.y, command.viewport.size.x, command.viewport.size.y);
//				break;
//			case CommandType::UseProgram:
//				if (command.pipeline)
//					glUseProgram(command.pipeline->getId());
//				break;
//			case CommandType::SetBlendState:
//				if (command.pipeline)
//					command.pipeline->setBlendState(command.blend_index);
//				break;
//			case CommandType::DrawIndexed:
//				glBufferSubData(GL_ARRAY_BUFFER, 0, command.draw.count * sizeof(Vertex), command.draw.data);
//				glDrawElements(GL_TRIANGLES, command.draw.count / 4 * 6, GL_UNSIGNED_INT, 0);
//				break;
//			case CommandType::UpdateTexture:
//				App.context->getGameTexture()->fill(m_tex_storage.data[command.update.index], command.update.size.x, command.update.size.y, command.update.offset.x, command.update.offset.y, command.update.num);
//				break;
//			case CommandType::UpdateUBO:
//				App.context->getGameColorUBO()->updateData(m_ubo_storage.data[command.ubo_index].name, &m_ubo_storage.data[command.ubo_index].value[0]);
//				break;
//			case CommandType::CopyTexture:
//				if (command.texture)
//					command.texture->get()->fillFromBuffer(command.framebuffer, command.fb_index);
//				break;
//			case CommandType::Resize:
//				App.context->onResize();
//				break;
//		}
//	}
// }

void CommandBuffer::reset()
{
	m_count = 0;
	m_command = &m_commands[0];
	m_ubo_storage.count = 0;
	m_texture_storage.offset = 0;
}

inline void CommandBuffer::next()
{
	m_command++;
	m_count++;
}

void CommandBuffer::pushCommand(CommandType type)
{
	m_command->type = type;
	next();
}

void CommandBuffer::colorUpdate(UBOType type, const void* data)
{
	memcpy(m_ubo_storage.data[m_ubo_storage.count].value, data, sizeof(glm::vec4) * 256);
	m_command->type = CommandType::ColorUpdate;
	m_command->color.data = m_ubo_storage.data[m_ubo_storage.count].value;
	m_command->color.type = type;
	m_ubo_storage.count++;
	next();
}

void CommandBuffer::textureUpdate(uint8_t* data, uint16_t tex_num, glm::vec<2, uint16_t> size, glm::vec<2, uint16_t> offset)
{
	// const uint32_t data_size = size.x * size.y;
	//// void* ptr = glMapBufferRange(GL_PIXEL_UNPACK_BUFFER, m_pbo_offset, data_size, GL_MAP_WRITE_BIT);
	//// memcpy(ptr, data, data_size);
	////// glFlushMappedBufferRange(GL_PIXEL_UNPACK_BUFFER, m_pbo_offset, data_size);
	//// glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
	//// glBufferSubData(GL_PIXEL_UNPACK_BUFFER, m_pbo_offset, data_size, data);
	// memcpy((void*)((uint32_t)App.context->getPixelBufferPtr() + m_pbo_offset), data, data_size);
	// glFlushMappedBufferRange(GL_PIXEL_UNPACK_BUFFER, m_pbo_offset, data_size);

	// m_command->type = CommandType::TextureUpdate;
	// m_command->texture.pbo_offset = m_pbo_offset;
	// m_command->texture.tex_num = tex_num;
	// m_command->texture.size = size;
	// m_command->texture.offset = offset;
	// m_pbo_offset += data_size;
	next();
}

void CommandBuffer::setBlendState(uint32_t index)
{
	m_command->type = CommandType::SetBlendState;
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