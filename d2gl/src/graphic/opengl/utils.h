#pragma once

namespace d2gl::opengl::utils {

void setBlendState(BlendType type, uint32_t index = 0);
GLuint createShader(const char* source, int type);

GLint getFilter(Filter filter);
GLenum getFormat(Format format);
GLint getInternalFormat(Format format);

void APIENTRY DebugMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* data);

}