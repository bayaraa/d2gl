#include "pch.h"
#include "context.h"
#include "utils.h"

namespace d2gl::vulkan {

Texture::Texture(const TextureCreateInfo& info)
	: VULKAN_CONTEXT_CLASS, m_width(info.size.x), m_height(info.size.y), m_layer_count(info.layer_count), m_format(utils::getFormat(info.format)), m_channel(4), m_linear_tiling(info.linear_tiling)
{
	if (info.format == Format::R8Srgb || info.format == Format::R8Unorm)
		m_channel = 1;

	m_image = utils::createImage(utils::imageCreateInfo(info.size, m_format).setArrayLayers(m_layer_count).setUsage(utils::getImageUsageFlags(info.usage_flags)));

	const auto requirements = vkc->m_device.getImageMemoryRequirements(m_image);
	vk::MemoryPropertyFlags memory_flags = vk::MemoryPropertyFlagBits::eDeviceLocal | vk::MemoryPropertyFlagBits::eHostVisible;
	auto type_index = utils::findMemoryType(requirements.memoryTypeBits, memory_flags);

	m_memory = utils::allocateMemory(requirements.size, type_index);
	utils::bindImageMemory(m_image, m_memory);

	vk::DeviceSize image_size = m_width * m_height * m_channel * m_layer_count;
	const auto result = vkc->m_device.mapMemory(m_memory, 0, image_size, {}, &m_memory_mapped);
	assert(result == vk::Result::eSuccess);

	vk::ImageViewType view_type = m_layer_count == 1 ? vk::ImageViewType::e2D : vk::ImageViewType::e2DArray;
	m_image_view = utils::createImageView(utils::imageViewCreateInfo(m_image, m_format, view_type, m_layer_count));

	auto sampler_create_info = utils::samplerCreateInfo(utils::getFilter(info.min_filter), utils::getFilter(info.mag_filter));
	m_sampler = utils::createSampler(sampler_create_info);

	auto memory_barrier = utils::imageMemoryBarrier(m_image, vk::AccessFlagBits(), vk::AccessFlagBits::eShaderRead | vk::AccessFlagBits::eInputAttachmentRead,
	  vk::ImageLayout::ePreinitialized, utils::getImageLayout(info.layout), utils::imageSubresourceRange(m_layer_count));

	auto command_buffer = vkc->beginCommandBuffer();
	command_buffer.pipelineBarrier(vk::PipelineStageFlagBits::eTopOfPipe, vk::PipelineStageFlagBits::eFragmentShader, vk::DependencyFlagBits(), {}, {}, memory_barrier);
	vkc->submitCommandBuffer();
}

Texture::~Texture()
{
	vkc->waitIdle();

	vkc->m_device.destroySampler(m_sampler);
	vkc->m_device.freeMemory(m_memory);
	vkc->m_device.destroyImageView(m_image_view);
	vkc->m_device.destroyImage(m_image);
}

void Texture::fill(const uint8_t* pixels, uint32_t width, uint32_t height, uint32_t offset_x, uint32_t offset_y, uint32_t layer)
{
	if (m_linear_tiling) {
		const uint32_t width_pix = m_width * m_channel;
		const uint32_t offset = m_width * m_height * layer * m_channel + offset_y * width_pix;

		const uint32_t col_offset = offset_x * m_channel;
		const uint32_t copy_size = width * m_channel;

		for (size_t i = 0; i < height; i++) {
			uint32_t pix_offset = offset + i * width_pix + col_offset;
			memcpy((uint8_t*)m_memory_mapped + pix_offset, pixels + i * copy_size, copy_size);
		}
	} else {
		// auto result = m_command.begin(vk::CommandBufferBeginInfo());
		// assert(result == vk::Result::eSuccess);

		// m_command.pipelineBarrier(vk::PipelineStageFlagBits::eTopOfPipe, vk::PipelineStageFlagBits::eTransfer, vk::DependencyFlagBits(), {}, {},
		//	vk::ImageMemoryBarrier()
		//		.setSrcAccessMask(vk::AccessFlagBits())
		//		.setDstAccessMask(vk::AccessFlagBits::eTransferWrite)
		//		.setOldLayout(vk::ImageLayout::ePreinitialized)
		//		.setNewLayout(vk::ImageLayout::eShaderReadOnlyOptimal)
		//		.setSrcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
		//		.setDstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
		//		.setImage(m_texture_image)
		//		.setSubresourceRange(vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1)));

		// result = m_command.end();
		// assert(result == vk::Result::eSuccess);

		// auto fence_return = m_device.createFence(vk::FenceCreateInfo());
		// assert(fence_return.result == vk::Result::eSuccess);
		// auto fence = fence_return.value;

		// auto submit_result = m_graphic_q.submit(vk::SubmitInfo().setCommandBuffers(m_command), fence);
		// assert(submit_result == vk::Result::eSuccess);

		// result = m_device.waitForFences(fence, VK_TRUE, UINT64_MAX);
		// assert(result == vk::Result::eSuccess);

		// memcpy(m_texture_mapped, pixels, static_cast<size_t>(image_size));
	}
}

void Texture::fillArray(const uint8_t* pixels, uint32_t div_x, uint32_t div_y)
{
	// auto memory_barrier = utils::imageMemoryBarrier(m_image, vk::AccessFlagBits(), vk::AccessFlagBits::eTransferWrite,
	//   vk::ImageLayout::ePreinitialized, vk::ImageLayout::eShaderReadOnlyOptimal, utils::imageSubresourceRange(m_layer_count));

	// auto command_buffer = vkc->beginCommandBuffer();
	// command_buffer.pipelineBarrier(vk::PipelineStageFlagBits::eTopOfPipe, vk::PipelineStageFlagBits::eTransfer, vk::DependencyFlagBits(), {}, {}, memory_barrier);
	// vkc->submitCommandBuffer();

	vk::DeviceSize image_size = m_width * m_height * m_channel * m_layer_count;
	memcpy(m_memory_mapped, pixels, static_cast<size_t>(image_size));
}

vk::DescriptorImageInfo Texture::descriptorImageInfo(vk::ImageLayout image_layout)
{
	return vk::DescriptorImageInfo().setSampler(m_sampler).setImageView(m_image_view).setImageLayout(image_layout);
}

}