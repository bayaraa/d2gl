#pragma once

#include "graphic/render_info.h"

#include "frame_buffer.h"
#include "pipeline.h"
#include "render_pass.h"
#include "texture.h"
#include "uniform_buffer.h"

namespace d2gl::opengl {

class Context final : public d2gl::Context {
	HGLRC m_instance = nullptr;

	GLuint current_framebuffer = 0;
	GLuint current_program = 0;

	GLuint m_index_buffer;
	GLuint m_vertex_array;
	GLuint m_vertex_buffer;

	Vertices m_vertices;
	VertexParams m_vertex_params;
	size_t m_index_count = 0;

	RenderInfo m_render_info;

public:
	Context() = default;
	bool init();
	~Context();

	void waitIdle();
	void createSwapchain();

	inline const size_t getMaxFrames() { return 1; }
	inline const uint32_t getFrameIndex() { return 0; }

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
	void imguiInit();
	void imguiDestroy();
};

}