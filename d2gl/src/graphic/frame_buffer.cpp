#include "pch.h"
#include "frame_buffer.h"
#include "texture.h"

namespace d2gl {

GLuint current_binded_fbo = 0;

FrameBuffer::FrameBuffer(const FrameBufferCreateInfo& info)
	: m_width(info.size.x), m_height(info.size.y)
{
	glGenFramebuffers(1, &m_id);
	glBindFramebuffer(GL_FRAMEBUFFER, m_id);

	size_t attachment_count = info.attachments.size();
	GLenum* attachments = new GLenum[attachment_count];

	TextureCreateInfo texture_ci;
	texture_ci.size = info.size;

	for (size_t i = 0; i < attachment_count; i++) {
		texture_ci.slot = info.attachments[i].slot;
		texture_ci.format = info.attachments[i].format;
		texture_ci.min_filter = info.attachments[i].min_filter;
		texture_ci.mag_filter = info.attachments[i].mag_filter;
		m_textures.push_back(std::make_unique<Texture>(texture_ci));

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, m_textures[i]->getId(), 0);
		attachments[i] = GL_COLOR_ATTACHMENT0 + i;

		m_clear_colors.push_back(info.attachments[i].clear_color);
	}

	glDrawBuffers(attachment_count, attachments);
	delete[] attachments;

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		error_log("Framebuffer is not complete!");

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	current_binded_fbo = 0;
}

FrameBuffer::~FrameBuffer()
{
	glDeleteFramebuffers(1, &m_id);
}

void FrameBuffer::bind(bool clear)
{
	if (current_binded_fbo == m_id) {
		if (clear)
			clearBuffer();
		return;
	}

	App.context->flushVertices();

	glBindFramebuffer(GL_FRAMEBUFFER, m_id);
	current_binded_fbo = m_id;

	if (clear)
		clearBuffer();
}

void FrameBuffer::unBind()
{
	if (current_binded_fbo == 0)
		return;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	current_binded_fbo = 0;
}

void FrameBuffer::clearBuffer()
{
	const auto count = m_clear_colors.size();
	if (count == 1) {
		const auto color = m_clear_colors[0];
		glClearColor(color[0], color[1], color[2], color[3]);
		glClear(GL_COLOR_BUFFER_BIT);
	} else {
		for (size_t i = 0; i < count; i++)
			glClearBufferfv(GL_COLOR, i, m_clear_colors[i].data());
	}
}

}