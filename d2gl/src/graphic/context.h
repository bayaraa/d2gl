/*
	D2GL: Diablo 2 LoD Glide/DDraw to OpenGL Wrapper.
	Copyright (C) 2023  Bayaraa

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once

#include "types.h"

#include "command_buffer.h"
#include "frame_buffer.h"
#include "object.h"
#include "pipeline.h"
#include "texture.h"
#include "uniform_buffer.h"

namespace d2gl {

#define MAX_FRAMES 6
#define MAX_INDICES 6 * 50000
#define MAX_VERTICES 4 * 50000
#define PIXEL_BUFFER_SIZE 16 * 1024 * 1024
#define FRAMETIME_SAMPLE_COUNT 120

#define TEXTURE_SLOT_DEFAULT 0
#define TEXTURE_SLOT_GAME 1
#define TEXTURE_SLOT_UPSCALE 4
#define TEXTURE_SLOT_POSTFX1 5
#define TEXTURE_SLOT_POSTFX2 6
#define TEXTURE_SLOT_MAP 7
#define TEXTURE_SLOT_LUT 8
#define TEXTURE_SLOT_PREFX 9
#define TEXTURE_SLOT_BLOOM1 10
#define TEXTURE_SLOT_BLOOM2 11

#define TEXTURE_SLOT_CURSOR 2
#define TEXTURE_SLOT_FONTS 3

#define IMAGE_UNIT_BLUR 0
#define IMAGE_UNIT_FXAA 1

struct GlideVertex {
	float x, y;
	uint32_t pargb;
	float scale;
	float s, t;
	uint32_t unused;
};

template <size_t size>
struct Vertices {
	Vertex* ptr = nullptr;
	uint32_t count = 0;
	uint32_t start = 0;
	std::array<Vertex, MAX_VERTICES> data[size];
};

struct VertexParams {
	uint32_t color;
	uint8_t tex_shift;
	uint16_t tex_num;
	glm::vec<2, uint16_t> offsets;
	uint16_t flags;
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
	LARGE_INTEGER time = { 0 };
	double frequency = 0.0;

	uint32_t vertex_count = 0;
	uint32_t drawcall_count = 0;
	uint32_t frame_count = 0;
};

struct LimiterMetrics {
	bool active = false;
	HANDLE timer = 0;
	LARGE_INTEGER due_time = { 0 };
	float frame_len_ms = 0.0f;
	uint64_t frame_len_ns = 0;
};

struct GLCaps {
	bool compute_shader = false;
	bool independent_blending = false;
};

class Context {
	HGLRC m_context_render = nullptr;
	HGLRC m_context_update = nullptr;
	HANDLE m_semaphore_cpu[MAX_FRAMES];
	HANDLE m_semaphore_gpu[MAX_FRAMES];
	GLsync m_sync_upload[MAX_FRAMES];
	GLsync m_sync_render[MAX_FRAMES];
	CommandBuffer m_command_buffer[MAX_FRAMES];
	bool m_rendering = true;

	GLuint m_index_buffer;
	GLuint m_vertex_array;
	GLuint m_vertex_buffer[MAX_FRAMES];
	GLuint m_pixel_buffer[MAX_FRAMES];
	uint32_t m_frame_index = 0;

	bool m_delay_push = false;
	Vertices<MAX_FRAMES> m_vertices;
	Vertices<2> m_vertices_mod;
	Vertices<1> m_vertices_late;
	VertexParams m_vertex_params;

	FrameMetrics m_frame;
	LimiterMetrics m_limiter;

	std::unique_ptr<Pipeline> m_movie_pipeline;
	std::unique_ptr<FrameBuffer> m_game_framebuffer;

	std::unique_ptr<UniformBuffer> m_upscale_ubo;
	std::unique_ptr<Texture> m_upscale_texture;
	std::unique_ptr<Pipeline> m_upscale_pipeline;
	int m_current_shader = -1;

	glm::vec3 m_sharpen_data;
	glm::uvec2 m_fxaa_work_size = { 0, 0 };
	std::unique_ptr<UniformBuffer> m_postfx_ubo;
	std::unique_ptr<Texture> m_postfx_texture;
	std::unique_ptr<FrameBuffer> m_postfx_framebuffer;
	std::unique_ptr<Pipeline> m_postfx_pipeline;
	std::unique_ptr<Pipeline> m_fxaa_compute_pipeline;

	std::unique_ptr<Pipeline> m_mod_pipeline;

	// Glide
	std::unique_ptr<Texture> m_game_texture;
	std::map<uint32_t, std::pair<uint32_t, BlendType>> m_blend_types;

	std::unique_ptr<Texture> m_movie_texture;

	std::unique_ptr<UniformBuffer> m_game_color_ubo;
	std::unique_ptr<Pipeline> m_game_pipeline;
	uint32_t m_current_blend_index = 0;
	bool m_blend_locked = false;

	glm::vec2 m_bloom_data;
	glm::uvec2 m_bloom_tex_size = { 0, 0 };
	glm::uvec2 m_bloom_work_size = { 0, 0 };
	std::unique_ptr<UniformBuffer> m_bloom_ubo;
	std::unique_ptr<Texture> m_bloom_texture;
	std::unique_ptr<FrameBuffer> m_bloom_framebuffer;
	std::unique_ptr<Pipeline> m_blur_compute_pipeline;

	std::unique_ptr<Texture> m_lut_texture;
	std::unique_ptr<Texture> m_prefx_texture;
	std::unique_ptr<Pipeline> m_prefx_pipeline;

public:
	Context();
	~Context();

	static void renderThread(void* context);

	void onResize();
	void onShaderChange(bool texture = false);

	void beginFrame();
	void bindDefaultFrameBuffer();
	void presentFrame();

	inline const uint32_t getFrameIndex() { return m_frame_index; }
	inline CommandBuffer* getCommandBuffer() { return &m_command_buffer[m_frame_index]; }
	inline UniformBuffer* getGameColorUBO() { return m_game_color_ubo.get(); }
	inline Texture* getGameTexture() { return m_game_texture.get(); }

	void setViewport(glm::ivec2 size, glm::ivec2 offset = { 0, 0 });
	inline void bindFrameBuffer(const std::unique_ptr<FrameBuffer>& framebuffer, bool clear = true) { framebuffer->bind(clear); }
	inline void bindPipeline(const std::unique_ptr<Pipeline>& pipeline, uint32_t index = 0) { pipeline->bind(index); }

	void pushVertex(const GlideVertex* vertex, glm::vec2 fix = { 0.0f, 0.0f }, glm::ivec2 offset = { 0, 0 });
	void pushQuad(int8_t x = 0, int8_t y = 0, int8_t z = 0, int8_t w = 0);
	void flushVertices();

	void pushObject(const std::unique_ptr<Object>& object);
	void flushVerticesMod();
	void appendDelayedObjects();
	inline void toggleDelayPush(bool delay) { m_delay_push = delay; }

	inline void setVertexColor(uint32_t color) { m_vertex_params.color = color; }
	inline void setVertexTexShift(uint8_t shift) { m_vertex_params.tex_shift = shift; }
	inline void setVertexTexNum(uint16_t num) { m_vertex_params.tex_num = num; }
	inline void setVertexOffset(glm::vec<2, uint16_t> offsets) { m_vertex_params.offsets = offsets; }
	inline void setVertexFlag(bool set, uint16_t flag) { m_vertex_params.flags = set ? (m_vertex_params.flags | flag) : (m_vertex_params.flags & ~flag); }

	const double getAvgFrameTime();
	inline const double getFrameTime() { return m_frame.frame_time; }
	inline const uint32_t getFrameCount() { return m_frame.frame_count; }
	inline const uint32_t getVertexCount() { return m_frame.vertex_count; }
	inline const uint32_t getDrawCallCount() { return m_frame.drawcall_count; }

	void toggleVsync();
	void setFpsLimit(bool active, int max_fps);
	void takeScreenShot();

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