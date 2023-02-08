#pragma once

#include "graphic/render_pass.h"

namespace d2gl::opengl {

struct RenderArea {
	glm::ivec2 offset = { 0, 0 };
	glm::uvec2 size = { 0, 0 };
};

class RenderPass final : public d2gl::RenderPass {
	RenderArea m_render_area;
	std::vector<RenderPassAttachment> m_attachments;

public:
	RenderPass(const RenderPassCreateInfo& info);
	~RenderPass() = default;

public:
	inline const RenderArea& getRenderArea() const { return m_render_area; }
	inline const std::vector<RenderPassAttachment>& getAttachments() const { return m_attachments; }
};

}