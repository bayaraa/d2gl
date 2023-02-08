#include "pch.h"
#include "utils.h"
#include "context.h"

namespace d2gl::vulkan::utils {

vk::AttachmentDescription attachmentDescription(bool clear, vk::Format format, vk::ImageLayout final_layout)
{
	return vk::AttachmentDescription()
	  .setFormat(format)
	  .setSamples(vk::SampleCountFlagBits::e1)
	  .setLoadOp(clear ? vk::AttachmentLoadOp::eClear : vk::AttachmentLoadOp::eDontCare)
	  .setStoreOp(vk::AttachmentStoreOp::eStore)
	  .setStencilLoadOp(vk::AttachmentLoadOp::eLoad)
	  .setStencilStoreOp(vk::AttachmentStoreOp::eNone)
	  .setInitialLayout(vk::ImageLayout::eUndefined)
	  .setFinalLayout(final_layout);
}

vk::RenderPass createRenderPass(const std::vector<vk::AttachmentDescription>& attachments)
{
	std::vector<vk::AttachmentReference> attachment_references;
	for (size_t i = 0; i < attachments.size(); i++)
		attachment_references.push_back(vk::AttachmentReference().setAttachment(i).setLayout(vk::ImageLayout::eColorAttachmentOptimal));

	auto const subpass = vk::SubpassDescription().setPipelineBindPoint(vk::PipelineBindPoint::eGraphics).setColorAttachments(attachment_references);

	const std::array<vk::SubpassDependency, 1> dependencies = {
		vk::SubpassDependency()
		  .setSrcSubpass(VK_SUBPASS_EXTERNAL)
		  .setDstSubpass(0)
		  .setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
		  .setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
		  .setSrcAccessMask(vk::AccessFlagBits())
		  .setDstAccessMask(vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eColorAttachmentRead)
		  .setDependencyFlags(vk::DependencyFlags()),
	};

	VULKAN_CONTEXT_INLINE;
	const auto result_value = vkc->getDevice().createRenderPass(vk::RenderPassCreateInfo().setAttachments(attachments).setSubpasses(subpass).setDependencies(dependencies));
	assert(result_value.result == vk::Result::eSuccess);

	return result_value.value;
}

Buffer createBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties, bool mapped)
{
	Buffer buffer;
	VULKAN_CONTEXT_INLINE;

	const auto buffer_return = vkc->getDevice().createBuffer(vk::BufferCreateInfo().setSize(size).setUsage(usage).setSharingMode(vk::SharingMode::eExclusive));
	assert(buffer_return.result == vk::Result::eSuccess);
	buffer.handle = buffer_return.value;

	const auto requirements = vkc->getDevice().getBufferMemoryRequirements(buffer.handle);
	auto type_index = findMemoryType(requirements.memoryTypeBits, properties);

	const auto alloc_return = vkc->getDevice().allocateMemory(vk::MemoryAllocateInfo().setAllocationSize(requirements.size).setMemoryTypeIndex(type_index));
	assert(alloc_return.result == vk::Result::eSuccess);
	buffer.memory = alloc_return.value;

	auto result = vkc->getDevice().bindBufferMemory(buffer.handle, buffer.memory, 0);
	assert(result == vk::Result::eSuccess);

	if (mapped) {
		result = vkc->getDevice().mapMemory(buffer.memory, 0, size, {}, &buffer.memory_mapped);
		assert(result == vk::Result::eSuccess);
	}

	return buffer;
}

uint32_t findMemoryType(uint32_t type_filter, vk::MemoryPropertyFlags flags)
{
	VULKAN_CONTEXT_INLINE;
	for (uint32_t i = 0; i < vkc->getPhysicalDevice().memory_properties.memoryTypeCount; i++) {
		if ((type_filter & (1 << i)) && (vkc->getPhysicalDevice().memory_properties.memoryTypes[i].propertyFlags & flags) == flags)
			return i;
	}

	throw std::runtime_error("Failed to find suitable memory type!");
}

vk::ImageViewCreateInfo imageViewCreateInfo(vk::Image image, vk::Format format, vk::ImageViewType type, uint32_t layer_count)
{
	return vk::ImageViewCreateInfo()
	  .setImage(image)
	  .setFormat(format)
	  .setViewType(type)
	  .setSubresourceRange(vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, layer_count));
}

vk::ImageView createImageView(vk::ImageViewCreateInfo info)
{
	VULKAN_CONTEXT_INLINE;
	const auto result_value = vkc->getDevice().createImageView(info);
	assert(result_value.result == vk::Result::eSuccess);

	return result_value.value;
}

vk::FramebufferCreateInfo framebufferCreateInfo(vk::RenderPass renderpass, const std::vector<vk::ImageView>& attachments, vk::Extent2D extent)
{
	return vk::FramebufferCreateInfo()
	  .setRenderPass(renderpass)
	  .setAttachments(attachments)
	  .setAttachmentCount(attachments.size())
	  .setWidth(extent.width)
	  .setHeight(extent.height)
	  .setLayers(1);
}

vk::Framebuffer createFrameBuffer(vk::FramebufferCreateInfo info)
{
	VULKAN_CONTEXT_INLINE;
	auto const result_value = vkc->getDevice().createFramebuffer(info);
	assert(result_value.result == vk::Result::eSuccess);

	return result_value.value;
}

vk::ImageCreateInfo imageCreateInfo(glm::uvec2 size, vk::Format format, bool linear_tiling)
{
	return vk::ImageCreateInfo()
	  .setFormat(format)
	  .setImageType(vk::ImageType::e2D)
	  .setMipLevels(1)
	  .setArrayLayers(1)
	  .setTiling(linear_tiling ? vk::ImageTiling::eLinear : vk::ImageTiling::eOptimal)
	  .setInitialLayout(vk::ImageLayout::ePreinitialized)
	  .setUsage(vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled)
	  .setSamples(vk::SampleCountFlagBits::e1)
	  .setSharingMode(vk::SharingMode::eExclusive)
	  .setExtent(vk::Extent3D(size.x, size.y, 1));
}

vk::Image createImage(vk::ImageCreateInfo info)
{
	VULKAN_CONTEXT_INLINE;
	const auto result_value = vkc->getDevice().createImage(info);
	assert(result_value.result == vk::Result::eSuccess);

	return result_value.value;
}

vk::SamplerCreateInfo samplerCreateInfo(vk::Filter min_filter, vk::Filter mag_filter)
{
	return vk::SamplerCreateInfo()
	  .setMinFilter(min_filter)
	  .setMagFilter(mag_filter)
	  .setMipmapMode(vk::SamplerMipmapMode::eNearest)
	  .setAddressModeU(vk::SamplerAddressMode::eClampToEdge)
	  .setAddressModeV(vk::SamplerAddressMode::eClampToEdge)
	  .setAddressModeW(vk::SamplerAddressMode::eClampToEdge)
	  .setMipLodBias(0.0f)
	  .setAnisotropyEnable(VK_FALSE)
	  .setMaxAnisotropy(1)
	  .setCompareEnable(VK_FALSE)
	  .setCompareOp(vk::CompareOp::eNever)
	  .setMinLod(0.0f)
	  .setMaxLod(0.0f)
	  .setBorderColor(vk::BorderColor::eFloatOpaqueWhite)
	  .setUnnormalizedCoordinates(VK_FALSE);
}

vk::Sampler createSampler(vk::SamplerCreateInfo info)
{
	VULKAN_CONTEXT_INLINE;
	const auto result_value = vkc->getDevice().createSampler(info);
	assert(result_value.result == vk::Result::eSuccess);

	return result_value.value;
}

vk::ImageSubresourceRange imageSubresourceRange(uint32_t layer_count)
{
	return vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, layer_count);
}

vk::ImageMemoryBarrier imageMemoryBarrier(const vk::Image& image, vk::AccessFlags s_access, vk::AccessFlags d_access, vk::ImageLayout o_layout, vk::ImageLayout n_layout,
  vk::ImageSubresourceRange sub_resource_range, uint32_t s_queue_index, uint32_t d_queue_index)
{
	return vk::ImageMemoryBarrier()
	  .setImage(image)
	  .setSrcAccessMask(s_access)
	  .setDstAccessMask(d_access)
	  .setOldLayout(o_layout)
	  .setNewLayout(n_layout)
	  .setSrcQueueFamilyIndex(s_queue_index)
	  .setDstQueueFamilyIndex(d_queue_index)
	  .setSubresourceRange(sub_resource_range);
}

vk::DescriptorSetLayoutBinding descriptorSetLayoutBinding(uint32_t binding, vk::DescriptorType type, vk::ShaderStageFlagBits flags, uint32_t count)
{
	return vk::DescriptorSetLayoutBinding()
	  .setBinding(binding)
	  .setDescriptorType(type)
	  .setStageFlags(flags)
	  .setDescriptorCount(count)
	  .setPImmutableSamplers(nullptr);
}

vk::PipelineColorBlendAttachmentState pipelineColorBlendAttachmentState(BlendType type)
{
	auto state = vk::PipelineColorBlendAttachmentState();
	state.setColorWriteMask(vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA);

	if (type != BlendType::NoBlend) {
		state.setBlendEnable(VK_TRUE);
		state.setColorBlendOp(vk::BlendOp::eAdd);
		state.setAlphaBlendOp(vk::BlendOp::eAdd);

		switch (type) {
			case BlendType::One_Zero:
				state.setSrcColorBlendFactor(vk::BlendFactor::eOne);
				state.setDstColorBlendFactor(vk::BlendFactor::eZero);
				state.setSrcAlphaBlendFactor(vk::BlendFactor::eOne);
				state.setDstAlphaBlendFactor(vk::BlendFactor::eZero);
				break;
			case BlendType::Zero_SColor:
				state.setSrcColorBlendFactor(vk::BlendFactor::eZero);
				state.setDstColorBlendFactor(vk::BlendFactor::eSrcColor);
				state.setSrcAlphaBlendFactor(vk::BlendFactor::eZero);
				state.setDstAlphaBlendFactor(vk::BlendFactor::eSrcColor);
				break;
			case BlendType::One_One:
				state.setSrcColorBlendFactor(vk::BlendFactor::eOne);
				state.setDstColorBlendFactor(vk::BlendFactor::eOne);
				state.setSrcAlphaBlendFactor(vk::BlendFactor::eZero);
				state.setDstAlphaBlendFactor(vk::BlendFactor::eOne);
				break;
			case BlendType::SAlpha_OneMinusSAlpha:
				state.setSrcColorBlendFactor(vk::BlendFactor::eSrcAlpha);
				state.setDstColorBlendFactor(vk::BlendFactor::eOneMinusSrcAlpha);
				state.setSrcAlphaBlendFactor(vk::BlendFactor::eOne);
				state.setDstAlphaBlendFactor(vk::BlendFactor::eOneMinusSrcAlpha);
				break;
			default:
				state.setSrcColorBlendFactor(vk::BlendFactor::eSrcAlpha);
				state.setDstColorBlendFactor(vk::BlendFactor::eOneMinusSrcAlpha);
				state.setSrcAlphaBlendFactor(vk::BlendFactor::eOne);
				state.setDstAlphaBlendFactor(vk::BlendFactor::eOneMinusSrcAlpha);
		}
	}

	return state;
}

vk::PipelineShaderStageCreateInfo pipelineShaderStageCreateInfo(const std::vector<uint32_t>& code, vk::ShaderStageFlagBits stage_flag)
{
	VULKAN_CONTEXT_INLINE;
	const auto result_value = vkc->getDevice().createShaderModule(vk::ShaderModuleCreateInfo().setCodeSize(code.size() * 4).setPCode(code.data()));
	assert(result_value.result == vk::Result::eSuccess);

	return vk::PipelineShaderStageCreateInfo().setModule(result_value.value).setPName("main").setStage(stage_flag);
}

vk::VertexInputAttributeDescription vertexInputAttributeDescription(uint32_t location, uint32_t offset, vk::Format format, uint32_t binding)
{
	return vk::VertexInputAttributeDescription().setBinding(binding).setLocation(location).setOffset(offset).setFormat(format);
}

vk::Viewport viewport(float width, float height, glm::vec2 pos, glm::vec2 depth)
{
	return vk::Viewport().setWidth(width).setHeight(height).setX(pos.x).setY(pos.y).setMinDepth(depth.x).setMaxDepth(depth.y);
}

vk::Rect2D rect2D(uint32_t width, uint32_t height, glm::ivec2 offset)
{
	return vk::Rect2D(vk::Offset2D(offset.x, offset.y), vk::Extent2D(width, height));
}

vk::ClearValue clearValue(glm::vec4 color)
{
	return vk::ClearColorValue(std::array<float, 4>({ color.r, color.g, color.b, color.a }));
}

vk::DeviceMemory allocateMemory(vk::DeviceSize size, uint32_t type_index)
{
	VULKAN_CONTEXT_INLINE;
	const auto result_value = vkc->getDevice().allocateMemory(vk::MemoryAllocateInfo().setAllocationSize(size).setMemoryTypeIndex(type_index));
	assert(result_value.result == vk::Result::eSuccess);

	return result_value.value;
}

void bindImageMemory(vk::Image image, vk::DeviceMemory memory)
{
	VULKAN_CONTEXT_INLINE;
	auto result = vkc->getDevice().bindImageMemory(image, memory, 0);
	assert(result == vk::Result::eSuccess);
}

vk::Semaphore createSemaphore(vk::SemaphoreCreateInfo info)
{
	VULKAN_CONTEXT_INLINE;
	auto result_value = vkc->getDevice().createSemaphore(info);
	assert(result_value.result == vk::Result::eSuccess);

	return result_value.value;
}

vk::Fence createFence(vk::FenceCreateInfo info)
{
	VULKAN_CONTEXT_INLINE;
	auto result_value = vkc->getDevice().createFence(info);
	assert(result_value.result == vk::Result::eSuccess);

	return result_value.value;
}

bool checkLayer(const std::vector<vk::LayerProperties>& layers, const char* name)
{
	for (const auto& layer : layers) {
		if (!strcmp(name, layer.layerName))
			return true;
	}

	return false;
}

bool checkExtension(const std::vector<vk::ExtensionProperties>& extentions, const char* name)
{
	for (const auto& extention : extentions) {
		if (!strcmp(name, extention.extensionName))
			return true;
	}

	return false;
}

vk::DescriptorType getDescriptorType(DescriptorType type)
{
	switch (type) {
		case DescriptorType::ImageSampler: return vk::DescriptorType::eCombinedImageSampler;
	}
	return vk::DescriptorType::eUniformBuffer;
}

vk::ShaderStageFlagBits getShaderStage(ShaderStage stage)
{
	switch (stage) {
		case ShaderStage::Fragment: return vk::ShaderStageFlagBits::eFragment;
		case ShaderStage::Compute: return vk::ShaderStageFlagBits::eCompute;
	}
	return vk::ShaderStageFlagBits::eVertex;
}

vk::Filter getFilter(Filter filter)
{
	switch (filter) {
		case Filter::Linear: return vk::Filter::eLinear;
	}
	return vk::Filter::eNearest;
}

vk::Format getFormat(Format format)
{
	switch (format) {
		case Format::R8G8B8A8Unorm: return vk::Format::eR8G8B8A8Unorm;
		case Format::B8G8R8A8Unorm: return vk::Format::eB8G8R8A8Unorm;
		case Format::R8Srgb: return vk::Format::eR8Srgb;
		case Format::R8Unorm: return vk::Format::eR8Unorm;
	}
	return vk::Format::eR8G8B8A8Srgb;
}

vk::ImageUsageFlags getImageUsageFlags(int flags)
{
	vk::ImageUsageFlags usage_flags = (vk::ImageUsageFlags)0;
	if (flags & ImageUsageFlag::Sampled)
		usage_flags |= vk::ImageUsageFlagBits::eSampled;
	if (flags & ImageUsageFlag::TransferDst)
		usage_flags |= vk::ImageUsageFlagBits::eTransferDst;
	if (flags & ImageUsageFlag::TransferSrc)
		usage_flags |= vk::ImageUsageFlagBits::eTransferSrc;
	if (flags & ImageUsageFlag::ColorAttachment)
		usage_flags |= vk::ImageUsageFlagBits::eColorAttachment;
	return usage_flags;
}

vk::ImageLayout getImageLayout(ImageLayout layout)
{
	switch (layout) {
		case ImageLayout::TransferDstOptimal: return vk::ImageLayout::eTransferDstOptimal;
		case ImageLayout::ColorAttachmentOptimal: return vk::ImageLayout::eColorAttachmentOptimal;
	}
	return vk::ImageLayout::eShaderReadOnlyOptimal;
}

VKAPI_ATTR vk::Bool32 VKAPI_CALL debugMessageCallback(VkDebugUtilsMessageSeverityFlagBitsEXT severity, VkDebugUtilsMessageTypeFlagsEXT type, const VkDebugUtilsMessengerCallbackDataEXT* callback_data, void* user_data)
{
	switch (vk::DebugUtilsMessageSeverityFlagBitsEXT(severity)) {
		case vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose: logTrace(C_MAGENTA, false, "\nVerbose: "); break;
		case vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo: logTrace(C_BLUE, false, "\nInfo: "); break;
		case vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning: logTrace(C_YELLOW, false, "\nWarning: "); break;
		case vk::DebugUtilsMessageSeverityFlagBitsEXT::eError: logTrace(C_RED, false, "\nError: "); break;
		default: logTrace(C_RED, false, "\nUnknown: ");
	}

	logTrace(C_WHITE, false, "[%u / %s]: ", callback_data->messageIdNumber, callback_data->pMessageIdName);
	logTrace(C_GRAY, true, "%s", vk::to_string(vk::DebugUtilsMessageTypeFlagsEXT(type)).c_str());
	trace("%s", callback_data->pMessage);

	return VK_FALSE;
}

}

VKAPI_ATTR VkResult VKAPI_CALL vkCreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pMessenger)
{
	if (auto fn = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT")))
		return fn(instance, pCreateInfo, pAllocator, pMessenger);
	return VkResult::VK_ERROR_EXTENSION_NOT_PRESENT;
}

VKAPI_ATTR void VKAPI_CALL vkDestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT messenger, VkAllocationCallbacks const* pAllocator)
{
	if (auto fn = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT")))
		fn(instance, messenger, pAllocator);
}