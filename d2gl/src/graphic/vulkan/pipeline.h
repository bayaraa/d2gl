#pragma once

#include "graphic/pipeline.h"

namespace d2gl::vulkan {

class Context;

class Pipeline final : public d2gl::Pipeline {
	Context* vkc;
	vk::PipelineCache m_pipeline_cache;
	vk::PipelineLayout m_pipeline_layout;
	std::vector<vk::Pipeline> m_pipelines;

	size_t m_descriptor_set_count;
	vk::DescriptorSetLayout m_descriptor_set_layout;
	std::vector<vk::DescriptorSet> m_descriptor_sets;

	std::vector<vk::DescriptorType> m_descriptor_types;
	std::vector<VxMember> m_vertex_members;
	int m_dynamic_states;

public:
	Pipeline(const PipelineCreateInfo& info);
	~Pipeline();

	void allocateDescriptorSets();
	void updateDescriptorSet(const WriteSets& write_sets, uint32_t index = 0);
	void createPipelines(const ShaderSource& shader, const void* renderpass, VecBlendTypes blends = { { BlendType::NoBlend } });
	void createPipelines(const ShaderSource& shader, ViewportInfo viewport, VecBlendTypes blends = { { BlendType::NoBlend } });

private:
	void createPipelines(vk::GraphicsPipelineCreateInfo& create_info, const ShaderSource& shader, VecBlendTypes blends);
	void destroyPipelines();
	std::vector<vk::VertexInputAttributeDescription> vertexInputAttributeDescription();

public:
	inline const vk::PipelineLayout& getPipelineLayout() const { return m_pipeline_layout; }
	inline const vk::Pipeline& getPipeline(uint32_t index = 0) const { return m_pipelines[index]; }
	inline const vk::DescriptorSet& getDescriptorSet(uint32_t index = 0) const { return m_descriptor_sets[index]; }
};

}