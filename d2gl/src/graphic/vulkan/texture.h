#pragma once

#include "graphic/texture.h"

namespace d2gl::vulkan {

class Context;

class Texture final : public d2gl::Texture {
	Context* vkc;
	vk::Image m_image;
	vk::ImageView m_image_view;
	vk::Sampler m_sampler;
	vk::Format m_format;
	uint32_t m_width, m_height, m_channel, m_layer_count;
	bool m_linear_tiling;
	vk::DeviceMemory m_memory;
	void* m_memory_mapped = nullptr;

	friend class Pipeline;

public:
	Texture(const TextureCreateInfo& info);
	~Texture();

	void fill(const uint8_t* pixels, uint32_t width, uint32_t height, uint32_t offset_x = 0, uint32_t offset_y = 0, uint32_t layer = 0);
	void fillArray(const uint8_t* pixels, uint32_t div_x = 1, uint32_t div_y = 1);

private:
	vk::DescriptorImageInfo descriptorImageInfo(vk::ImageLayout image_layout = vk::ImageLayout::eShaderReadOnlyOptimal);

public:
	inline const vk::Image& getImage() const { return m_image; }
	inline const uint32_t& getWidth() const { return m_width; }
	inline const uint32_t& getHeight() const { return m_height; }
};

}