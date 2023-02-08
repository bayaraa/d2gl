#include "pch.h"
#include "context.h"
#include "utils.h"

namespace d2gl::vulkan {

RenderPass::RenderPass(const RenderPassCreateInfo& info)
	: VULKAN_CONTEXT_CLASS, m_render_area(vk::Rect2D({ info.offset.x, info.offset.y }, { info.size.x, info.size.y }))
{
	std::vector<vk::AttachmentDescription> attachment_descriptions;

	for (size_t i = 0; i < info.attachments.size(); i++) {
		auto attachment_description = utils::attachmentDescription(info.attachments[i].clear);
		attachment_descriptions.push_back(attachment_description);

		const auto clear_color = vk::ClearColorValue(info.attachments[i].clear_color);
		m_clear_values.push_back(clear_color);
	}
	m_renderpass = utils::createRenderPass(attachment_descriptions);
}

RenderPass::~RenderPass()
{
	vkc->waitIdle();

	vkc->m_device.destroyRenderPass(m_renderpass);
}

}