#include "pch.h"
#include "context.h"
#include "utils.h"

namespace d2gl::vulkan {

FrameBuffer::FrameBuffer(const FrameBufferCreateInfo& info)
	: VULKAN_CONTEXT_CLASS
{
	const auto renderpass = static_cast<RenderPass*>(info.renderpass);
	const auto render_area = renderpass->getRenderArea();
	m_extent = render_area.extent;

	std::vector<vk::ImageView> image_views;
	m_resources.resize(info.attachments.size());

	for (size_t i = 0; i < info.attachments.size(); i++) {
		auto image_create_info = utils::imageCreateInfo({ m_extent.width, m_extent.height });
		m_resources[i].image = utils::createImage(image_create_info.setUsage(utils::getImageUsageFlags(info.attachments[i].usage_flags)));

		const auto requirements = vkc->m_device.getImageMemoryRequirements(m_resources[i].image);
		const auto type_index = utils::findMemoryType(requirements.memoryTypeBits, vk::MemoryPropertyFlagBits::eDeviceLocal);
		m_resources[i].image_memory = utils::allocateMemory(requirements.size, type_index);
		utils::bindImageMemory(m_resources[i].image, m_resources[i].image_memory);

		m_resources[i].image_view = utils::createImageView(utils::imageViewCreateInfo(m_resources[i].image));
		image_views.push_back(m_resources[i].image_view);

		const auto sampler_create_info = utils::samplerCreateInfo(utils::getFilter(info.attachments[i].min_filter), utils::getFilter(info.attachments[i].mag_filter));
		m_resources[i].sampler = utils::createSampler(sampler_create_info);

		auto memory_barrier = utils::imageMemoryBarrier(m_resources[i].image, vk::AccessFlagBits(), vk::AccessFlagBits::eShaderRead | vk::AccessFlagBits::eInputAttachmentRead,
		  vk::ImageLayout::ePreinitialized, utils::getImageLayout(ImageLayout::ShaderReadOnlyOptimal), utils::imageSubresourceRange());

		auto command_buffer = vkc->beginCommandBuffer();
		command_buffer.pipelineBarrier(vk::PipelineStageFlagBits::eTopOfPipe, vk::PipelineStageFlagBits::eFragmentShader, vk::DependencyFlagBits(), {}, {}, memory_barrier);
		vkc->submitCommandBuffer();
	}

	m_framebuffer = utils::createFrameBuffer(utils::framebufferCreateInfo(renderpass->getRenderPass(), image_views, m_extent));
}

FrameBuffer::~FrameBuffer()
{
	vkc->waitIdle();

	for (auto& resource : m_resources) {
		vkc->m_device.freeMemory(resource.image_memory);
		vkc->m_device.destroyImageView(resource.image_view);
		vkc->m_device.destroyImage(resource.image);
		vkc->m_device.destroySampler(resource.sampler);
	}
	vkc->m_device.destroyFramebuffer(m_framebuffer);
}

vk::DescriptorImageInfo FrameBuffer::descriptorImageInfo(uint32_t index, vk::ImageLayout image_layout)
{
	return vk::DescriptorImageInfo().setSampler(m_resources[index].sampler).setImageView(m_resources[index].image_view).setImageLayout(image_layout);
}

}