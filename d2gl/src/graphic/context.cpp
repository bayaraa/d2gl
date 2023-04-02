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

#include "pch.h"
#include "context.h"
#include "extra/pd2_fixes.h"
#include "helpers.h"
#include "modules/hd_text.h"
#include "modules/mini_map.h"
#include "modules/motion_prediction.h"
#include "option/menu.h"

#include <imgui/imgui.h>
#include <imgui/imgui_impl_opengl3.h>
#include <imgui/imgui_impl_win32.h>

namespace d2gl {

Context::Context()
{
	PIXELFORMATDESCRIPTOR pfd;
	memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_GENERIC_ACCELERATED;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cDepthBits = 24;
	pfd.cStencilBits = 8;
	pfd.iLayerType = PFD_MAIN_PLANE;

	SetPixelFormat(App.hdc, ChoosePixelFormat(App.hdc, &pfd), &pfd);

	HGLRC context = wglCreateContext(App.hdc);
	wglMakeCurrent(App.hdc, context);

	if (glewInit() != GLEW_OK) {
		MessageBoxA(NULL, "OpenGL loader failed!", "OpenGL failed!", MB_OK);
		exit(1);
	}

	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(context);

	std::vector<glm::vec<2, uint8_t>> versions = { { 4, 6 }, { 4, 5 }, { 4, 4 }, { 4, 3 }, { 4, 2 }, { 4, 1 }, { 4, 0 }, { 3, 3 } };
	for (auto& version : versions) {
		if (App.gl_ver_major < version.x)
			continue;
		if (App.gl_ver_minor < version.y)
			continue;

		int attribs[] = {
			WGL_CONTEXT_MAJOR_VERSION_ARB, version.x,
			WGL_CONTEXT_MINOR_VERSION_ARB, version.y,
			WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
			WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB, 0
		};
		if (App.debug)
			attribs[5] |= WGL_CONTEXT_DEBUG_BIT_ARB;

		if (m_context_render = wglCreateContextAttribsARB(App.hdc, 0, attribs)) {
			m_context_update = wglCreateContextAttribsARB(App.hdc, m_context_render, attribs);
			break;
		}
	}

	if (!m_context_render || !m_context_update) {
		MessageBoxA(App.hwnd, "Requires OpenGL 3.3 or newer!", "Unsupported OpenGL version!", MB_OK | MB_ICONERROR);
		error_log("Requires OpenGL 3.3 or newer! exiting.");
		exit(1);
	}

	wglMakeCurrent(App.hdc, m_context_render);
	glewInit();

	GLint major_version, minor_version;
	glGetIntegerv(GL_MAJOR_VERSION, &major_version);
	glGetIntegerv(GL_MINOR_VERSION, &minor_version);

	char version_str[50] = { 0 };
	sprintf_s(version_str, "%d.%d", major_version, minor_version);
	trace_log("OpenGL: %s (%s)", version_str, glGetString(GL_RENDERER));
	App.version = version_str;

	GLint max_texture_unit;
	glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &max_texture_unit);
	trace_log("OpenGL: GL_MAX_TEXTURE_IMAGE_UNITS = %d", max_texture_unit);

	if (App.debug && glewIsSupported("GL_KHR_debug")) {
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(Context::debugMessageCallback, nullptr);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
		trace("GL_KHR_debug enabled!");
	}

	if (App.use_compute_shader && (glewIsSupported("GL_VERSION_4_3") || glewIsSupported("GL_ARB_compute_shader"))) {
		App.gl_caps.compute_shader = true;
		trace_log("OpenGL: Compute shader available.");
	}

	if (glewIsSupported("GL_VERSION_4_0")) {
		App.gl_caps.independent_blending = true;
		trace_log("OpenGL: Independent blending available.");
	}

	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_STENCIL_TEST);

	glBlendEquation(GL_FUNC_ADD);

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

	glGenVertexArrays(1, &m_vertex_array);
	glBindVertexArray(m_vertex_array);

	glGenBuffers(1, &m_index_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_index_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * MAX_INDICES, indices, GL_STATIC_DRAW);
	delete[] indices;

	for (uint32_t i = 0; i < MAX_FRAMES; i++) {
		glGenBuffers(1, &m_vertex_buffer[i]);
		glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer[i]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(m_vertices.data[0]), nullptr, GL_DYNAMIC_DRAW);

		glGenBuffers(1, &m_pixel_buffer[i]);
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_pixel_buffer[i]);
		glBufferData(GL_PIXEL_UNPACK_BUFFER, PIXEL_BUFFER_SIZE, NULL, GL_STREAM_DRAW);
	}
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

	// glEnableVertexAttribArray(0);
	// glVertexAttribPointer(0, 2, GL_HALF_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, position));
	// glEnableVertexAttribArray(1);
	// glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, tex_coord));
	// glEnableVertexAttribArray(2);
	// glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), (const void*)offsetof(Vertex, color1));
	// glEnableVertexAttribArray(3);
	// glVertexAttribPointer(3, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), (const void*)offsetof(Vertex, color2));
	// glEnableVertexAttribArray(4);
	// glVertexAttribIPointer(4, 1, GL_UNSIGNED_SHORT, sizeof(Vertex), (const void*)offsetof(Vertex, tex_num));
	// glEnableVertexAttribArray(5);
	// glVertexAttribIPointer(5, 1, GL_UNSIGNED_SHORT, sizeof(Vertex), (const void*)offsetof(Vertex, flags));
	//  glEnableVertexAttribArray(6);
	//  glVertexAttribPointer(6, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, extra));

	imguiInit();

	PipelineCreateInfo movie_pipeline_ci;
	movie_pipeline_ci.shader = g_shader_movie;
	movie_pipeline_ci.bindings = { { BindingType::Texture, "u_Texture", TEXTURE_SLOT_DEFAULT } };
	m_movie_pipeline = Context::createPipeline(movie_pipeline_ci);

	UniformBufferCreateInfo upscale_ubo_ci;
	upscale_ubo_ci.variables = { { "out_size", sizeof(glm::vec2) }, { "tex_size", sizeof(glm::vec2) }, { "rel_size", sizeof(glm::vec2) } };
	m_upscale_ubo = Context::createUniformBuffer(upscale_ubo_ci);

	UniformBufferCreateInfo postfx_ubo_ci;
	postfx_ubo_ci.variables = { { "sharpen", sizeof(glm::vec4) }, { "rel_size", sizeof(glm::vec2) } };
	m_postfx_ubo = Context::createUniformBuffer(postfx_ubo_ci);

	m_sharpen_data = { App.sharpen.strength.value, App.sharpen.clamp.value, App.sharpen.radius.value };
	m_postfx_ubo->updateDataVec4f("sharpen", glm::vec4(m_sharpen_data, 1.0f));

	PipelineCreateInfo postfx_pipeline_ci;
	postfx_pipeline_ci.shader = g_shader_postfx;
	postfx_pipeline_ci.bindings = {
		{ BindingType::UniformBuffer, "ubo_Metrics", m_postfx_ubo->getBinding() },
		{ BindingType::Texture, "u_Texture0", TEXTURE_SLOT_POSTFX1 },
		{ BindingType::Texture, "u_Texture1", TEXTURE_SLOT_POSTFX2 },
	};
	m_postfx_pipeline = Context::createPipeline(postfx_pipeline_ci);
	m_postfx_pipeline->setUniformMat4f("u_MVP", glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f));

	if (App.gl_caps.compute_shader) {
		PipelineCreateInfo fxaa_pipeline_ci;
		fxaa_pipeline_ci.shader = g_shader_postfx;
		fxaa_pipeline_ci.bindings = {
			{ BindingType::Texture, "u_InTexture", TEXTURE_SLOT_POSTFX2 },
			{ BindingType::Image, "u_OutTexture", IMAGE_UNIT_FXAA },
		};
		fxaa_pipeline_ci.compute = true;
		m_fxaa_compute_pipeline = Context::createPipeline(fxaa_pipeline_ci);
	}

	PipelineCreateInfo mod_pipeline_ci;
	mod_pipeline_ci.shader = g_shader_mod;
	mod_pipeline_ci.attachment_blends = { { BlendType::SAlpha_OneMinusSAlpha } };
	mod_pipeline_ci.bindings = {
		{ BindingType::Texture, "u_MapTexture", TEXTURE_SLOT_MAP },
		{ BindingType::Texture, "u_CursorTexture", TEXTURE_SLOT_CURSOR },
		{ BindingType::Texture, "u_FontTexture", TEXTURE_SLOT_FONTS },
	};
	m_mod_pipeline = Context::createPipeline(mod_pipeline_ci);

	if (App.api == Api::Glide) {
		TextureCreateInfo game_texture_ci;
		game_texture_ci.size = { 512, 512 };
		game_texture_ci.layer_count = 512;
		game_texture_ci.format = GL_RED;
		m_game_texture = std::make_unique<Texture>(game_texture_ci);

		TextureCreateInfo movie_texture_ci;
		movie_texture_ci.size = { 640, 480 };
		movie_texture_ci.min_filter = GL_LINEAR;
		movie_texture_ci.mag_filter = GL_LINEAR;
		movie_texture_ci.format = GL_BGRA;
		m_movie_texture = Context::createTexture(movie_texture_ci);

		UniformBufferCreateInfo game_ubo_ci;
		game_ubo_ci.variables = { { "palette", 256 * sizeof(glm::vec4) }, { "gamma", 256 * sizeof(glm::vec4) } };
		m_game_color_ubo = Context::createUniformBuffer(game_ubo_ci);

		PipelineCreateInfo game_pipeline_ci;
		game_pipeline_ci.shader = g_shader_glide;
		game_pipeline_ci.bindings = {
			{ BindingType::UniformBuffer, "ubo_Colors", m_game_color_ubo->getBinding() },
			{ BindingType::Texture, "u_Texture", TEXTURE_SLOT_DEFAULT },
		};
		game_pipeline_ci.attachment_blends.clear();
		for (auto& blend : g_blend_types)
			game_pipeline_ci.attachment_blends.push_back({ blend.second.second, BlendType::SAlpha_OneMinusSAlpha });
		m_game_pipeline = Context::createPipeline(game_pipeline_ci);

		TextureCreateInfo lut_texture_ci;
		lut_texture_ci.size = { 1024, 32 };
		lut_texture_ci.layer_count = 14;
		lut_texture_ci.slot = TEXTURE_SLOT_LUT;
		m_lut_texture = Context::createTexture(lut_texture_ci);

		auto image_data = helpers::loadImage("assets\\textures\\lut.png", false);
		m_lut_texture->fillImage(image_data, 1, 14);
		helpers::clearImage(image_data);

		if (App.gl_caps.compute_shader) {
			PipelineCreateInfo blur_pipeline_ci;
			blur_pipeline_ci.shader = g_shader_prefx;
			blur_pipeline_ci.bindings = {
				{ BindingType::Texture, "u_InTexture", TEXTURE_SLOT_BLOOM2 },
				{ BindingType::Image, "u_OutTexture", IMAGE_UNIT_BLUR },
			};
			blur_pipeline_ci.compute = true;
			m_blur_compute_pipeline = Context::createPipeline(blur_pipeline_ci);
		}

		UniformBufferCreateInfo bloom_ubo_ci;
		bloom_ubo_ci.variables = { { "bloom", sizeof(glm::vec2) }, { "rel_size", sizeof(glm::vec2) } };
		m_bloom_ubo = Context::createUniformBuffer(bloom_ubo_ci);

		m_bloom_data = { App.bloom.exposure.value, App.bloom.gamma.value };
		m_bloom_ubo->updateDataVec2f("bloom", m_bloom_data);

		PipelineCreateInfo prefx_pipeline_ci;
		prefx_pipeline_ci.shader = g_shader_prefx;
		prefx_pipeline_ci.bindings = {
			{ BindingType::UniformBuffer, "ubo_Metrics", m_bloom_ubo->getBinding() },
			{ BindingType::Texture, "u_Texture", TEXTURE_SLOT_PREFX },
			{ BindingType::Texture, "u_BloomTexture1", TEXTURE_SLOT_BLOOM1 },
			{ BindingType::Texture, "u_BloomTexture2", TEXTURE_SLOT_BLOOM2 },
			{ BindingType::Texture, "u_LUTTexture", m_lut_texture->getSlot() },
		};
		m_prefx_pipeline = Context::createPipeline(prefx_pipeline_ci);
		m_prefx_pipeline->setUniformMat4f("u_MVP", glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f));
	}

	onResize();

	LARGE_INTEGER qpf;
	QueryPerformanceFrequency(&qpf);
	m_frame.frequency = double(qpf.QuadPart) / 1000.0;
	m_frame.frame_times.assign(FRAMETIME_SAMPLE_COUNT, m_frame.frame_time);

	m_limiter.timer = CreateWaitableTimer(NULL, TRUE, NULL);
	setFpsLimit(App.foreground_fps.active, App.foreground_fps.range.value);

	m_vertices_mod.count = 0;
	m_vertices_mod.ptr = m_vertices_mod.data[m_frame_index].data();

	m_frame.vertex_count = 0;
	m_frame.drawcall_count = 0;

	for (uint32_t i = 0; i < MAX_FRAMES; i++) {
		m_semaphore_cpu[i] = CreateSemaphore(NULL, 0, 1, NULL);
		m_semaphore_gpu[i] = CreateSemaphore(NULL, 0, 1, NULL);
		ReleaseSemaphore(m_semaphore_gpu[i], 1, NULL);
	}

	wglMakeCurrent(NULL, NULL);
	wglMakeCurrent(App.hdc, m_context_update);
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Context::renderThread, reinterpret_cast<void*>(this), 0, NULL);
}

Context::~Context()
{
	m_rendering = false;
	for (uint32_t i = 0; i < MAX_FRAMES; i++)
		ReleaseSemaphore(m_semaphore_cpu[i], 1, NULL);

	for (uint32_t i = 0; i < MAX_FRAMES; i++)
		WaitForSingleObject(m_semaphore_gpu[i], INFINITE);

	wglMakeCurrent(NULL, NULL);
	wglMakeCurrent(App.hdc, m_context_render);
	imguiDestroy();

	for (uint32_t i = 0; i < MAX_FRAMES; i++) {
		glDeleteBuffers(1, &m_pixel_buffer[i]);
		glDeleteBuffers(1, &m_vertex_buffer[i]);
	}
	glDeleteBuffers(1, &m_index_buffer);
	glDeleteVertexArrays(1, &m_vertex_array);

	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(m_context_update);
	wglDeleteContext(m_context_render);
}

void Context::renderThread(void* context)
{
	Context* ctx = reinterpret_cast<Context*>(context);
	wglMakeCurrent(App.hdc, ctx->m_context_render);
	uint32_t frame_index = 0;

	while (ctx->m_rendering) {
		WaitForSingleObject(ctx->m_semaphore_cpu[frame_index], INFINITE);
		glClientWaitSync(ctx->m_sync_upload[frame_index], 0, 1000000000u);

		const auto cmd = &ctx->m_command_buffer[frame_index];
		glBindBuffer(GL_ARRAY_BUFFER, ctx->m_vertex_buffer[frame_index]);
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, ctx->m_pixel_buffer[frame_index]);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_HALF_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, position));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, tex_coord));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), (const void*)offsetof(Vertex, color1));
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), (const void*)offsetof(Vertex, color2));
		glEnableVertexAttribArray(4);
		glVertexAttribIPointer(4, 1, GL_UNSIGNED_SHORT, sizeof(Vertex), (const void*)offsetof(Vertex, tex_num));
		glEnableVertexAttribArray(5);
		glVertexAttribIPointer(5, 1, GL_UNSIGNED_SHORT, sizeof(Vertex), (const void*)offsetof(Vertex, flags));

		// const auto cmd = &m_command_buffer[m_frame_index];
		// if (cmd->m_vertex_count) {
		//	// glBindBuffer(GL_ARRAY_BUFFER, ctx->m_vertex_buffer[frame_index]);
		//	glBufferSubData(GL_ARRAY_BUFFER, 0, cmd->m_vertex_count * sizeof(Vertex), ctx->m_vertices.data[frame_index].data());
		//	// glBindBuffer(GL_ARRAY_BUFFER, 0);
		//}
		// if (cmd->m_tex_update_queue.count) {
		//	// glBindBuffer(GL_PIXEL_UNPACK_BUFFER, ctx->m_pixel_buffer[frame_index]);
		//	glBufferSubData(GL_PIXEL_UNPACK_BUFFER, 0, cmd->m_tex_update_queue.data_offset, cmd->m_tex_update_queue.data);
		//	// glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
		//}

		if (cmd->m_tex_update_queue.count) {
			for (uint32_t i = 0; i < cmd->m_tex_update_queue.count; i++) {
				const auto data = &cmd->m_tex_update_queue.tex_data[i];
				ctx->m_game_texture->fill((uint8_t*)data->offset, data->tex_size.x, data->tex_size.y, data->tex_offset.x, data->tex_offset.y, data->tex_num);
			}
		}

		// if (cmd->m_ubo_update_queue.count) {
		//	for (uint32_t i = 0; i < cmd->m_ubo_update_queue.count; i++) {
		//		const auto data = &cmd->m_ubo_update_queue.data[i];
		//		ctx->m_game_color_ubo->updateData(data->type == UBOType::Gamma ? "gamma" : "palette", data->value);
		//		const uint32_t hash = helpers::hash(data->value, sizeof(uint32_t) * 256);
		//		trace_log("%s: %08X", data->type == UBOType::Gamma ? "gamma" : "palette", hash);
		//	}
		// }

		for (uint32_t i = 0; i < cmd->m_count; i++) {
			const auto command = &cmd->m_commands[i];

			switch (command->type) {
				case CommandType::Begin:
					ctx->bindDefaultFrameBuffer();
					// ctx->bindFrameBuffer(ctx->m_game_framebuffer);
					ctx->setViewport(App.game.size);
					break;
				case CommandType::UBOUpdate: {
					const auto data = &cmd->m_ubo_update_queue.data[command->index];
					ctx->m_game_color_ubo->updateData(data->type == UBOType::Gamma ? "gamma" : "palette", data->value);
				} break;
				case CommandType::SetBlendState:
					ctx->bindPipeline(ctx->m_game_pipeline, command->index);
					break;
				case CommandType::PreFx:
					break;
				case CommandType::DrawIndexed:
					if (command->draw.count > 0)
						glDrawElementsBaseVertex(GL_TRIANGLES, command->draw.count, GL_UNSIGNED_INT, 0, command->draw.start);
					break;
				case CommandType::Submit:
					break;
				case CommandType::Resize:
					ctx->onResize();
					break;
			}
		}

		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		ctx->m_sync_render[frame_index] = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
		glFlush();

		if (ctx->m_limiter.active) {
			WaitForSingleObject(ctx->m_limiter.timer, (DWORD)ctx->m_limiter.frame_len_ms + 1);
			ctx->m_limiter.due_time.QuadPart += ctx->m_limiter.frame_len_ns;
			SetWaitableTimer(ctx->m_limiter.timer, &ctx->m_limiter.due_time, 0, NULL, NULL, FALSE);
		}

		ReleaseSemaphore(ctx->m_semaphore_gpu[frame_index], 1, NULL);
		SwapBuffers(App.hdc);
		// option::Menu::instance().draw();

		frame_index = (frame_index + 1) % App.frames_in_flight;
	}

	wglMakeCurrent(NULL, NULL);
	for (uint32_t i = 0; i < 2; i++)
		ReleaseSemaphore(ctx->m_semaphore_gpu[i], 1, NULL);
}

void Context::onResize()
{
	static glm::uvec2 game_size = { 0, 0 };
	bool game_resized = game_size != App.game.size;
	game_size = App.game.size;

	static glm::uvec2 window_size = { 0, 0 };
	bool window_resized = (App.window.resized || window_size != App.window.size);
	window_size = App.window.size;
	App.window.resized = false;

	if (game_resized) {
		glm::mat4 mvp = glm::ortho(0.0f, (float)game_size.x, (float)game_size.y, 0.0f);
		modules::HDText::Instance().setMVP(mvp);

		FrameBufferCreateInfo frambuffer_ci;
		frambuffer_ci.size = game_size;
		if (App.api == Api::Glide)
			frambuffer_ci.attachments = {
				{ TEXTURE_SLOT_GAME, { 0.0f, 0.0f, 0.0f, 1.0f }, GL_LINEAR, GL_LINEAR },
				{ TEXTURE_SLOT_MAP, { 0.0f, 0.0f, 0.0f, 0.0f } }
			};
		else
			frambuffer_ci.attachments = { { TEXTURE_SLOT_GAME } };
		m_game_framebuffer = Context::createFrameBuffer(frambuffer_ci);

		m_mod_pipeline->setUniformMat4f("u_MVP", mvp);
		m_mod_pipeline->setUniformVec2f("u_Scale", App.viewport.scale);
		m_mod_pipeline->setUniformVec2f("u_Size", { (float)game_size.x, (float)game_size.y });

		m_upscale_ubo->updateDataVec2f("tex_size", { (float)App.game.tex_size.x, (float)App.game.tex_size.y });
		m_upscale_ubo->updateDataVec2f("rel_size", { 1.0f / App.game.tex_size.x, 1.0f / App.game.tex_size.y });
	}

	if (game_resized) {
		if (App.api == Api::Glide) {
			glm::mat4 mvp = glm::ortho(0.0f, (float)game_size.x, (float)game_size.y, 0.0f);
			m_game_pipeline->setUniformMat4f("u_MVP", mvp);

			m_bloom_ubo->updateDataVec2f("rel_size", { 4.0f / game_size.x, 4.0f / game_size.y });
			m_bloom_tex_size = { game_size.x / 4, game_size.y / 4 };
			m_bloom_work_size = { ceil((float)m_bloom_tex_size.x / 16), ceil((float)m_bloom_tex_size.y / 16) };

			FrameBufferCreateInfo bloom_frambuffer_ci;
			bloom_frambuffer_ci.size = m_bloom_tex_size;
			bloom_frambuffer_ci.attachments = { { TEXTURE_SLOT_BLOOM1, {}, GL_LINEAR, GL_LINEAR }, { TEXTURE_SLOT_BLOOM2 } };
			m_bloom_framebuffer = Context::createFrameBuffer(bloom_frambuffer_ci);
			if (App.gl_caps.compute_shader)
				m_bloom_framebuffer->getTexture()->bindImage(IMAGE_UNIT_BLUR);

			TextureCreateInfo bloom_texture_ci;
			bloom_texture_ci.size = m_bloom_tex_size;
			bloom_texture_ci.slot = TEXTURE_SLOT_BLOOM2;
			bloom_texture_ci.min_filter = GL_LINEAR;
			bloom_texture_ci.mag_filter = GL_LINEAR;
			m_bloom_texture = Context::createTexture(bloom_texture_ci);

			TextureCreateInfo prefx_texture_ci;
			prefx_texture_ci.size = game_size;
			prefx_texture_ci.slot = TEXTURE_SLOT_PREFX;
			m_prefx_texture = Context::createTexture(prefx_texture_ci);
		}
	}

	if (window_resized) {
		toggleVsync();

		glm::vec2 scale = { (float)window_size.x / 640, (float)window_size.y / 360 };
		float offset_x = ((scale.x > scale.y) ? scale.x / scale.y : 1.0f) * 1.00f;
		float offset_y = ((scale.x < scale.y) ? scale.y / scale.x : 1.0f) * 0.75f;
		m_movie_pipeline->setUniformMat4f("u_MVP", glm::ortho(-1.0f * offset_x, 1.0f * offset_x, 1.0f * offset_y, -1.0f * offset_y));
	}

	FrameBufferCreateInfo frambuffer_ci;
	frambuffer_ci.size = App.viewport.size;
	frambuffer_ci.attachments = { { TEXTURE_SLOT_POSTFX1, {}, GL_LINEAR, GL_LINEAR } };
	m_postfx_framebuffer = Context::createFrameBuffer(frambuffer_ci);
	if (App.gl_caps.compute_shader)
		m_postfx_framebuffer->getTexture()->bindImage(IMAGE_UNIT_FXAA);

	m_fxaa_work_size = { ceil((float)App.viewport.size.x / 16), ceil((float)App.viewport.size.y / 16) };

	TextureCreateInfo texture_ci;
	texture_ci.size = App.viewport.size;
	texture_ci.slot = TEXTURE_SLOT_POSTFX2;
	texture_ci.min_filter = GL_LINEAR;
	texture_ci.mag_filter = GL_LINEAR;
	m_postfx_texture = Context::createTexture(texture_ci);

	onShaderChange(game_resized);
	m_upscale_ubo->updateDataVec2f("out_size", { (float)App.game.tex_size.x * App.viewport.scale.x, (float)App.game.tex_size.y * App.viewport.scale.y });
	m_postfx_ubo->updateDataVec2f("rel_size", { 1.0f / App.viewport.size.x, 1.0f / App.viewport.size.y });
	m_mod_pipeline->setUniformVec2f("u_Scale", App.viewport.scale);

	modules::MiniMap::Instance().resize();
}

void Context::onShaderChange(bool texture)
{
	const auto shader = g_shader_upscale[App.shader.selected];

	if (texture || m_current_shader != App.shader.selected) {
		TextureCreateInfo texture_ci;
		texture_ci.size = App.game.tex_size;
		texture_ci.slot = TEXTURE_SLOT_UPSCALE;
		if (shader.linear) {
			texture_ci.min_filter = GL_LINEAR;
			texture_ci.mag_filter = GL_LINEAR;
		}
		m_upscale_texture = Context::createTexture(texture_ci);
	}

	if (m_current_shader != App.shader.selected) {
		PipelineCreateInfo pipeline_ci;
		pipeline_ci.shader = shader.source;
		pipeline_ci.bindings = {
			{ BindingType::UniformBuffer, "ubo_Metrics", m_upscale_ubo->getBinding() },
			{ BindingType::Texture, "u_Texture", TEXTURE_SLOT_UPSCALE },
		};
		m_upscale_pipeline = Context::createPipeline(pipeline_ci);
	}

	m_upscale_pipeline->setUniformMat4f("u_MVP", glm::ortho(-App.game.tex_scale.x, App.game.tex_scale.x, -App.game.tex_scale.y, App.game.tex_scale.y));
	m_current_shader = App.shader.selected;
}

void Context::beginFrame()
{
	m_vertices.count = m_vertices.start = 0;
	m_vertices.ptr = m_vertices.data[m_frame_index].data();

	m_vertices_mod.count = m_vertices_mod.start = 0;
	m_vertices_mod.ptr = m_vertices_mod.data[m_frame_index].data();

	m_delay_push = false;
	m_vertices_late.count = 0;
	m_vertices_late.ptr = m_vertices_late.data[0].data();

	m_frame.vertex_count = 0;
	m_frame.drawcall_count = 0;

	modules::HDText::Instance().reset();
	modules::MotionPrediction::Instance().update();

	if (App.game.screen != GameScreen::Movie)
		m_command_buffer[m_frame_index].pushCommand(CommandType::Begin);
	// trace("=== begin ===");
}

void Context::bindDefaultFrameBuffer()
{
	// flushVertices();

	FrameBuffer::unBind();
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}

void Context::presentFrame()
{
	if (App.game.screen == GameScreen::Movie) {
		bindDefaultFrameBuffer();
		setViewport(App.window.size);
		bindPipeline(m_movie_pipeline);
		pushQuad(m_frame_index);
	} else {
		flushVertices();
		m_command_buffer->pushCommand(CommandType::Submit);
		/*if (m_current_shader != App.shader.selected)
			onShaderChange();

		if (App.sharpen.active) {
			const auto sharpen_data = glm::vec3(App.sharpen.strength.value, App.sharpen.clamp.value, App.sharpen.radius.value);
			if (m_sharpen_data != sharpen_data) {
				m_postfx_ubo->updateDataVec4f("sharpen", glm::vec4(sharpen_data, 1.0f));
				m_sharpen_data = sharpen_data;
			}
		}

		m_upscale_texture->fillFromBuffer(m_game_framebuffer);

		if (App.sharpen.active || App.fxaa) {
			bindFrameBuffer(m_postfx_framebuffer, false);
			setViewport(App.viewport.size);
		} else {
			bindDefaultFrameBuffer();
			setViewport(App.viewport.size, App.viewport.offset);
		}
		bindPipeline(m_upscale_pipeline);
		pushQuad();

		if (App.sharpen.active) {
			if (App.fxaa)
				m_postfx_texture->fillFromBuffer(m_postfx_framebuffer);
			else {
				bindDefaultFrameBuffer();
				setViewport(App.viewport.size, App.viewport.offset);
			}
			bindPipeline(m_postfx_pipeline);
			pushQuad(0, App.fxaa);
		}

		if (App.fxaa) {
			if (App.gl_caps.compute_shader) {
				m_postfx_texture->fillFromBuffer(m_postfx_framebuffer);
				m_fxaa_compute_pipeline->dispatchCompute(0, m_fxaa_work_size, GL_SHADER_IMAGE_ACCESS_BARRIER_BIT); // TODO:
			}
			bindDefaultFrameBuffer();
			setViewport(App.viewport.size, App.viewport.offset);
			bindPipeline(m_postfx_pipeline);
			pushQuad(1 + App.gl_caps.compute_shader);
		}*/

		// modules::HDText::Instance().update(m_mod_pipeline);
		// bindPipeline(m_mod_pipeline);
		// flushVerticesMod();
	}

	// option::Menu::instance().draw();

	fixPD2invItemActions();

	glClientWaitSync(m_sync_render[m_frame_index], 0, 1000000000u);
	const auto cmd = &m_command_buffer[m_frame_index];
	if (cmd->m_vertex_count) {
		glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer[m_frame_index]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, cmd->m_vertex_count * sizeof(Vertex), m_vertices.data[m_frame_index].data());
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	if (cmd->m_tex_update_queue.count) {
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_pixel_buffer[m_frame_index]);
		glBufferSubData(GL_PIXEL_UNPACK_BUFFER, 0, cmd->m_tex_update_queue.data_offset, cmd->m_tex_update_queue.data);
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
	}
	m_sync_upload[m_frame_index] = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
	glFlush();

	ReleaseSemaphore(m_semaphore_cpu[m_frame_index], 1, NULL);
	// m_frame_index = m_frame_index ? 0 : 1;
	m_frame_index = (m_frame_index + 1) % App.frames_in_flight;

	WaitForSingleObject(m_semaphore_gpu[m_frame_index], INFINITE);
	m_command_buffer[m_frame_index].reset();

	// if (m_limiter.active) {
	//	WaitForSingleObject(m_limiter.timer, (DWORD)m_limiter.frame_len_ms + 1);
	//	m_limiter.due_time.QuadPart += m_limiter.frame_len_ns;
	//	SetWaitableTimer(m_limiter.timer, &m_limiter.due_time, 0, NULL, NULL, FALSE);
	// }

	QueryPerformanceCounter(&m_frame.time);
	double cur_time = (double(m_frame.time.QuadPart) / m_frame.frequency);
	m_frame.frame_time = cur_time - m_frame.prev_time;
	m_frame.prev_time = cur_time;

	m_frame.frame_times.pop_front();
	m_frame.frame_times.push_back(m_frame.frame_time);
	m_frame.frame_count++;
}

void Context::setViewport(glm::ivec2 size, glm::ivec2 offset)
{
	static glm::ivec4 viewport_metrics = { 0, 0, 0, 0 };
	const auto metrics = glm::ivec4(size, offset);
	if (viewport_metrics == metrics)
		return;

	glViewport(offset.x, offset.y, size.x, size.y);
	viewport_metrics = metrics;
}

void Context::pushVertex(const GlideVertex* vertex, glm::vec2 fix, glm::ivec2 offset)
{
	if (m_vertices.count >= MAX_VERTICES - 4)
		flushVertices();

	m_vertices.ptr->position = {
		glm::detail::toFloat16(vertex->x - (float)offset.x),
		glm::detail::toFloat16(vertex->y - (float)offset.y),
	};
	m_vertices.ptr->tex_coord = {
		((float)((uint32_t)vertex->s >> m_vertex_params.tex_shift) + (float)m_vertex_params.offsets.x) / (512.0f + fix.x),
		((float)((uint32_t)vertex->t >> m_vertex_params.tex_shift) + (float)m_vertex_params.offsets.y) / (512.0f + fix.y),
	};
	m_vertices.ptr->color1 = vertex->pargb;
	m_vertices.ptr->color2 = m_vertex_params.color;
	m_vertices.ptr->tex_num = m_vertex_params.tex_num;
	m_vertices.ptr->flags = m_vertex_params.flags;

	m_vertices.ptr++;
	m_vertices.count++;
	m_frame.vertex_count++;
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
		// m_vertices.ptr->flags = { x, y, z, w };
		m_vertices.ptr++;
	}
	m_vertices.count += 4;
	m_frame.vertex_count += 4;
}

void Context::flushVertices()
{
	if (m_vertices.count == 0)
		return;

	m_command_buffer[m_frame_index].drawIndexed(m_vertices.start, m_vertices.count);

	m_vertices.start += m_vertices.count;
	m_vertices.count = 0;
	m_frame.drawcall_count++;
}

void Context::pushObject(const std::unique_ptr<Object>& object)
{
	const auto vertices = object->getVertices();

	if (m_delay_push) {
		memcpy(m_vertices_late.ptr, vertices, sizeof(Vertex) * 4);

		m_vertices_late.ptr += 4;
		m_vertices_late.count += 4;
	} else {
		if (m_vertices_mod.count >= MAX_VERTICES - 4)
			flushVerticesMod();

		memcpy(m_vertices_mod.ptr, vertices, sizeof(Vertex) * 4);

		m_vertices_mod.ptr += 4;
		m_vertices_mod.count += 4;
	}
	m_frame.vertex_count += 4;
}

void Context::flushVerticesMod()
{
	if (m_vertices_mod.count == 0)
		return;

	// m_command_buffer[m_frame_index].drawIndexed(&m_vertices_mod.data[m_frame_index][m_vertices.start], m_vertices_mod.count);

	m_vertices_mod.start += m_vertices_mod.count;
	m_vertices_mod.count = 0;
	m_frame.drawcall_count++;
}

void Context::appendDelayedObjects()
{
	if (m_vertices_late.count == 0)
		return;

	memcpy(m_vertices_mod.ptr, m_vertices_late.data[0].data(), m_vertices_late.count * sizeof(Vertex));

	m_vertices_mod.count += m_vertices_late.count;
	m_vertices_mod.ptr += m_vertices_late.count;

	m_delay_push = false;
	m_vertices_late.count = 0;
	m_vertices_late.ptr = m_vertices_late.data[0].data();
}

void Context::toggleVsync()
{
	wglSwapIntervalEXT(App.vsync);
	resetFileTime();
}

const double Context::getAvgFrameTime()
{
	return std::reduce(m_frame.frame_times.begin(), m_frame.frame_times.end()) / FRAMETIME_SAMPLE_COUNT;
}

void Context::setFpsLimit(bool active, int max_fps)
{
	m_limiter.active = active;
	m_limiter.frame_len_ms = 1000.0f / max_fps;
	m_limiter.frame_len_ns = (uint64_t)(m_limiter.frame_len_ms * 10000);

	if (m_limiter.active)
		resetFileTime();
}

void Context::resetFileTime()
{
	FILETIME ft = { 0 };
	GetSystemTimeAsFileTime(&ft);
	memcpy(&m_limiter.due_time, &ft, sizeof(LARGE_INTEGER));
}

void Context::takeScreenShot()
{
	static uint8_t* data = new GLubyte[App.viewport.size.x * App.viewport.size.y * 4];
	memset(data, 0, App.viewport.size.x * App.viewport.size.y * 4);

	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glReadPixels(App.viewport.offset.x, App.viewport.offset.y, App.viewport.size.x, App.viewport.size.y, GL_RGBA, GL_UNSIGNED_BYTE, data);

	std::string file_name = helpers::saveScreenShot(data, App.viewport.size.x, App.viewport.size.y);
}

void Context::imguiInit()
{
	ImGui::CreateContext();
	ImGui_ImplWin32_Init(App.hwnd);
	ImGui_ImplOpenGL3_Init("#version 150");
}

void Context::imguiDestroy()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void Context::imguiStartFrame()
{
	flushVertices();

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void Context::imguiRender()
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void APIENTRY Context::debugMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* data)
{
	if (severity == GL_DEBUG_SEVERITY_NOTIFICATION)
		return;

	const char* source_str;
	const char* severity_str;

	// clang-format off
	switch (type) {
	case GL_DEBUG_TYPE_ERROR: logTrace(C_RED, false, "\nError: "); break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: logTrace(C_YELLOW, false, "\nDeprecated behavior: "); break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: logTrace(C_YELLOW, false, "\nUndefined behavior: "); break;
	case GL_DEBUG_TYPE_PORTABILITY: logTrace(C_BLUE, false, "\nPortability: "); break;
	case GL_DEBUG_TYPE_PERFORMANCE: logTrace(C_BLUE, false, "\nPerformance: "); break;
	case GL_DEBUG_TYPE_MARKER: logTrace(C_MAGENTA, false, "\nMarker: "); break;
	case GL_DEBUG_TYPE_OTHER: logTrace(C_GRAY, false, "\nOther: "); break;
	default: logTrace(C_RED, false, "\nUnknown: ");
	}

	switch (source) {
	case GL_DEBUG_SOURCE_API: source_str = "Api"; break;
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM: source_str = "Window system"; break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER: source_str = "Shader compiler"; break;
	case GL_DEBUG_SOURCE_THIRD_PARTY: source_str = "Third party"; break;
	case GL_DEBUG_SOURCE_APPLICATION: source_str = "Application"; break;
	case GL_DEBUG_SOURCE_OTHER: source_str = "Other"; break;
	default: source_str = "Unknown";
	}

	switch (severity) {
	case GL_DEBUG_SEVERITY_HIGH: severity_str = "High"; break;
	case GL_DEBUG_SEVERITY_MEDIUM: severity_str = "Medium"; break;
	case GL_DEBUG_SEVERITY_LOW: severity_str = "Low"; break;
	case GL_DEBUG_SEVERITY_NOTIFICATION: severity_str = "Notification"; break;
	default: severity_str = "Unknown";
	}
	// clang-format on

	logTrace(C_WHITE, false, "[%u / %s]: ", id, severity_str);
	logTrace(C_GRAY, true, "%s", source_str);
	trace_log("%s", message);
}

}