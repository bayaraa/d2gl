#include "pch.h"
#include "context.h"
#include "utils.h"

namespace d2gl::opengl {

Pipeline::Pipeline(const PipelineCreateInfo& info)
	: m_bindings(info.layout_bindings), m_dynamic_states(info.dynamic_states)
{}

void Pipeline::allocateDescriptorSets() {}

void Pipeline::updateDescriptorSet(const WriteSets& write_sets, uint32_t index)
{
	for (size_t i = 0; i < write_sets.size(); i++) {
		auto& binding = m_bindings[i];
		auto& objects = write_sets[i];

		if (binding.type == DescriptorType::UniformBuffer) {
			for (auto& object : objects) {
				auto ubo = static_cast<UniformBuffer*>(object.ptr);
				m_ubo_maps[binding.name] = ubo->getBinding();
			}

		} else if (binding.type == DescriptorType::ImageSampler) {
			m_texture_maps[binding.name].clear();

			for (auto& object : objects) {
				if (object.index > -1) {
					auto framebuffer = static_cast<FrameBuffer*>(object.ptr);
					auto texture = framebuffer->getTexture((uint32_t)object.index);
					m_texture_maps[binding.name].push_back(texture);
				} else {
					auto texture = static_cast<Texture*>(object.ptr);
					m_texture_maps[binding.name].push_back(texture);
				}
			}
		}
	}
}

void Pipeline::createPipelines(const ShaderSource& shader, const void* renderpass, VecBlendTypes blends)
{
	const auto gl_renderpass = static_cast<const RenderPass*>(renderpass);
	const auto render_area = gl_renderpass->getRenderArea();

	m_viewport.offset = render_area.offset;
	m_viewport.size = render_area.size;

	createPipelines(shader, blends);
}

void Pipeline::createPipelines(const ShaderSource& shader, ViewportInfo viewport, VecBlendTypes blends)
{
	m_viewport = viewport;

	createPipelines(shader, blends);
}

void Pipeline::createPipelines(const ShaderSource& shader, VecBlendTypes blends)
{
	if (m_id)
		return;

	m_id = glCreateProgram();

	GLuint vs = 0, fs = 0, cs = 0;
	if (shader.glsl_vert) {
		vs = utils::createShader(shader.glsl_vert, GL_VERTEX_SHADER);
		glAttachShader(m_id, vs);
	}
	if (shader.glsl_frag) {
		fs = utils::createShader(shader.glsl_frag, GL_FRAGMENT_SHADER);
		glAttachShader(m_id, fs);
	}
	if (shader.glsl_comp) {
		cs = utils::createShader(shader.glsl_comp, GL_COMPUTE_SHADER);
		glAttachShader(m_id, cs);
	}

	glLinkProgram(m_id);
	glValidateProgram(m_id);

	glDeleteShader(vs);
	glDeleteShader(fs);
	glDeleteShader(cs);

	glUseProgram(m_id);

	for (auto& map : m_ubo_maps) {
		GLuint ubo_index = glGetUniformBlockIndex(m_id, map.first.c_str());
		glUniformBlockBinding(m_id, ubo_index, map.second);
	}

	for (auto& map : m_texture_maps) {
		for (size_t i = 0; i < map.second.size(); i++) {
			std::string name = map.first;
			if (map.second.size() > 1)
				name += "[" + std::to_string(i) + "]";

			GLint location = glGetUniformLocation(m_id, name.c_str());
			if (location == -1) {
				error("Shader(%s) location = -1", map.first.c_str());
				continue;
			}
			glUniform1i(location, map.second[i]->getSlot());
		}
	}

	glUseProgram(0);

	m_blend_states = blends;
}

}