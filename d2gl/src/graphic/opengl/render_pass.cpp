#include "pch.h"
#include "context.h"
#include "utils.h"

namespace d2gl::opengl {

RenderPass::RenderPass(const RenderPassCreateInfo& info)
	: m_render_area({ info.offset, info.size }), m_attachments(info.attachments)
{}

}