#pragma once

#include "graphic/frame_buffer.h"

namespace d2gl::vulkan {

class Context;
class Pipeline;

struct FrameBufferResource {
	vk::Image image;
	vk::ImageView image_view;
	vk::DeviceMemory image_memory;
	vk::Sampler sampler;
};

class FrameBuffer final : public d2gl::FrameBuffer {
	Context* vkc;
	vk::Extent2D m_extent;
	vk::Framebuffer m_framebuffer;
	std::vector<FrameBufferResource> m_resources;

	friend class Pipeline;

public:
	FrameBuffer(const FrameBufferCreateInfo& info);
	~FrameBuffer();

private:
	vk::DescriptorImageInfo descriptorImageInfo(uint32_t index = 0, vk::ImageLayout image_layout = vk::ImageLayout::eShaderReadOnlyOptimal);

public:
	inline const vk::Framebuffer& getFrameBuffer() const { return m_framebuffer; }
	inline const vk::Extent2D& getExtent() const { return m_extent; }
	inline const vk::Image& getImage(uint32_t index = 0) const { return m_resources[index].image; }
};

}