#pragma once

#include "graphic/render_info.h"

#include "frame_buffer.h"
#include "pipeline.h"
#include "render_pass.h"
#include "texture.h"
#include "uniform_buffer.h"

namespace d2gl::vulkan {

#define VULKAN_CONTEXT_CLASS vkc(static_cast<vulkan::Context*>(App.context.get()))
#define VULKAN_CONTEXT_INLINE auto vkc = static_cast<vulkan::Context*>(App.context.get())

struct Surface {
	vk::SurfaceKHR handle;
	vk::SurfaceFormatKHR format;
	vk::SurfaceCapabilitiesKHR capabilities;
	std::vector<vk::PresentModeKHR> present_modes;
};

struct PhysicalDevice {
	vk::PhysicalDevice handle;
	vk::PhysicalDeviceFeatures features;
	vk::PhysicalDeviceFeatures enabled_features;
	vk::PhysicalDeviceProperties properties;
	vk::PhysicalDeviceMemoryProperties memory_properties;
};

struct Queue {
	vk::Queue graphic;
	vk::Queue present;
	uint32_t graphic_index = 0;
	uint32_t present_index = 0;
	bool is_separate = false;
};

struct Command {
	vk::CommandPool pool;
	vk::CommandPool pool_present;
	vk::CommandBuffer buffer;
};

struct SwapchainResource {
	vk::Image image;
	vk::ImageView image_view;
	vk::Framebuffer framebuffer;
	vk::CommandBuffer command_buffer;
	vk::CommandBuffer command_buffer_present;
};

struct FrameResource {
	vk::Fence fence;
	vk::Semaphore acquired_sem;
	vk::Semaphore complete_sem;
	vk::Semaphore ownership_sem;
	Buffer vertex_buffer;
	vk::RenderPass current_renderpass = nullptr;
	vk::Pipeline current_pipeline = nullptr;
	vk::DescriptorSet current_descriptor_set = nullptr;
};

class Context final : public d2gl::Context {
	vk::Instance m_instance;
	bool m_debug_extension_enabled = false;
	vk::DebugUtilsMessengerEXT m_debug_extension;

	Surface m_surface;
	PhysicalDevice m_physical_device;
	vk::Device m_device;
	Queue m_queue;
	Command m_command;
	vk::DescriptorPool m_descriptor_pool;

	Buffer m_index_buffer;
	vk::RenderPass m_renderpass;
	vk::SwapchainKHR m_swapchain;
	std::vector<SwapchainResource> m_swapchain_resources;
	uint32_t m_swapchain_index = 0;

	std::vector<FrameResource> m_frame_resources;
	uint32_t m_frame_index = 0;

	Vertices m_vertices;
	VertexParams m_vertex_params;
	size_t m_index_count = 0;

	RenderInfo m_render_info;

	friend class vulkan::Texture;
	friend class vulkan::Pipeline;
	friend class vulkan::RenderPass;
	friend class vulkan::FrameBuffer;
	friend class vulkan::UniformBuffer;

public:
	Context() = default;
	bool init();
	~Context();

	void waitIdle();
	void createSwapchain();

	inline const size_t getMaxFrames() { return 2; }
	inline const uint32_t getFrameIndex() { return m_frame_index; }

	inline const double getFrameTime() { return m_render_info.getFrameTime(); }
	inline const double getAvgFrameTime() { return m_render_info.getAvgFrameTime(); }
	inline const uint32_t getFrameCount() { return m_render_info.frame_count; }
	inline const uint32_t getVertexCount() { return m_render_info.vertex_count; }
	inline const uint32_t getDrawCallCount() { return m_render_info.drawcall_count; }
	inline void setFpsLimit(bool active, int max_fps) { m_render_info.setLimit(active, max_fps); }

	void startFrame();
	void beginFinalPass();
	void presentFrame();

	void bindPipeline(const std::unique_ptr<d2gl::Pipeline>& pipeline, uint32_t index = 0);
	void bindDescriptorSet(const std::unique_ptr<d2gl::Pipeline>& pipeline, uint32_t index = 0);
	void beginRenderPass(const std::unique_ptr<d2gl::RenderPass>& renderpass, const std::unique_ptr<d2gl::FrameBuffer>& framebuffer);
	void setViewport(glm::uvec2 size, glm::ivec2 offset = { 0, 0 });

	void pushVertex(const GlideVertex* vertex, glm::vec2 fix = { 0.0f, 0.0f }, glm::ivec2 offset = { 0, 0 });
	void pushQuad(int8_t x = 0, int8_t y = 0, int8_t z = 0, int8_t w = 0);
	void flushVertices();

	void copyToTexture(const std::unique_ptr<d2gl::FrameBuffer>& framebuffer, const std::unique_ptr<d2gl::Texture>& texture, uint32_t index = 0);

	inline void setVertexColor(uint32_t color) { m_vertex_params.color = color; }
	inline void setVertexTexShift(uint8_t shift) { m_vertex_params.texture_shift = shift; }
	inline void setVertexTexIds(glm::vec<2, int16_t> ids) { m_vertex_params.texture_ids = ids; }
	inline void setVertexOffset(glm::vec<2, uint16_t> offsets) { m_vertex_params.offsets = offsets; }
	inline void setVertexFlags(int8_t x, int8_t y = 0, int8_t z = 0, int8_t w = 0) { m_vertex_params.flags = { x, y, z, w }; }
	inline void setVertexFlagX(int8_t x) { m_vertex_params.flags.x = x; }
	inline void setVertexFlagY(int8_t y) { m_vertex_params.flags.y = y; }
	inline void setVertexFlagZ(int8_t z) { m_vertex_params.flags.z = z; }
	inline void setVertexFlagW(int8_t w) { m_vertex_params.flags.w = w; }

	void imguiSetup();
	void imguiStartFrame();
	void imguiRender();

private:
	void createSurface();
	void destroySwapchainResources();

	void imguiInit();
	void imguiDestroy();

public:
	vk::CommandBuffer beginCommandBuffer(vk::CommandBufferBeginInfo info = {});
	void submitCommandBuffer(vk::SubmitInfo info = {});

	inline const PhysicalDevice& getPhysicalDevice() { return m_physical_device; };
	inline const vk::Device& getDevice() { return m_device; };
};

}