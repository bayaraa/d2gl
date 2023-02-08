#pragma once

#include "graphic/render_pass.h"

namespace d2gl::vulkan {

class Context;

class RenderPass final : public d2gl::RenderPass {
	Context* vkc;
	vk::RenderPass m_renderpass;
	vk::Rect2D m_render_area;
	std::vector<vk::ClearValue> m_clear_values;

public:
	RenderPass(const RenderPassCreateInfo& info);
	~RenderPass();

public:
	inline const vk::RenderPass& getRenderPass() const { return m_renderpass; }
	inline const vk::Rect2D& getRenderArea() const { return m_render_area; }
	inline const std::vector<vk::ClearValue>& getClearValues() const { return m_clear_values; }
};

}