#include "pch.h"
#include "context.h"
#include "utils.h"

namespace d2gl::opengl {

FrameBuffer::FrameBuffer(const FrameBufferCreateInfo& info)
{
	glGenFramebuffers(1, &m_id);
	glBindFramebuffer(GL_FRAMEBUFFER, m_id);

	const auto renderpass = static_cast<RenderPass*>(info.renderpass);
	const auto render_area = renderpass->getRenderArea();
	m_width = render_area.size.x;
	m_height = render_area.size.y;

	size_t attachment_count = info.attachments.size();
	GLenum* attachments = new GLenum[attachment_count];

	TextureCreateInfo texture_ci;
	texture_ci.size = render_area.size;

	for (size_t i = 0; i < attachment_count; i++) {
		texture_ci.slot = info.attachments[i].slot;
		texture_ci.min_filter = info.attachments[i].min_filter;
		texture_ci.mag_filter = info.attachments[i].mag_filter;
		m_textures.push_back(std::make_unique<Texture>(texture_ci));

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, m_textures[i]->getId(), 0);
		attachments[i] = GL_COLOR_ATTACHMENT0 + i;
	}

	glDrawBuffers(attachment_count, attachments);
	delete[] attachments;

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		error("Framebuffer is not complete!");

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

FrameBuffer::~FrameBuffer()
{
	glDeleteFramebuffers(1, &m_id);
}

}