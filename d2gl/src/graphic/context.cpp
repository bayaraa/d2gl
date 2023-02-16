#include "pch.h"
#include "context.h"
#include "helpers.h"

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
		int attribs[] = {
			WGL_CONTEXT_MAJOR_VERSION_ARB, version.x,
			WGL_CONTEXT_MINOR_VERSION_ARB, version.y,
			WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
			WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB, 0
		};
		if (App.debug)
			attribs[5] |= WGL_CONTEXT_DEBUG_BIT_ARB;

		if (m_context = wglCreateContextAttribsARB(App.hdc, 0, attribs))
			break;
	}

	if (!m_context) {
		MessageBoxA(NULL, "Requires OpenGL 3.3 or newer!", "Unsupported OpenGL version!", MB_OK);
		exit(1);
	}

	wglMakeCurrent(App.hdc, m_context);
	glewInit();

	GLint major_version, minor_version;
	glGetIntegerv(GL_MAJOR_VERSION, &major_version);
	glGetIntegerv(GL_MINOR_VERSION, &minor_version);

	char version_str[50] = { 0 };
	sprintf_s(version_str, "%d.%d", major_version, minor_version);
	trace("OpenGL: %s", version_str);
	App.version = version_str;

	if (App.debug && glewIsSupported("GL_KHR_debug")) {
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(Context::debugMessageCallback, nullptr);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
		trace("GL_KHR_debug enabled!");
	}

	if (glewIsSupported("GL_VERSION_4_3") || glewIsSupported("GL_ARB_compute_shader"))
		App.gl_caps.compute_shader = true;

	if (glewIsSupported("GL_VERSION_4_0"))
		App.gl_caps.independent_blending = true;

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

	imguiInit();

	LARGE_INTEGER qpf;
	QueryPerformanceFrequency(&qpf);
	m_frame.frequency = double(qpf.QuadPart) / 1000.0;
	m_frame.frame_times.assign(FRAMETIME_SAMPLE_COUNT, m_frame.frame_time);

	m_limiter.timer = CreateWaitableTimer(NULL, TRUE, NULL);
	setFpsLimit(App.foreground_fps.active, App.foreground_fps.range.value);

	m_vertices_mod.count = 0;
	m_vertices_mod.ptr = m_vertices_mod.data.data();

	m_frame.vertex_count = 0;
	m_frame.drawcall_count = 0;
}

Context::~Context()
{
	imguiDestroy();

	glDeleteBuffers(1, &m_index_buffer);
	glDeleteBuffers(1, &m_vertex_buffer);
	glDeleteVertexArrays(1, &m_vertex_array);

	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(m_context);
}

void Context::beginFrame()
{
	m_vertices.count = 0;
	m_vertices.ptr = m_vertices.data.data();

	m_vertices_mod.count = 0;
	m_vertices_mod.ptr = m_vertices_mod.data.data();

	m_delay_push = false;
	m_vertices_late.count = 0;
	m_vertices_late.ptr = m_vertices_late.data.data();

	m_frame.vertex_count = 0;
	m_frame.drawcall_count = 0;
}

void Context::bindDefaultFrameBuffer()
{
	flushVertices();

	FrameBuffer::unBind();
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}

void Context::presentFrame()
{
	flushVertices();

	Sleep(1);
	SwapBuffers(App.hdc);

	if (m_limiter.active) {
		WaitForSingleObject(m_limiter.timer, (DWORD)m_limiter.frame_len_ms + 1);
		m_limiter.due_time.QuadPart += m_limiter.frame_len_ns;
		SetWaitableTimer(m_limiter.timer, &m_limiter.due_time, 0, NULL, NULL, FALSE);
	}

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
		m_vertices.ptr->flags = { x, y, z, w };
		m_vertices.ptr++;
	}
	m_vertices.count += 4;
	m_frame.vertex_count += 4;
}

void Context::flushVertices()
{
	if (m_vertices.count == 0)
		return;

	glBufferSubData(GL_ARRAY_BUFFER, 0, m_vertices.count * sizeof(Vertex), m_vertices.data.data());
	glDrawElements(GL_TRIANGLES, m_vertices.count / 4 * 6, GL_UNSIGNED_INT, 0);

	m_vertices.count = 0;
	m_vertices.ptr = m_vertices.data.data();
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

	glBufferSubData(GL_ARRAY_BUFFER, 0, m_vertices_mod.count * sizeof(Vertex), m_vertices_mod.data.data());
	glDrawElements(GL_TRIANGLES, m_vertices_mod.count / 4 * 6, GL_UNSIGNED_INT, 0);

	m_vertices_mod.count = 0;
	m_vertices_mod.ptr = m_vertices_mod.data.data();
	m_frame.drawcall_count++;
}

void Context::appendDelayedObjects()
{
	if (m_vertices_late.count == 0)
		return;

	memcpy(m_vertices_mod.ptr, m_vertices_late.data.data(), m_vertices_late.count * sizeof(Vertex));

	m_vertices_mod.count += m_vertices_late.count;
	m_vertices_mod.ptr += m_vertices_late.count;

	m_delay_push = false;
	m_vertices_late.count = 0;
	m_vertices_late.ptr = m_vertices_late.data.data();
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
	trace("%s", message);
}

}