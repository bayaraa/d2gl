#pragma once

#include "graphic/pipeline.h"

namespace d2gl::opengl {

class Texture;

class Pipeline final : public d2gl::Pipeline {
	GLuint m_id = 0;
	std::vector<LayoutBinding> m_bindings;
	std::map<std::string, uint32_t> m_ubo_maps;
	std::map<std::string, std::vector<Texture*>> m_texture_maps;
	VecBlendTypes m_blend_states;
	ViewportInfo m_viewport;
	int m_dynamic_states;

public:
	Pipeline(const PipelineCreateInfo& info);
	~Pipeline() = default;

	void allocateDescriptorSets();
	void updateDescriptorSet(const WriteSets& write_sets, uint32_t index = 0);
	void createPipelines(const ShaderSource& shader, const void* renderpass, VecBlendTypes blends = { { BlendType::NoBlend } });
	void createPipelines(const ShaderSource& shader, ViewportInfo viewport, VecBlendTypes blends = { { BlendType::NoBlend } });

private:
	void createPipelines(const ShaderSource& shader, VecBlendTypes blends);

public:
	inline const GLuint& getId() const { return m_id; }
	inline const int& getDynamicStates() const { return m_dynamic_states; }
	inline const ViewportInfo& getViewportInfo() const { return m_viewport; }
	inline const std::map<std::string, std::vector<Texture*>>& getTextureMaps() const { return m_texture_maps; }
	inline const BlendTypes& getBlendState(uint32_t index = 0) const { return m_blend_states[index]; }
};

}