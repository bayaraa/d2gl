#include "pch.h"
#include "context.h"
#include "utils.h"

#include <imgui/imgui.h>
#include <imgui/imgui_impl_vulkan.h>
#include <imgui/imgui_impl_win32.h>

namespace d2gl::vulkan {

bool Context::init()
{
	char version_str[50];
	const auto version_return = vk::enumerateInstanceVersion();
	uint32_t version = version_return.value;

	sprintf_s(version_str, "%d.%d.%d", VK_API_VERSION_MAJOR(version), VK_API_VERSION_MINOR(version), VK_API_VERSION_PATCH(version));
	App.version = version_str;
	trace("Vulkan: %s", version_str);

	std::vector<const char*> enabled_instance_layers;

	if (App.debug) {
		const auto instance_layers_return = vk::enumerateInstanceLayerProperties();
		assert(instance_layers_return.result == vk::Result::eSuccess);

		const char* validation_layer = "VK_LAYER_KHRONOS_validation";
		if (utils::checkLayer(instance_layers_return.value, validation_layer)) {
			enabled_instance_layers.push_back(validation_layer);
			trace("%s found.", validation_layer);
		}
	}

	std::vector<const char*> enabled_instance_extensions;
	const auto instance_extensions_return = vk::enumerateInstanceExtensionProperties();
	assert(instance_extensions_return.result == vk::Result::eSuccess);

	if (utils::checkExtension(instance_extensions_return.value, VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME))
		enabled_instance_extensions.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);

	if (App.debug && utils::checkExtension(instance_extensions_return.value, VK_EXT_DEBUG_UTILS_EXTENSION_NAME)) {
		enabled_instance_extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		trace("%s found.", VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		m_debug_extension_enabled = true;
	}

	bool portability_enumeration_active = false;
	if (utils::checkExtension(instance_extensions_return.value, VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME)) {
		enabled_instance_extensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
		portability_enumeration_active = true;
	}

	if (!utils::checkExtension(instance_extensions_return.value, VK_KHR_SURFACE_EXTENSION_NAME)) {
		error("Failed to find the " VK_KHR_SURFACE_EXTENSION_NAME " extension.");
		return false;
	}
	enabled_instance_extensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);

	if (!utils::checkExtension(instance_extensions_return.value, VK_KHR_WIN32_SURFACE_EXTENSION_NAME)) {
		error("Failed to find the " VK_KHR_WIN32_SURFACE_EXTENSION_NAME " extension.");
		return false;
	}
	enabled_instance_extensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);

	auto debug_utils_create_info = vk::DebugUtilsMessengerCreateInfoEXT();
	if (m_debug_extension_enabled) {
		debug_utils_create_info.setMessageSeverity(vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError);
		debug_utils_create_info.setMessageType(vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation);
		debug_utils_create_info.setPfnUserCallback(&utils::debugMessageCallback);
	}

	// Instance creation
	auto const app_info = vk::ApplicationInfo().setPApplicationName(App.app_name).setApplicationVersion(0).setPEngineName(App.engine_name).setEngineVersion(0);
	auto const instance_create_info =
	  vk::InstanceCreateInfo()
		.setFlags(portability_enumeration_active ? vk::InstanceCreateFlagBits::eEnumeratePortabilityKHR : static_cast<vk::InstanceCreateFlagBits>(0))
		.setPNext(m_debug_extension_enabled ? &debug_utils_create_info : nullptr)
		.setPApplicationInfo(&app_info)
		.setPEnabledLayerNames(enabled_instance_layers)
		.setPEnabledExtensionNames(enabled_instance_extensions);
	auto instance_return = vk::createInstance(instance_create_info);

	if (instance_return.result != vk::Result::eSuccess) {
		if (instance_return.result == vk::Result::eErrorIncompatibleDriver)
			error("Cannot find a compatible Vulkan installable client driver (ICD).");
		else if (instance_return.result == vk::Result::eErrorExtensionNotPresent)
			error("Cannot find a specified extension library.");
		else
			error("vkCreateInstance failed.");
		return false;
	}
	m_instance = instance_return.value;

	if (m_debug_extension_enabled) {
		const auto debug_messenger_return = m_instance.createDebugUtilsMessengerEXT(debug_utils_create_info);
		assert(debug_messenger_return.result == vk::Result::eSuccess);
		m_debug_extension = debug_messenger_return.value;
	}

	// Physical Device selection
	const auto physical_device_return = m_instance.enumeratePhysicalDevices();
	assert(physical_device_return.result == vk::Result::eSuccess);
	const auto physical_devices = physical_device_return.value;

	if (physical_devices.size() <= 0) {
		error("vkEnumeratePhysicalDevices reported zero accessible devices.");
		return false;
	}
	if (App.gpu_number >= 0 && App.gpu_number >= physical_devices.size()) {
		error("GPU %d specified is not present, GPU count = %zu", App.gpu_number, physical_devices.size());
		return false;
	}
	if (App.gpu_number == -1) {
		constexpr uint32_t device_type_count = static_cast<uint32_t>(vk::PhysicalDeviceType::eCpu) + 1;
		std::array<uint32_t, device_type_count> count_device_type = {};

		for (uint32_t i = 0; i < physical_devices.size(); i++) {
			const auto phys_device_props = physical_devices[i].getProperties();
			assert(phys_device_props.deviceType <= vk::PhysicalDeviceType::eCpu);
			count_device_type[static_cast<int>(phys_device_props.deviceType)]++;
		}

		std::array<vk::PhysicalDeviceType, device_type_count> const device_type_preference = {
			vk::PhysicalDeviceType::eDiscreteGpu, vk::PhysicalDeviceType::eIntegratedGpu,
			vk::PhysicalDeviceType::eVirtualGpu, vk::PhysicalDeviceType::eCpu, vk::PhysicalDeviceType::eOther
		};

		vk::PhysicalDeviceType search_for_device_type = vk::PhysicalDeviceType::eDiscreteGpu;
		for (uint32_t i = 0; i < sizeof(device_type_preference) / sizeof(vk::PhysicalDeviceType); i++) {
			if (count_device_type[static_cast<int>(device_type_preference[i])]) {
				search_for_device_type = device_type_preference[i];
				break;
			}
		}

		for (uint32_t i = 0; i < physical_devices.size(); i++) {
			const auto phys_device_props = physical_devices[i].getProperties();
			if (phys_device_props.deviceType == search_for_device_type) {
				App.gpu_number = i;
				break;
			}
		}
	}

	assert(App.gpu_number >= 0);
	m_physical_device.handle = physical_devices[App.gpu_number];
	m_physical_device.properties = m_physical_device.handle.getProperties();
	trace("GPU %d: %s, type: %s", App.gpu_number, m_physical_device.properties.deviceName.data(), vk::to_string(m_physical_device.properties.deviceType).c_str());

	m_physical_device.features = m_physical_device.handle.getFeatures();
	if (m_physical_device.features.independentBlend)
		m_physical_device.enabled_features.independentBlend = VK_TRUE;

	m_physical_device.memory_properties = m_physical_device.handle.getMemoryProperties();

	std::vector<const char*> enabled_device_extensions;
	const auto device_extension_return = m_physical_device.handle.enumerateDeviceExtensionProperties();
	assert(device_extension_return.result == vk::Result::eSuccess);

	if (!utils::checkExtension(device_extension_return.value, VK_KHR_SWAPCHAIN_EXTENSION_NAME)) {
		error("Failed to find the " VK_KHR_SWAPCHAIN_EXTENSION_NAME " extension.");
		return false;
	}
	enabled_device_extensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

	if (utils::checkExtension(device_extension_return.value, "VK_KHR_portability_subset"))
		enabled_device_extensions.push_back("VK_KHR_portability_subset");

	// Win32 Surface creation
	createSurface();

	// Logical Devic & Queues creation
	uint32_t graphic_queue_indexes = UINT32_MAX;
	uint32_t present_queue_indexes = UINT32_MAX;

	const auto queue_family_props = m_physical_device.handle.getQueueFamilyProperties();
	assert(queue_family_props.size() >= 1);

	std::vector<uint32_t> supports_present;
	for (size_t i = 0; i < queue_family_props.size(); i++) {
		const auto supports = m_physical_device.handle.getSurfaceSupportKHR(i, m_surface.handle);
		assert(supports.result == vk::Result::eSuccess);
		supports_present.push_back(supports.value);
	}

	for (size_t i = 0; i < queue_family_props.size(); i++) {
		if (queue_family_props[i].queueFlags & vk::QueueFlagBits::eGraphics) {
			if (graphic_queue_indexes == UINT32_MAX)
				graphic_queue_indexes = i;

			if (supports_present[i]) {
				graphic_queue_indexes = present_queue_indexes = i;
				break;
			}
		}
	}

	if (present_queue_indexes == UINT32_MAX) {
		for (size_t i = 0; i < queue_family_props.size(); i++) {
			if (supports_present[i]) {
				present_queue_indexes = i;
				break;
			}
		}
	}

	if (graphic_queue_indexes == UINT32_MAX || present_queue_indexes == UINT32_MAX) {
		error("Could not find both graphics and present queues.");
		return false;
	}

	m_queue.graphic_index = graphic_queue_indexes;
	m_queue.present_index = present_queue_indexes;
	m_queue.is_separate = graphic_queue_indexes != present_queue_indexes;

	float priorities = 0.0f;
	std::vector<vk::DeviceQueueCreateInfo> queue_create_infos;
	queue_create_infos.push_back(vk::DeviceQueueCreateInfo().setQueueFamilyIndex(m_queue.graphic_index).setQueuePriorities(priorities));
	if (m_queue.is_separate)
		queue_create_infos.push_back(vk::DeviceQueueCreateInfo().setQueueFamilyIndex(m_queue.present_index).setQueuePriorities(priorities));

	const auto device_create_info = vk::DeviceCreateInfo().setQueueCreateInfos(queue_create_infos).setPEnabledExtensionNames(enabled_device_extensions).setPEnabledFeatures(&m_physical_device.enabled_features);
	const auto device_return = m_physical_device.handle.createDevice(device_create_info);
	assert(device_return.result == vk::Result::eSuccess);
	m_device = device_return.value;

	m_queue.graphic = m_device.getQueue(m_queue.graphic_index, 0);
	m_queue.present = m_queue.is_separate ? m_device.getQueue(m_queue.present_index, 0) : m_queue.graphic;

	// Command Pool & Buffer creation
	auto cmd_pool_create_info = vk::CommandPoolCreateInfo().setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer);
	auto cmd_pool_return = m_device.createCommandPool(cmd_pool_create_info.setQueueFamilyIndex(m_queue.graphic_index));
	assert(cmd_pool_return.result == vk::Result::eSuccess);
	m_command.pool = cmd_pool_return.value;

	if (m_queue.is_separate) {
		cmd_pool_return = m_device.createCommandPool(cmd_pool_create_info.setQueueFamilyIndex(m_queue.present_index));
		assert(cmd_pool_return.result == vk::Result::eSuccess);
		m_command.pool_present = cmd_pool_return.value;
	}

	auto cmd_buffer_return = m_device.allocateCommandBuffers(vk::CommandBufferAllocateInfo().setCommandPool(m_command.pool).setLevel(vk::CommandBufferLevel::ePrimary).setCommandBufferCount(1));
	assert(cmd_buffer_return.result == vk::Result::eSuccess);
	m_command.buffer = cmd_buffer_return.value[0];

	// Descriptor Pool creation
	std::vector<vk::DescriptorPoolSize> descriptor_pool_sizes = {
		{ vk::DescriptorType::eSampler, 100 },
		{ vk::DescriptorType::eCombinedImageSampler, 100 },
		{ vk::DescriptorType::eSampledImage, 100 },
		{ vk::DescriptorType::eStorageImage, 100 },
		{ vk::DescriptorType::eUniformTexelBuffer, 100 },
		{ vk::DescriptorType::eStorageTexelBuffer, 100 },
		{ vk::DescriptorType::eUniformBuffer, 100 },
		{ vk::DescriptorType::eStorageBuffer, 100 },
		{ vk::DescriptorType::eUniformBufferDynamic, 100 },
		{ vk::DescriptorType::eStorageBufferDynamic, 100 },
		{ vk::DescriptorType::eInputAttachment, 100 },
	};

	auto result_value = m_device.createDescriptorPool(
	  vk::DescriptorPoolCreateInfo()
		.setFlags(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet)
		.setPoolSizes(descriptor_pool_sizes)
		.setMaxSets(100 * descriptor_pool_sizes.size()));
	assert(result_value.result == vk::Result::eSuccess);
	m_descriptor_pool = result_value.value;

	// Present RenderPass & Swapchain creation
	m_renderpass = utils::createRenderPass({ utils::attachmentDescription(true, m_surface.format.format, vk::ImageLayout::ePresentSrcKHR) });
	createSwapchain();

	// Index & Vertex buffers & frame resources creation
	uint32_t offset = 0;
	uint32_t* indices = new uint32_t[MAX_INDICES];
	for (size_t i = 0; i < MAX_INDICES; i += 6) {
		indices[i + 0] = offset + 0;
		indices[i + 1] = offset + 1;
		indices[i + 2] = offset + 2;
		indices[i + 3] = offset + 2;
		indices[i + 4] = offset + 3;
		indices[i + 5] = offset + 0;
		offset += 4;
	}

	vk::DeviceSize buffer_size = sizeof(indices[0]) * MAX_INDICES;
	auto staging_buffer = utils::createBuffer(buffer_size, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostCoherent | vk::MemoryPropertyFlagBits::eHostVisible);

	memcpy(staging_buffer.memory_mapped, indices, static_cast<size_t>(buffer_size));
	m_device.unmapMemory(staging_buffer.memory);
	delete[] indices;

	m_index_buffer = utils::createBuffer(buffer_size, vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer, vk::MemoryPropertyFlagBits::eDeviceLocal, false);

	const auto command_buffer = beginCommandBuffer();
	const auto copy_region = vk::BufferCopy().setSize(buffer_size);
	command_buffer.copyBuffer(staging_buffer.handle, m_index_buffer.handle, 1, &copy_region);
	submitCommandBuffer();

	m_device.freeMemory(staging_buffer.memory);
	m_device.destroyBuffer(staging_buffer.handle);

	m_frame_resources.resize(getMaxFrames());
	for (auto& resource : m_frame_resources) {
		resource.fence = utils::createFence(vk::FenceCreateInfo().setFlags(vk::FenceCreateFlagBits::eSignaled));
		resource.acquired_sem = utils::createSemaphore();
		resource.complete_sem = utils::createSemaphore();
		if (m_queue.is_separate)
			resource.ownership_sem = utils::createSemaphore();

		buffer_size = sizeof(Vertex) * MAX_VERTICES;
		resource.vertex_buffer = utils::createBuffer(buffer_size, vk::BufferUsageFlagBits::eVertexBuffer, vk::MemoryPropertyFlagBits::eDeviceLocal | vk::MemoryPropertyFlagBits::eHostVisible);
	}

	// ImGui
	imguiInit();

	return true;
}

Context::~Context()
{
	if (!m_instance)
		return;

	if (m_device) {
		waitIdle();

		imguiDestroy();

		for (auto& resource : m_frame_resources) {
			m_device.destroyFence(resource.fence);
			m_device.destroySemaphore(resource.acquired_sem);
			m_device.destroySemaphore(resource.complete_sem);
			if (m_queue.is_separate)
				m_device.destroySemaphore(resource.ownership_sem);

			m_device.unmapMemory(resource.vertex_buffer.memory);
			m_device.freeMemory(resource.vertex_buffer.memory);
			m_device.destroyBuffer(resource.vertex_buffer.handle);

			// m_device.unmapMemory(resource.vertex_buffer_mod.memory);
			// m_device.freeMemory(resource.vertex_buffer_mod.memory);
			// m_device.destroyBuffer(resource.vertex_buffer_mod.handle);
		}

		m_device.freeMemory(m_index_buffer.memory);
		m_device.destroyBuffer(m_index_buffer.handle);

		m_device.destroyRenderPass(m_renderpass);
		if (m_descriptor_pool)
			m_device.destroyDescriptorPool(m_descriptor_pool);

		m_device.freeCommandBuffers(m_command.pool, { m_command.buffer });
		m_device.destroyCommandPool(m_command.pool);
		if (m_command.pool_present)
			m_device.destroyCommandPool(m_command.pool_present);

		m_device.destroy();
	}

	if (m_surface.handle)
		m_instance.destroySurfaceKHR(m_surface.handle);

	if (m_debug_extension_enabled)
		m_instance.destroyDebugUtilsMessengerEXT(m_debug_extension);

	m_instance.destroy();
}

void Context::waitIdle()
{
	auto result = m_device.waitIdle();
	assert(result == vk::Result::eSuccess);
}

void Context::startFrame()
{
	const auto frame_resource = &m_frame_resources[m_frame_index];
	frame_resource->current_renderpass = nullptr;
	frame_resource->current_pipeline = nullptr;
	frame_resource->current_descriptor_set = nullptr;

	auto result = m_device.waitForFences(frame_resource->fence, VK_TRUE, UINT64_MAX);
	assert(result == vk::Result::eSuccess);

	result = m_device.resetFences({ frame_resource->fence });
	assert(result == vk::Result::eSuccess);

	result = vk::Result::eNotReady;
	do {
		result = m_device.acquireNextImageKHR(m_swapchain, UINT64_MAX, frame_resource->acquired_sem, vk::Fence(), &m_swapchain_index);
		if (result == vk::Result::eErrorOutOfDateKHR) {
			createSwapchain();
		} else if (result == vk::Result::eSuboptimalKHR) {
			// swapchain is not as optimal as it could be, but the platform's
			// presentation engine will still present the image correctly.
			// createSwapchain();
			break;
		} else if (result == vk::Result::eErrorSurfaceLostKHR) {
			createSurface();
			createSwapchain();
		} else
			assert(result == vk::Result::eSuccess);
	} while (result != vk::Result::eSuccess);

	const auto swapchain_resource = &m_swapchain_resources[m_swapchain_index];
	const auto command_buffer = swapchain_resource->command_buffer;
	command_buffer.reset();

	result = command_buffer.begin(vk::CommandBufferBeginInfo().setFlags(vk::CommandBufferUsageFlagBits::eSimultaneousUse));
	assert(result == vk::Result::eSuccess);

	command_buffer.bindIndexBuffer(m_index_buffer.handle, 0, vk::IndexType::eUint32);

	vk::DeviceSize offsets[] = { 0 };
	vk::Buffer vertex_buffers[] = { frame_resource->vertex_buffer.handle };
	command_buffer.bindVertexBuffers(0, 1, vertex_buffers, offsets);

	m_vertices.ptr = m_vertices.data.data();
	m_vertices.total_count = 0;
	m_index_count = 0;

	m_render_info.vertex_count = 0;
	m_render_info.drawcall_count = 0;
}

void Context::beginFinalPass()
{
	flushVertices();

	const auto frame_resource = &m_frame_resources[m_frame_index];
	const auto swapchain_resource = &m_swapchain_resources[m_swapchain_index];
	const auto command_buffer = swapchain_resource->command_buffer;

	if (frame_resource->current_renderpass != nullptr) {
		frame_resource->current_renderpass = nullptr;
		command_buffer.endRenderPass();
	}

	static std::array<vk::ClearValue, 1> clear_values = { utils::clearValue({ 0.0f, 0.0f, 0.0f, 1.0f }) };

	command_buffer.beginRenderPass(
	  vk::RenderPassBeginInfo()
		.setRenderPass(m_renderpass)
		.setFramebuffer(swapchain_resource->framebuffer)
		.setRenderArea(utils::rect2D(App.window.size.x, App.window.size.y))
		.setClearValues(clear_values),
	  vk::SubpassContents::eInline);
}

void Context::presentFrame()
{
	flushVertices();

	const auto frame_resource = &m_frame_resources[m_frame_index];
	const auto swapchain_resource = &m_swapchain_resources[m_swapchain_index];
	const auto command_buffer = swapchain_resource->command_buffer;

	command_buffer.endRenderPass();

	if (m_queue.is_separate) {
		command_buffer.pipelineBarrier(
		  vk::PipelineStageFlagBits::eBottomOfPipe, vk::PipelineStageFlagBits::eBottomOfPipe, vk::DependencyFlagBits(), {}, {},
		  vk::ImageMemoryBarrier()
			.setSrcAccessMask(vk::AccessFlags())
			.setDstAccessMask(vk::AccessFlags())
			.setOldLayout(vk::ImageLayout::ePresentSrcKHR)
			.setNewLayout(vk::ImageLayout::ePresentSrcKHR)
			.setSrcQueueFamilyIndex(m_queue.graphic_index)
			.setDstQueueFamilyIndex(m_queue.present_index)
			.setImage(swapchain_resource->image)
			.setSubresourceRange(vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1)));
	}

	auto result = command_buffer.end();
	assert(result == vk::Result::eSuccess);

	memcpy(frame_resource->vertex_buffer.memory_mapped, m_vertices.data.data(), sizeof(Vertex) * m_vertices.total_count);

	const vk::PipelineStageFlags pipe_stage_flags = vk::PipelineStageFlagBits::eColorAttachmentOutput;
	result = m_queue.graphic.submit(
	  vk::SubmitInfo()
		.setCommandBuffers(command_buffer)
		.setWaitDstStageMask(pipe_stage_flags)
		.setWaitSemaphores(frame_resource->acquired_sem)
		.setSignalSemaphores(frame_resource->complete_sem),
	  frame_resource->fence);
	assert(result == vk::Result::eSuccess);

	if (m_queue.is_separate) {
		result = m_queue.present.submit(
		  vk::SubmitInfo()
			.setCommandBuffers(swapchain_resource->command_buffer_present)
			.setWaitDstStageMask(pipe_stage_flags)
			.setWaitSemaphores(frame_resource->complete_sem)
			.setSignalSemaphores(frame_resource->ownership_sem));
		assert(result == vk::Result::eSuccess);
	}

	const auto present_info =
	  vk::PresentInfoKHR()
		.setWaitSemaphores(m_queue.is_separate ? frame_resource->ownership_sem : frame_resource->complete_sem)
		.setSwapchains(m_swapchain)
		.setImageIndices(m_swapchain_index);
	result = m_queue.present.presentKHR(&present_info);
	if (result == vk::Result::eErrorOutOfDateKHR) {
		createSwapchain();
	} else if (result == vk::Result::eSuboptimalKHR) {
		// if (m_surface.capabilities.currentExtent.width != m_width || m_surface.capabilities.currentExtent.height != m_height)
		// createSwapchain();
	} else if (result == vk::Result::eErrorSurfaceLostKHR) {
		createSurface();
		createSwapchain();
	} else
		assert(result == vk::Result::eSuccess);

	m_frame_index = (m_frame_index + 1) % getMaxFrames();

	m_render_info.onFrameFinish();
}

void Context::bindPipeline(const std::unique_ptr<d2gl::Pipeline>& pipeline, uint32_t index)
{
	flushVertices();

	auto vk_pipeline = static_cast<const Pipeline*>(pipeline.get());
	const auto pipeline_obj = vk_pipeline->getPipeline(index);

	m_swapchain_resources[m_swapchain_index].command_buffer.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline_obj);
}

void Context::bindDescriptorSet(const std::unique_ptr<d2gl::Pipeline>& pipeline, uint32_t index)
{
	flushVertices();

	auto vk_pipeline = static_cast<const Pipeline*>(pipeline.get());
	const auto descriptor_set = vk_pipeline->getDescriptorSet(index);
	const auto pipeline_layout = vk_pipeline->getPipelineLayout();

	m_swapchain_resources[m_swapchain_index].command_buffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipeline_layout, 0, descriptor_set, {});
}

void Context::beginRenderPass(const std::unique_ptr<d2gl::RenderPass>& renderpass, const std::unique_ptr<d2gl::FrameBuffer>& framebuffer)
{
	auto vk_renderpass = static_cast<const RenderPass*>(renderpass.get());
	auto vk_framebuffer = static_cast<const FrameBuffer*>(framebuffer.get());
	const auto renderpass_obj = vk_renderpass->getRenderPass();

	const auto frame_resource = &m_frame_resources[m_frame_index];
	const auto command_buffer = m_swapchain_resources[m_swapchain_index].command_buffer;

	flushVertices();
	if (frame_resource->current_renderpass != nullptr)
		command_buffer.endRenderPass();

	frame_resource->current_renderpass = renderpass_obj;
	command_buffer.beginRenderPass(
	  vk::RenderPassBeginInfo()
		.setRenderPass(renderpass_obj)
		.setFramebuffer(vk_framebuffer->getFrameBuffer())
		.setRenderArea(vk_renderpass->getRenderArea())
		.setClearValues(vk_renderpass->getClearValues()),
	  vk::SubpassContents::eInline);
}

void Context::setViewport(glm::uvec2 size, glm::ivec2 offset)
{
	const auto command_buffer = m_swapchain_resources[m_swapchain_index].command_buffer;

	auto viewport = utils::viewport(static_cast<float>(size.x), static_cast<float>(size.y), { static_cast<float>(offset.x), static_cast<float>(offset.y) });
	command_buffer.setViewport(0, viewport);

	auto scissor = vk::Rect2D().setExtent({ size.x, size.y }).setOffset({ offset.x, offset.y });
	command_buffer.setScissor(0, scissor);
}

void Context::pushVertex(const GlideVertex* vertex, glm::vec2 fix, glm::ivec2 offset)
{
	m_vertices.ptr->position = { vertex->x - (float)offset.x, vertex->y - (float)offset.y };
	m_vertices.ptr->tex_coord = {
		((float)((uint32_t)vertex->s >> m_vertex_params.texture_shift) + (float)m_vertex_params.offsets.x) / (512.0f + fix.x),
		((float)((uint32_t)vertex->t >> m_vertex_params.texture_shift) + (float)m_vertex_params.offsets.y) / (512.0f + fix.y),
	};
	m_vertices.ptr->color1 = vertex->pargb;
	m_vertices.ptr->color2 = m_vertex_params.color;
	m_vertices.ptr->texture_ids = m_vertex_params.texture_ids;
	m_vertices.ptr->flags = m_vertex_params.flags;

	m_vertices.ptr++;
	m_vertices.count++;
	m_render_info.vertex_count++;
}

void Context::pushQuad(int8_t x, int8_t y, int8_t z, int8_t w)
{
	static Vertex quad[4] = {
		{ { -1.0f, -1.0f }, { 0.0f, 0.0f } },
		{ { +1.0f, -1.0f }, { 1.0f, 0.0f } },
		{ { +1.0f, +1.0f }, { 1.0f, 1.0f } },
		{ { -1.0f, +1.0f }, { 0.0f, 1.0f } },
	};
	for (size_t i = 0; i < 4; i++) {
		m_vertices.ptr->position = quad[i].position;
		m_vertices.ptr->tex_coord = quad[i].tex_coord;
		m_vertices.ptr->flags = { x, y, z, w };
		m_vertices.ptr++;
	}
	m_vertices.count += 4;
	m_render_info.vertex_count += 4;
}

void Context::flushVertices()
{
	if (m_vertices.count == 0)
		return;

	const uint32_t index_count = m_vertices.count / 4 * 6;
	m_swapchain_resources[m_swapchain_index].command_buffer.drawIndexed(index_count, 1, m_index_count, 0, 0);

	m_render_info.drawcall_count++;
	m_index_count += index_count;
	m_vertices.total_count += m_vertices.count;
	m_vertices.count = 0;
}

void Context::copyToTexture(const std::unique_ptr<d2gl::FrameBuffer>& framebuffer, const std::unique_ptr<d2gl::Texture>& texture, uint32_t index)
{
	const auto frame_resource = &m_frame_resources[m_frame_index];
	const auto command_buffer = m_swapchain_resources[m_swapchain_index].command_buffer;

	flushVertices();
	if (frame_resource->current_renderpass != nullptr) {
		command_buffer.endRenderPass();
		frame_resource->current_renderpass = nullptr;
	}

	auto vk_framebuffer = static_cast<const FrameBuffer*>(framebuffer.get());
	auto vk_texture = static_cast<const Texture*>(texture.get());

	const auto src_image = vk_framebuffer->getImage(index);
	const auto src_width = vk_framebuffer->getExtent().width;
	const auto src_height = vk_framebuffer->getExtent().height;

	const auto dst_image = vk_texture->getImage();
	const auto dst_width = vk_texture->getWidth();
	const auto dst_height = vk_texture->getHeight();

	const auto sub_resource_range = utils::imageSubresourceRange();
	const vk::PipelineStageFlags pipeline_stage_flag = vk::PipelineStageFlagBits::eTransfer;

	command_buffer.pipelineBarrier(pipeline_stage_flag, pipeline_stage_flag, vk::DependencyFlagBits(), {}, {},
	  utils::imageMemoryBarrier(dst_image, vk::AccessFlagBits(), vk::AccessFlagBits::eTransferWrite,
		vk::ImageLayout::eShaderReadOnlyOptimal, vk::ImageLayout::eTransferDstOptimal, sub_resource_range));
	command_buffer.pipelineBarrier(pipeline_stage_flag, pipeline_stage_flag, vk::DependencyFlagBits(), {}, {},
	  utils::imageMemoryBarrier(src_image, vk::AccessFlagBits::eMemoryRead, vk::AccessFlagBits::eTransferRead,
		vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferSrcOptimal, sub_resource_range));

	auto image_copy =
	  vk::ImageCopy()
		.setSrcSubresource(vk::ImageSubresourceLayers().setAspectMask(vk::ImageAspectFlagBits::eColor).setLayerCount(1))
		.setDstSubresource(vk::ImageSubresourceLayers().setAspectMask(vk::ImageAspectFlagBits::eColor).setLayerCount(1))
		.setExtent(vk::Extent3D(src_width, src_height, 1))
		.setDstOffset(vk::Offset3D((dst_width - src_width) / 2, (dst_height - src_height) / 2, 0));
	command_buffer.copyImage(src_image, vk::ImageLayout::eTransferSrcOptimal, dst_image, vk::ImageLayout::eTransferDstOptimal, image_copy);

	command_buffer.pipelineBarrier(pipeline_stage_flag, pipeline_stage_flag, vk::DependencyFlagBits(), {}, {},
	  utils::imageMemoryBarrier(dst_image, vk::AccessFlagBits::eTransferWrite, vk::AccessFlagBits::eMemoryRead,
		vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal, sub_resource_range));
	command_buffer.pipelineBarrier(pipeline_stage_flag, pipeline_stage_flag, vk::DependencyFlagBits(), {}, {},
	  utils::imageMemoryBarrier(src_image, vk::AccessFlagBits::eTransferRead, vk::AccessFlagBits::eMemoryRead,
		vk::ImageLayout::eTransferSrcOptimal, vk::ImageLayout::eTransferDstOptimal, sub_resource_range));
}

void Context::createSurface()
{
	if (m_surface.handle)
		m_instance.destroySurfaceKHR(m_surface.handle);

	const auto surface_return = m_instance.createWin32SurfaceKHR(vk::Win32SurfaceCreateInfoKHR().setHinstance(App.hmodule).setHwnd(App.hwnd));
	assert(surface_return.result == vk::Result::eSuccess);
	m_surface.handle = surface_return.value;

	const auto surface_formats_return = m_physical_device.handle.getSurfaceFormatsKHR(m_surface.handle);
	assert(surface_formats_return.result == vk::Result::eSuccess);
	m_surface.format = surface_formats_return.value[0];

	for (const auto& surface_format : surface_formats_return.value) {
		const vk::Format format = surface_format.format;
		if (format == vk::Format::eR8G8B8A8Unorm || format == vk::Format::eB8G8R8A8Unorm ||
		  format == vk::Format::eA2B10G10R10UnormPack32 || format == vk::Format::eA2R10G10B10UnormPack32 ||
		  format == vk::Format::eR16G16B16A16Sfloat)
			m_surface.format = surface_format;
	}

	auto present_modes_return = m_physical_device.handle.getSurfacePresentModesKHR(m_surface.handle);
	assert(present_modes_return.result == vk::Result::eSuccess);
	m_surface.present_modes = present_modes_return.value;
}

void Context::createSwapchain()
{
	vk::SwapchainKHR old_swapchain = m_swapchain;

	const auto surface_capabilities_return = m_physical_device.handle.getSurfaceCapabilitiesKHR(m_surface.handle);
	assert(surface_capabilities_return.result == vk::Result::eSuccess);
	m_surface.capabilities = surface_capabilities_return.value;

	vk::Extent2D extent = m_surface.capabilities.currentExtent;
	if (extent.width == static_cast<uint32_t>(-1)) {
		extent.width = App.window.size.x;
		extent.height = App.window.size.y;
	} else {
		App.window.size.x = extent.width;
		App.window.size.y = extent.height;
	}
	trace("extent: %d x %d", extent.width, extent.height);

	const auto present_mode = App.vsync ? vk::PresentModeKHR::eFifo : vk::PresentModeKHR::eImmediate;
	if (std::find(m_surface.present_modes.begin(), m_surface.present_modes.end(), present_mode) == m_surface.present_modes.end())
		error("Present mode specified is not supported.");

	uint32_t swapchain_image_count = m_surface.capabilities.minImageCount > 3 ? m_surface.capabilities.minImageCount : 3; // Triple buffering
	if ((m_surface.capabilities.maxImageCount > 0) && (swapchain_image_count > m_surface.capabilities.maxImageCount))
		swapchain_image_count = m_surface.capabilities.maxImageCount;

	vk::SurfaceTransformFlagBitsKHR pre_transform = m_surface.capabilities.currentTransform;
	if (m_surface.capabilities.supportedTransforms & vk::SurfaceTransformFlagBitsKHR::eIdentity)
		pre_transform = vk::SurfaceTransformFlagBitsKHR::eIdentity;

	vk::CompositeAlphaFlagBitsKHR composite_alpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
	std::array<vk::CompositeAlphaFlagBitsKHR, 4> composite_alpha_flags = {
		vk::CompositeAlphaFlagBitsKHR::eOpaque, vk::CompositeAlphaFlagBitsKHR::ePreMultiplied,
		vk::CompositeAlphaFlagBitsKHR::ePostMultiplied, vk::CompositeAlphaFlagBitsKHR::eInherit
	};
	for (const auto& flag : composite_alpha_flags) {
		if (m_surface.capabilities.supportedCompositeAlpha & flag) {
			composite_alpha = flag;
			break;
		}
	}

	const auto swapchain_create_info =
	  vk::SwapchainCreateInfoKHR()
		.setSurface(m_surface.handle)
		.setMinImageCount(swapchain_image_count)
		.setImageFormat(m_surface.format.format)
		.setImageColorSpace(m_surface.format.colorSpace)
		.setImageExtent(extent)
		.setImageArrayLayers(1)
		.setImageUsage(vk::ImageUsageFlagBits::eColorAttachment)
		.setImageSharingMode(vk::SharingMode::eExclusive)
		.setPreTransform(pre_transform)
		.setCompositeAlpha(composite_alpha)
		.setPresentMode(present_mode)
		.setClipped(true)
		.setOldSwapchain(old_swapchain);
	const auto swapchain_return = m_device.createSwapchainKHR(swapchain_create_info);
	assert(swapchain_return.result == vk::Result::eSuccess);
	m_swapchain = swapchain_return.value;

	if (old_swapchain) {
		destroySwapchainResources();
		m_device.destroySwapchainKHR(old_swapchain);
	}

	auto swapchain_images_return = m_device.getSwapchainImagesKHR(m_swapchain);
	assert(swapchain_images_return.result == vk::Result::eSuccess);
	m_swapchain_resources.resize(swapchain_images_return.value.size());

	auto cmd_buffer_info = vk::CommandBufferAllocateInfo().setLevel(vk::CommandBufferLevel::ePrimary).setCommandBufferCount(1);

	for (size_t i = 0; i < m_swapchain_resources.size(); i++) {
		auto& resource = m_swapchain_resources[i];
		resource.image = swapchain_images_return.value[i];
		resource.image_view = utils::createImageView(utils::imageViewCreateInfo(resource.image, m_surface.format.format));

		auto alloc_cmd_return = m_device.allocateCommandBuffers(cmd_buffer_info.setCommandPool(m_command.pool));
		assert(alloc_cmd_return.result == vk::Result::eSuccess);
		resource.command_buffer = alloc_cmd_return.value[0];

		if (m_queue.is_separate) {
			alloc_cmd_return = m_device.allocateCommandBuffers(cmd_buffer_info.setCommandPool(m_command.pool_present));
			assert(alloc_cmd_return.result == vk::Result::eSuccess);
			resource.command_buffer_present = alloc_cmd_return.value[0];

			auto result = resource.command_buffer_present.begin(vk::CommandBufferBeginInfo().setFlags(vk::CommandBufferUsageFlagBits::eSimultaneousUse));
			assert(result == vk::Result::eSuccess);

			resource.command_buffer_present.pipelineBarrier(vk::PipelineStageFlagBits::eBottomOfPipe, vk::PipelineStageFlagBits::eBottomOfPipe, vk::DependencyFlagBits(), {}, {},
			  utils::imageMemoryBarrier(resource.image, vk::AccessFlagBits(), vk::AccessFlagBits(), vk::ImageLayout::ePresentSrcKHR, vk::ImageLayout::ePresentSrcKHR,
				utils::imageSubresourceRange(), m_queue.graphic_index, m_queue.present_index));

			result = resource.command_buffer_present.end();
			assert(result == vk::Result::eSuccess);
		}

		resource.framebuffer = utils::createFrameBuffer(utils::framebufferCreateInfo(m_renderpass, { resource.image_view }, extent));
	}

	m_render_info.ftReset();
}

void Context::destroySwapchainResources()
{
	for (auto& resource : m_swapchain_resources) {
		m_device.freeCommandBuffers(m_command.pool, { resource.command_buffer, resource.command_buffer_present });
		m_device.destroyFramebuffer(resource.framebuffer);
		m_device.destroyImageView(resource.image_view);
	}
	m_swapchain_resources.clear();
}

vk::CommandBuffer Context::beginCommandBuffer(vk::CommandBufferBeginInfo info)
{
	const auto result = m_command.buffer.begin(info);
	assert(result == vk::Result::eSuccess);

	return m_command.buffer;
}

void Context::submitCommandBuffer(vk::SubmitInfo info)
{
	auto result = m_command.buffer.end();
	assert(result == vk::Result::eSuccess);

	const auto fence = utils::createFence();
	result = m_queue.graphic.submit(info.setCommandBuffers(m_command.buffer), fence);
	assert(result == vk::Result::eSuccess);

	result = m_device.waitForFences(fence, VK_TRUE, UINT64_MAX);
	assert(result == vk::Result::eSuccess);

	m_device.destroyFence(fence);
}

void Context::imguiInit()
{
	ImGui::CreateContext();
	ImGui_ImplWin32_Init(App.hwnd);

	ImGui_ImplVulkan_InitInfo init_info = {};
	init_info.Instance = m_instance;
	init_info.PhysicalDevice = m_physical_device.handle;
	init_info.Device = m_device;
	init_info.Queue = m_queue.graphic;
	init_info.DescriptorPool = m_descriptor_pool;
	init_info.MinImageCount = m_swapchain_resources.size();
	init_info.ImageCount = m_swapchain_resources.size();
	init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
	ImGui_ImplVulkan_Init(&init_info, m_renderpass);
}

void Context::imguiDestroy()
{
	ImGui_ImplVulkan_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void Context::imguiSetup()
{
	auto command_buffer = beginCommandBuffer();
	ImGui_ImplVulkan_CreateFontsTexture(command_buffer);
	submitCommandBuffer();

	ImGui_ImplVulkan_DestroyFontUploadObjects();
}

void Context::imguiStartFrame()
{
	flushVertices();

	ImGui_ImplVulkan_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void Context::imguiRender()
{
	ImGui::Render();
	ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), m_swapchain_resources[m_swapchain_index].command_buffer);
}
}