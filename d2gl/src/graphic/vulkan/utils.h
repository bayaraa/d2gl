#pragma once

#include "buffer.h"

namespace d2gl::vulkan::utils {

vk::AttachmentDescription attachmentDescription(bool clear = true, vk::Format format = vk::Format::eR8G8B8A8Unorm, vk::ImageLayout final_layout = vk::ImageLayout::eShaderReadOnlyOptimal);
vk::RenderPass createRenderPass(const std::vector<vk::AttachmentDescription>& attachments);

Buffer createBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties, bool mapped = true);
uint32_t findMemoryType(uint32_t type_filter, vk::MemoryPropertyFlags flags);

vk::ImageViewCreateInfo imageViewCreateInfo(vk::Image image, vk::Format format = vk::Format::eR8G8B8A8Unorm, vk::ImageViewType type = vk::ImageViewType::e2D, uint32_t layer_count = 1);
vk::ImageView createImageView(vk::ImageViewCreateInfo info = {});

vk::FramebufferCreateInfo framebufferCreateInfo(vk::RenderPass renderpass, const std::vector<vk::ImageView>& attachments, vk::Extent2D extent);
vk::Framebuffer createFrameBuffer(vk::FramebufferCreateInfo info = {});

vk::ImageCreateInfo imageCreateInfo(glm::uvec2 size, vk::Format format = vk::Format::eR8G8B8A8Unorm, bool linear_tiling = true);
vk::Image createImage(vk::ImageCreateInfo info = {});

vk::SamplerCreateInfo samplerCreateInfo(vk::Filter min_filter = vk::Filter::eNearest, vk::Filter mag_filter = vk::Filter::eNearest);
vk::Sampler createSampler(vk::SamplerCreateInfo info = {});

vk::ImageSubresourceRange imageSubresourceRange(uint32_t layer_count = 1);
vk::ImageMemoryBarrier imageMemoryBarrier(const vk::Image& image, vk::AccessFlags s_access, vk::AccessFlags d_access, vk::ImageLayout o_layout, vk::ImageLayout n_layout,
  vk::ImageSubresourceRange sub_resource_range, uint32_t s_queue_index = VK_QUEUE_FAMILY_IGNORED, uint32_t d_queue_index = VK_QUEUE_FAMILY_IGNORED);

vk::DescriptorSetLayoutBinding descriptorSetLayoutBinding(uint32_t binding, vk::DescriptorType type, vk::ShaderStageFlagBits flags, uint32_t count = 1);

vk::PipelineColorBlendAttachmentState pipelineColorBlendAttachmentState(BlendType type);
vk::PipelineShaderStageCreateInfo pipelineShaderStageCreateInfo(const std::vector<uint32_t>& code, vk::ShaderStageFlagBits stage_flag);
vk::VertexInputAttributeDescription vertexInputAttributeDescription(uint32_t location, uint32_t offset, vk::Format format, uint32_t binding = 0);
//
vk::Viewport viewport(float width = 0.0f, float height = 0.0f, glm::vec2 pos = { 0.0f, 0.0f }, glm::vec2 depth = { 0.0f, 1.0f });
vk::Rect2D rect2D(uint32_t width, uint32_t height, glm::ivec2 offset = { 0, 0 });
vk::ClearValue clearValue(glm::vec4 color = { 0.0f, 0.0f, 0.0f, 1.0f });

vk::DeviceMemory allocateMemory(vk::DeviceSize size, uint32_t type_index);
void bindImageMemory(vk::Image image, vk::DeviceMemory memory);

vk::Semaphore createSemaphore(vk::SemaphoreCreateInfo info = {});
vk::Fence createFence(vk::FenceCreateInfo info = {});

bool checkLayer(const std::vector<vk::LayerProperties>& layers, const char* name);
bool checkExtension(const std::vector<vk::ExtensionProperties>& extentions, const char* name);

vk::DescriptorType getDescriptorType(DescriptorType type);
vk::ShaderStageFlagBits getShaderStage(ShaderStage stage);
vk::Filter getFilter(Filter filter);
vk::Format getFormat(Format format);
vk::ImageUsageFlags getImageUsageFlags(int flags);
vk::ImageLayout getImageLayout(ImageLayout layout);

VKAPI_ATTR vk::Bool32 VKAPI_CALL debugMessageCallback(VkDebugUtilsMessageSeverityFlagBitsEXT severity, VkDebugUtilsMessageTypeFlagsEXT type, const VkDebugUtilsMessengerCallbackDataEXT* callback_data, void* user_data);

}