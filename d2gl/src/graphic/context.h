#pragma once

#include "frame_buffer.h"
#include "pipeline.h"
#include "texture.h"
#include "uniform_buffer.h"

namespace d2gl {

#define MAX_INDICES 6 * 40000
#define MAX_VERTICES 4 * 40000
#define FRAMETIME_SAMPLE_COUNT 120

struct Vertex {
	glm::vec2 position;
	glm::vec2 tex_coord;
	uint32_t color1;
	uint32_t color2;
	glm::vec<2, int16_t> texture_ids;
	glm::vec<4, int8_t> flags;
	glm::vec2 extra;
};

struct GlideVertex {
	float x, y;
	uint32_t pargb;
	float scale;
	float s, t;
	uint32_t unused;
};

struct Vertices {
	Vertex* ptr;
	size_t count = 0;
	std::array<Vertex, MAX_VERTICES> data;
};

struct VertexParams {
	uint32_t color;
	uint8_t texture_shift;
	glm::vec<2, int16_t> texture_ids;
	glm::vec<2, uint16_t> offsets;
	glm::vec<4, int8_t> flags;
};

struct GameColorData {
	glm::vec4 data[256] = {};
	uint32_t hash = 0;
};

struct SharpenData {
	float strength = 0.405f;
	float clamp = 0.135f;
	float radius = 1.0f;
};

struct FrameMetrics {
	double frame_time = 0.0;
	double prev_time = 0.0;
	std::deque<double> frame_times;
	LARGE_INTEGER time;
	double frequency;

	uint32_t vertex_count = 0;
	uint32_t drawcall_count = 0;
	uint32_t frame_count = 0;
};

struct LimiterMetrics {
	bool active = false;
	HANDLE timer;
	LARGE_INTEGER due_time;
	float frame_len_ms;
	uint64_t frame_len_ns;
};

class Context {
	HGLRC m_context = nullptr;

	GLuint m_index_buffer;
	GLuint m_vertex_array;
	GLuint m_vertex_buffer;

	Vertices m_vertices;
	VertexParams m_vertex_params;

	FrameMetrics m_frame;
	LimiterMetrics m_limiter;

public:
	Context();
	~Context();

	void beginFrame();
	void bindDefaultFrameBuffer();
	void presentFrame();

	void setViewport(glm::ivec2 size, glm::ivec2 offset = { 0, 0 });
	inline void bindFrameBuffer(const std::unique_ptr<FrameBuffer>& framebuffer, bool clear = true) { framebuffer->bind(clear); }
	inline void bindPipeline(const std::unique_ptr<Pipeline>& pipeline, uint32_t index = 0) { pipeline->bind(index); }

	void pushVertex(const GlideVertex* vertex, glm::vec2 fix = { 0.0f, 0.0f }, glm::ivec2 offset = { 0, 0 });
	void pushQuad(int8_t x = 0, int8_t y = 0, int8_t z = 0, int8_t w = 0);
	void flushVertices();

	inline void setVertexColor(uint32_t color) { m_vertex_params.color = color; }
	inline void setVertexTexShift(uint8_t shift) { m_vertex_params.texture_shift = shift; }
	inline void setVertexTexIds(glm::vec<2, int16_t> ids) { m_vertex_params.texture_ids = ids; }
	inline void setVertexOffset(glm::vec<2, uint16_t> offsets) { m_vertex_params.offsets = offsets; }
	inline void setVertexFlags(int8_t x, int8_t y = 0, int8_t z = 0, int8_t w = 0) { m_vertex_params.flags = { x, y, z, w }; }
	inline void setVertexFlagX(int8_t x) { m_vertex_params.flags.x = x; }
	inline void setVertexFlagY(int8_t y) { m_vertex_params.flags.y = y; }
	inline void setVertexFlagZ(int8_t z) { m_vertex_params.flags.z = z; }
	inline void setVertexFlagW(int8_t w) { m_vertex_params.flags.w = w; }

	const double getAvgFrameTime();
	inline const double getFrameTime() { return m_frame.frame_time; }
	inline const uint32_t getFrameCount() { return m_frame.frame_count; }
	inline const uint32_t getVertexCount() { return m_frame.vertex_count; }
	inline const uint32_t getDrawCallCount() { return m_frame.drawcall_count; }

	void toggleVsync();
	void setFpsLimit(bool active, int max_fps);

	void imguiStartFrame();
	void imguiRender();

private:
	void resetFileTime();

	void imguiInit();
	void imguiDestroy();

public:
	static void APIENTRY debugMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* data);

	inline static std::unique_ptr<Context> createContext() { return std::make_unique<Context>(); }
	inline static std::unique_ptr<Texture> createTexture(const TextureCreateInfo& info) { return std::make_unique<Texture>(info); }
	inline static std::unique_ptr<Pipeline> createPipeline(const PipelineCreateInfo& info) { return std::make_unique<Pipeline>(info); }
	inline static std::unique_ptr<FrameBuffer> createFrameBuffer(const FrameBufferCreateInfo& info) { return std::make_unique<FrameBuffer>(info); }
	inline static std::unique_ptr<UniformBuffer> createUniformBuffer(const UniformBufferCreateInfo& info) { return std::make_unique<UniformBuffer>(info); }
};

}