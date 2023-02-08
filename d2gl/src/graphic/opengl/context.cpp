#include "pch.h"
#include "context.h"
#include "utils.h"

#include <imgui/imgui.h>
#include <imgui/imgui_impl_opengl3.h>
#include <imgui/imgui_impl_win32.h>

namespace d2gl::opengl {

bool Context::init()
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

	HGLRC instance = wglCreateContext(App.hdc);
	wglMakeCurrent(App.hdc, instance);

	if (glewInit() != GLEW_OK) {
		error("OpenGL Extension Wrangler Library glewInit() failed.");
		return false;
	}

	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(instance);

	std::vector<glm::vec<2, uint8_t>> versions = { { 4, 6 }, { 4, 5 }, { 4, 4 }, { 4, 3 }, { 4, 2 }, { 4, 1 }, { 4, 0 }, { 3, 3 } };
	for (auto& version : versions) {
		int attribs[] = {
			WGL_CONTEXT_MAJOR_VERSION_ARB, version.x,
			WGL_CONTEXT_MINOR_VERSION_ARB, version.y,
			WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
			WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB, 0
		};
		if (App.debug)
			attribs[5] |= WGL_CONTEXT_DEBUG_BIT_ARB;

		if (m_instance = wglCreateContextAttribsARB(App.hdc, 0, attribs))
			break;
	}

	if (!m_instance) {
		error("Unsupported OpenGL version!");
		return false;
	}

	wglMakeCurrent(App.hdc, m_instance);
	glewInit();

	if (App.debug && glewIsSupported("GL_KHR_debug")) {
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(utils::DebugMessageCallback, nullptr);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
	}

	GLint major_version, minor_version;
	glGetIntegerv(GL_MAJOR_VERSION, &major_version);
	glGetIntegerv(GL_MINOR_VERSION, &minor_version);

	char version_str[50];
	sprintf_s(version_str, "%d.%d", major_version, minor_version);
	App.version = version_str;
	trace("OpenGL: %s", App.version.c_str());

	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_STENCIL_TEST);
	glEnable(GL_BLEND);

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

	glGenVertexArrays(1, &m_vertex_array);
	glBindVertexArray(m_vertex_array);

	glGenBuffers(1, &m_index_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_index_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * MAX_INDICES, indices, GL_STATIC_DRAW);
	delete[] indices;

	glGenBuffers(1, &m_vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(m_vertices.data), nullptr, GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, position));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, tex_coord));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), (const void*)offsetof(Vertex, color1));
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), (const void*)offsetof(Vertex, color2));
	glEnableVertexAttribArray(4);
	glVertexAttribIPointer(4, 2, GL_UNSIGNED_SHORT, sizeof(Vertex), (const void*)offsetof(Vertex, texture_ids));
	glEnableVertexAttribArray(5);
	glVertexAttribIPointer(5, 4, GL_UNSIGNED_BYTE, sizeof(Vertex), (const void*)offsetof(Vertex, flags));
	glEnableVertexAttribArray(6);
	glVertexAttribPointer(6, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, extra));

	// ImGui
	imguiInit();

	return true;
}

Context::~Context()
{
	glDeleteBuffers(1, &m_index_buffer);
	glDeleteBuffers(1, &m_vertex_buffer);
	glDeleteVertexArrays(1, &m_vertex_array);

	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(m_instance);
}

void Context::waitIdle() {}

void Context::startFrame()
{
	// glBindVertexArray(m_vertex_array);
	// glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer);
	//  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_index_buffer);

	// glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	// glFrontFace(GL_CW);

	current_framebuffer = 0;
	current_program = 0;

	m_vertices.ptr = m_vertices.data.data();
	m_vertices.total_count = 0;
	m_index_count = 0;

	m_render_info.vertex_count = 0;
	m_render_info.drawcall_count = 0;
}

void Context::beginFinalPass()
{
	flushVertices();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glViewport(0, 0, App.window.size.x, App.window.size.y);
}

void Context::presentFrame()
{
	flushVertices();

	SwapBuffers(App.hdc);
	// glFinish();

	m_render_info.onFrameFinish();
}

void Context::bindPipeline(const std::unique_ptr<d2gl::Pipeline>& pipeline, uint32_t index)
{
	flushVertices();

	const auto gl_pipeline = static_cast<const Pipeline*>(pipeline.get());
	const auto blend_state = gl_pipeline->getBlendState(index);

	utils::setBlendState(blend_state.a1);
	if (blend_state.a2 != BlendType::Undefined)
		utils::setBlendState(blend_state.a2, 1);

	// blend state
	// viewport
	// scissor
}

void Context::bindDescriptorSet(const std::unique_ptr<d2gl::Pipeline>& pipeline, uint32_t index)
{
	flushVertices();

	const auto gl_pipeline = static_cast<const Pipeline*>(pipeline.get());
	const auto program_id = gl_pipeline->getId();
	const auto texture_maps = gl_pipeline->getTextureMaps();

	if (current_program != program_id) {
		current_program = program_id;
		glUseProgram(program_id);
	}

	for (auto& map : texture_maps) {
		for (auto& tex : map.second) {
			tex->bind();
		}
	}
}

void Context::beginRenderPass(const std::unique_ptr<d2gl::RenderPass>& renderpass, const std::unique_ptr<d2gl::FrameBuffer>& framebuffer)
{
	flushVertices();

	const auto gl_renderpass = static_cast<const RenderPass*>(renderpass.get());
	const auto gl_framebuffer = static_cast<const FrameBuffer*>(framebuffer.get());
	const auto framebuffer_id = gl_framebuffer->getId();
	const auto attachments = gl_renderpass->getAttachments();
	const auto render_area = gl_renderpass->getRenderArea();

	if (current_framebuffer != framebuffer_id) {
		current_framebuffer = framebuffer_id;
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_id);
	}

	glViewport(render_area.offset.x, render_area.offset.y, render_area.size.x, render_area.size.y);

	for (size_t i = 0; i < attachments.size(); i++) {
		if (attachments[i].clear)
			glClearBufferfv(GL_COLOR, i, attachments[i].clear_color.data());
	}
}

void Context::setViewport(glm::uvec2 size, glm::ivec2 offset)
{
	glViewport(offset.x, offset.y, size.x, size.y);
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
	glBufferSubData(GL_ARRAY_BUFFER, 0, m_vertices.count * sizeof(Vertex), m_vertices.data.data());
	glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_INT, 0);

	m_render_info.drawcall_count++;
	m_vertices.ptr = m_vertices.data.data();
	m_vertices.count = 0;
	m_index_count = 0;
}

void Context::copyToTexture(const std::unique_ptr<d2gl::FrameBuffer>& framebuffer, const std::unique_ptr<d2gl::Texture>& texture, uint32_t index)
{
	flushVertices();

	const auto gl_framebuffer = static_cast<const FrameBuffer*>(framebuffer.get());
	const auto gl_texture = static_cast<Texture*>(texture.get());
	const auto framebuffer_id = gl_framebuffer->getId();

	if (current_framebuffer != framebuffer_id) {
		current_framebuffer = framebuffer_id;
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_id);
	}
	glReadBuffer(GL_COLOR_ATTACHMENT0 + index);

	gl_texture->fillFromBuffer(gl_framebuffer->getWidth(), gl_framebuffer->getHeight());
}

void Context::createSwapchain()
{
	wglSwapIntervalEXT(App.vsync);
	m_render_info.ftReset();
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

void Context::imguiSetup() {}

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

}