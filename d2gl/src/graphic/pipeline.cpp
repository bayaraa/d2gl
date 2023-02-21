#include "pch.h"
#include "pipeline.h"
#include "frame_buffer.h"
#include "texture.h"
#include "uniform_buffer.h"

namespace d2gl {

GLuint current_program = 0;
uint32_t current_blend_index = 10;

Pipeline::Pipeline(const PipelineCreateInfo& info)
	: m_attachment_blends(info.attachment_blends), m_compute(info.compute)
{
	m_id = glCreateProgram();

	GLuint vs = 0, fs = 0, cs = 0;
	if (m_compute) {
		cs = createShader(info.shader, GL_COMPUTE_SHADER);
		glAttachShader(m_id, cs);
	} else {
		vs = createShader(info.shader, GL_VERTEX_SHADER);
		glAttachShader(m_id, vs);

		fs = createShader(info.shader, GL_FRAGMENT_SHADER);
		glAttachShader(m_id, fs);
	}

	glLinkProgram(m_id);
	glValidateProgram(m_id);
	glDeleteShader(vs);
	glDeleteShader(fs);
	glDeleteShader(cs);

	glUseProgram(m_id);

	if (info.bindings.size() > 0) {
		for (auto& binding : info.bindings) {
			switch (binding.type) {
				case BindingType::UniformBuffer: {
					GLuint ubo_index = glGetUniformBlockIndex(m_id, binding.name.c_str());
					glUniformBlockBinding(m_id, ubo_index, binding.value);
					break;
				}
				case BindingType::Texture:
				case BindingType::Image: {
					setUniform1i(binding.name, binding.value);
					break;
				}
				case BindingType::TextureArray: {
					for (int i = 0; i < 4; i++) {
						char uf_name[20];
						sprintf_s(uf_name, "u_Textures[%d]", i);
						setUniform1i(uf_name, i);
					}
					break;
				}
			};
		}
	}

	glUseProgram(0);
	current_program = 0;
	current_blend_index = 10;
}

Pipeline::~Pipeline()
{
	glDeleteProgram(m_id);
}

void Pipeline::bind(uint32_t index)
{
	if (current_program == m_id && current_blend_index == index)
		return;

	App.context->flushVertices();

	if (current_program != m_id) {
		glUseProgram(m_id);
		current_program = m_id;
		current_blend_index = 10;
	}

	if (current_blend_index != index) {
		setBlendState(index);
		current_blend_index = index;
	}
}

void Pipeline::setBlendState(uint32_t index)
{
	const auto& blends = m_attachment_blends[index];
	if (blends[0] == BlendType::NoBlend) {
		glDisable(GL_BLEND);
		return;
	}

	glEnable(GL_BLEND);
	if (App.gl_caps.independent_blending) {
		for (size_t i = 0; i < blends.size(); i++) {
			const auto factor = blendFactor(blends[i]);
			glBlendFuncSeparatei(i, factor.src_color, factor.dst_color, factor.src_alpha, factor.dst_alpha);
		}
	} else {
		const auto factor = blendFactor(blends[0]);
		glBlendFuncSeparate(factor.src_color, factor.dst_color, factor.src_alpha, factor.dst_alpha);
	}
}

BlendFactors Pipeline::blendFactor(BlendType type)
{
	switch (type) {
		case BlendType::One_Zero: return { GL_ONE, GL_ZERO, GL_ONE, GL_ZERO };
		case BlendType::Zero_SColor: return { GL_ZERO, GL_SRC_COLOR, GL_ZERO, GL_SRC_COLOR };
		case BlendType::One_One: return { GL_ONE, GL_ONE, GL_ZERO, GL_ONE };
		case BlendType::SAlpha_OneMinusSAlpha: return { GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA };
	}
	return { GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA };
}

void Pipeline::setUniform1i(const std::string& name, int value)
{
	bind();
	glUniform1i(getUniformLocation(name), value);
}

void Pipeline::setUniformVec2f(const std::string& name, const glm::vec2& value)
{
	bind();
	glUniform2fv(getUniformLocation(name), 1, &value.x);
}

void Pipeline::setUniformVec4f(const std::string& name, const glm::vec4& value)
{
	bind();
	glUniform4fv(getUniformLocation(name), 1, &value.x);
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

void Pipeline::dispatchCompute(int flag, glm::ivec2 work_size, GLbitfield barrier)
{
	setUniform1i("u_Flag", flag);
	glDispatchCompute(work_size.x, work_size.y, 1);

	if (barrier)
		glMemoryBarrier(barrier);
}

GLuint Pipeline::createShader(const char* source, int type)
{
	std::string shader_src = "#version";
	switch (type) {
		case GL_VERTEX_SHADER: shader_src += " 330\n#define VERTEX 1\n"; break;
		case GL_FRAGMENT_SHADER: shader_src += " 330\n#define FRAGMENT 1\n"; break;
		case GL_COMPUTE_SHADER: shader_src += " 430\n#define COMPUTE 1\n"; break;
	}
	shader_src += source;

	GLuint id = glCreateShader(type);
	GLint length = shader_src.length();
	const char* src = shader_src.c_str();

	glShaderSource(id, 1, &src, &length);
	glCompileShader(id);

	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE) {
		int lenght;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &lenght);
		char* message = (char*)_malloca(lenght * sizeof(char));
		glGetShaderInfoLog(id, lenght, &lenght, message);
		error_log("Shader compile failed! (%s) | %s", (type == GL_VERTEX_SHADER ? "VERTEX" : (type == GL_FRAGMENT_SHADER ? "FRAGMENT" : "COMPUTE")), message);

		glDeleteShader(id);
		return 0;
	}

	return id;
}

const char* g_shader_movie = {
#include "shaders/movie.glsl.h"
};

const char* g_shader_postfx = {
#include "shaders/postfx.glsl.h"
};

const char* g_shader_mod = {
#include "shaders/mod.glsl.h"
};

// clang-format off
const std::vector<std::pair<std::string, const char*>> g_shader_upscale = {
	{ "simple-sharp-bilinear", {
		#include "shaders/upscale/simple-sharp-bilinear.glsl.h"
	} },
	{ "xbr-lv2-noblend", {
		#include "shaders/upscale/xbr-lv2-noblend.glsl.h"
	} },
	{ "xbrz-freescale", {
		#include "shaders/upscale/xbrz-freescale.glsl.h"
	} },
	{ "2xbr-hybrid-v5-gamma", {
		#include "shaders/upscale/2xbr-hybrid-v5-gamma.glsl.h"
	} },
	{ "aa-shader-4.0", {
		#include "shaders/upscale/aa-shader-4.0.glsl.h"
	} },
	{ "advanced-aa", {
		#include "shaders/upscale/advanced-aa.glsl.h"
	} },
};
// clang-format on

}