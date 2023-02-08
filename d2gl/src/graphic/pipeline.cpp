#include "pch.h"
#include "pipeline.h"
#include "frame_buffer.h"
#include "texture.h"
#include "uniform_buffer.h"

namespace d2gl {

GLuint current_program = 0;

Pipeline::Pipeline(const PipelineCreateInfo& info)
	: m_attachment_blends(info.attachment_blends)
{
	m_id = glCreateProgram();

	GLuint vs = 0, fs = 0, cs = 0;
	if (info.shader->vert) {
		vs = createShader(info.shader->vert, GL_VERTEX_SHADER);
		glAttachShader(m_id, vs);
	}
	if (info.shader->frag) {
		fs = createShader(info.shader->frag, GL_FRAGMENT_SHADER);
		glAttachShader(m_id, fs);
	}
	if (info.shader->comp) {
		cs = createShader(info.shader->comp, GL_COMPUTE_SHADER);
		glAttachShader(m_id, cs);
	}

	glLinkProgram(m_id);
	glValidateProgram(m_id);
	glDeleteShader(vs);
	glDeleteShader(fs);
	glDeleteShader(cs);

	glUseProgram(m_id);

	for (auto& binding : info.bindings) {
		switch (binding.type) {
			case BindingType::UniformBuffer: {
				const auto ubo = static_cast<UniformBuffer*>(binding.ptr);
				GLuint ubo_index = glGetUniformBlockIndex(m_id, binding.name.c_str());
				glUniformBlockBinding(m_id, ubo_index, ubo->getBinding());
				break;
			}
			case BindingType::FrameBuffer: {
				const auto fbo = static_cast<FrameBuffer*>(binding.ptr);
				setUniform1i(binding.name, fbo->getTexture(binding.index)->getSlot());
				break;
			}
			case BindingType::Texture: {
				const auto tex = static_cast<Texture*>(binding.ptr);
				setUniform1i(binding.name, tex->getSlot());
				break;
			}
		};
	}

	glUseProgram(0);
	current_program = 0;
}

Pipeline::~Pipeline()
{
	glDeleteProgram(m_id);
}

void Pipeline::bind(uint32_t index)
{
	if (current_program == m_id)
		return;

	App.context->flushVertices();

	glUseProgram(m_id);
	current_program = m_id;
}

void Pipeline::setUniform1i(const std::string& name, int value)
{
	bind();
	glUniform1i(getUniformLocation(name), value);
}

void Pipeline::setUniformMat4f(const std::string& name, const glm::mat4& matrix)
{
	bind();
	glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, &matrix[0][0]);
}

GLint Pipeline::getUniformLocation(const std::string& name)
{
	if (m_uniform_cache.find(name) != m_uniform_cache.end())
		return m_uniform_cache[name];

	int location = glGetUniformLocation(m_id, name.c_str());
	if (location == -1) {
		error("Uniform (%s) not found!", name.c_str());
		return -1;
	}

	m_uniform_cache.insert({ name, location });
	return location;
}

GLuint Pipeline::createShader(const char* source, int type)
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

// clang-format off
const ShaderSource g_shader_movie = {
	#include "shaders/movie.glsl.h"
};

const ShaderSource g_shader_postfx = {
	#include "shaders/postfx.glsl.h"
};

const std::vector<std::pair<std::string, ShaderSource>> g_shader_upscale = {
	{ "simple-sharp-bilinear", {
		#include "shaders/upscale/simple-sharp-bilinear.glsl.h"
	} },
	//{ "xbr-lv2-noblend", {
	//	#include "shaders/upscale/xbr-lv2-noblend.glsl.h"
	//} },
	//{ "xbrz-freescale", {
	//	#include "shaders/upscale/xbrz-freescale.glsl.h"
	//} },
	//{ "2xbr-hybrid-v5-gamma", {
	//	#include "shaders/upscale/2xbr-hybrid-v5-gamma.glsl.h"
	//} },
	//{ "aa-shader-4.0", {
	//	#include "shaders/upscale/aa-shader-4.0.glsl.h"
	//} },
	//{ "advanced-aa", {
	//	#include "shaders/upscale/advanced-aa.glsl.h"
	//} },
};
// clang-format on

}