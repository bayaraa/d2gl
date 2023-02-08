#include "pch.h"
#include "utils.h"

namespace d2gl::opengl::utils {

void setBlendState(BlendType type, uint32_t index)
{
	if (type != BlendType::NoBlend) {
		// glDrawBuffer(GL_COLOR_ATTACHMENT0 + index);
		glBlendEquation(GL_FUNC_ADD);

		switch (type) {
			case BlendType::One_Zero:
				// glBlendFuncSeparate(GL_ONE, GL_ZERO, GL_ONE, GL_ZERO);
				glBlendFuncSeparatei(index, GL_ONE, GL_ZERO, GL_ONE, GL_ZERO);
				break;
			case BlendType::Zero_SColor:
				// glBlendFuncSeparate(GL_ZERO, GL_SRC_COLOR, GL_ZERO, GL_SRC_COLOR);
				glBlendFuncSeparatei(index, GL_ZERO, GL_SRC_COLOR, GL_ZERO, GL_SRC_COLOR);
				break;
			case BlendType::One_One:
				// glBlendFuncSeparate(GL_ONE, GL_ONE, GL_ZERO, GL_ONE);
				glBlendFuncSeparatei(index, GL_ONE, GL_ONE, GL_ZERO, GL_ONE);
				break;
			case BlendType::SAlpha_OneMinusSAlpha:
				// glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
				glBlendFuncSeparatei(index, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
				break;
			default:
				// glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
				glBlendFuncSeparatei(index, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
		}
	}
}

GLuint createShader(const char* source, int type)
{
	GLuint id = glCreateShader(type);
	GLint length = strlen(source);

	glShaderSource(id, 1, &source, &length);
	glCompileShader(id);

	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE) {
		int lenght;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &lenght);
		char* message = (char*)_malloca(lenght * sizeof(char));
		glGetShaderInfoLog(id, lenght, &lenght, message);
		error("Shader compile failed! (%s) | %s", (type == GL_VERTEX_SHADER ? "VERTEX" : (type == GL_FRAGMENT_SHADER ? "FRAGMENT" : "COMPUTE")), message);

		glDeleteShader(id);
		return 0;
	}
	return id;
}

GLint getFilter(Filter filter)
{
	switch (filter) {
		case Filter::Linear: return GL_LINEAR;
	}
	return GL_NEAREST;
}

GLenum getFormat(Format format)
{
	switch (format) {
		case Format::R8G8B8A8Unorm: return GL_RGBA;
		case Format::B8G8R8A8Unorm: return GL_BGRA;
		case Format::R8Srgb:
		case Format::R8Unorm: return GL_RED;
	}
	return GL_RGBA;
}

GLint getInternalFormat(Format format)
{
	switch (format) {
		case Format::R8G8B8A8Unorm: return GL_RGBA8;
		case Format::B8G8R8A8Unorm: return GL_RGBA8;
		case Format::R8Srgb:
		case Format::R8Unorm: return GL_R8;
	}
	return GL_RGBA8;
}

void APIENTRY DebugMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* data)
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