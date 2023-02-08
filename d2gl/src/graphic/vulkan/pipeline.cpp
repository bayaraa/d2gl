#include "pch.h"
#include "context.h"
#include "utils.h"

namespace d2gl::vulkan {

Pipeline::Pipeline(const PipelineCreateInfo& info)
	: VULKAN_CONTEXT_CLASS, m_descriptor_set_count(info.descriptor_set_count), m_vertex_members(info.vertex_members), m_dynamic_states(info.dynamic_states)
{
	std::vector<vk::DescriptorSetLayoutBinding> layout_bindings;
	for (size_t i = 0; i < info.layout_bindings.size(); i++) {
		auto& binding = info.layout_bindings[i];
		auto descriptor_type = utils::getDescriptorType(binding.type);

		m_descriptor_types.push_back(descriptor_type);
		layout_bindings.push_back(utils::descriptorSetLayoutBinding(i, descriptor_type, utils::getShaderStage(binding.stage), binding.count));
	}

	const auto descriptor_set_layout = vkc->m_device.createDescriptorSetLayout(vk::DescriptorSetLayoutCreateInfo().setBindings(layout_bindings));
	assert(descriptor_set_layout.result == vk::Result::eSuccess);
	m_descriptor_set_layout = descriptor_set_layout.value;

	const auto pipeline_layout = vkc->m_device.createPipelineLayout(vk::PipelineLayoutCreateInfo().setSetLayoutCount(1).setSetLayouts(m_descriptor_set_layout));
	assert(pipeline_layout.result == vk::Result::eSuccess);
	m_pipeline_layout = pipeline_layout.value;
}

Pipeline::~Pipeline()
{
	vkc->waitIdle();

	vkc->m_device.destroyPipelineLayout(m_pipeline_layout);
	vkc->m_device.destroyDescriptorSetLayout(m_descriptor_set_layout);

	destroyPipelines();
}

void Pipeline::allocateDescriptorSets()
{
	std::vector<vk::DescriptorSetLayout> layouts;
	for (size_t i = 0; i < m_descriptor_set_count; i++)
		layouts.push_back(m_descriptor_set_layout);

	const auto info = vk::DescriptorSetAllocateInfo().setDescriptorSetCount(m_descriptor_set_count).setDescriptorPool(vkc->m_descriptor_pool).setSetLayouts(layouts);
	const auto result_value = vkc->m_device.allocateDescriptorSets(info);
	assert(result_value.result == vk::Result::eSuccess);
	m_descriptor_sets = result_value.value;
}

void Pipeline::updateDescriptorSet(const WriteSets& write_sets, uint32_t index)
{
	std::vector<vk::WriteDescriptorSet> write_descriptor_sets;
	std::vector<std::vector<vk::DescriptorBufferInfo>> buffer_info;
	std::vector<std::vector<vk::DescriptorImageInfo>> image_info;

	buffer_info.resize(write_sets.size());
	image_info.resize(write_sets.size());

	for (size_t i = 0; i < write_sets.size(); i++) {
		auto& objects = write_sets[i];
		auto set = vk::WriteDescriptorSet().setDstBinding(i).setDescriptorType(m_descriptor_types[i]).setDstSet(m_descriptor_sets[index]);

		switch (m_descriptor_types[i]) {
			case vk::DescriptorType::eUniformBuffer:
				for (auto& object : objects) {
					auto ubo = static_cast<UniformBuffer*>(object.ptr);
					buffer_info[i].push_back(ubo->descriptorBufferInfo());
				}
				set.setBufferInfo(buffer_info[i]);
				set.setDescriptorCount(buffer_info[i].size());
				break;
			case vk::DescriptorType::eCombinedImageSampler:
				for (auto& object : objects) {
					if (object.index > -1) {
						auto framebuffer = static_cast<FrameBuffer*>(object.ptr);
						image_info[i].push_back(framebuffer->descriptorImageInfo((uint32_t)object.index));
					} else {
						auto texture = static_cast<Texture*>(object.ptr);
						image_info[i].push_back(texture->descriptorImageInfo());
					}
				}
				set.setImageInfo(image_info[i]);
				set.setDescriptorCount(image_info[i].size());
				break;
		}
		write_descriptor_sets.push_back(set);
	}
	vkc->m_device.updateDescriptorSets(write_descriptor_sets, {});
}

void Pipeline::createPipelines(const ShaderSource& shader, const void* renderpass, VecBlendTypes blends)
{
	auto vk_renderpass = static_cast<const RenderPass*>(renderpass);

	auto create_info = vk::GraphicsPipelineCreateInfo();
	create_info.setRenderPass(vk_renderpass->getRenderPass());

	auto render_area = vk_renderpass->getRenderArea();
	auto vk_scissor = vk::Rect2D().setExtent(render_area.extent).setOffset(render_area.offset);
	auto vk_viewport = utils::viewport(static_cast<float>(render_area.extent.width), static_cast<float>(render_area.extent.height));
	vk_viewport.setX(static_cast<float>(render_area.offset.x)).setY(static_cast<float>(render_area.offset.y));

	auto viewport_info = vk::PipelineViewportStateCreateInfo().setViewportCount(1).setScissorCount(1).setPViewports(&vk_viewport).setPScissors(&vk_scissor);
	create_info.setPViewportState(&viewport_info);

	createPipelines(create_info, shader, blends);
}

void Pipeline::createPipelines(const ShaderSource& shader, ViewportInfo viewport, VecBlendTypes blends)
{
	auto create_info = vk::GraphicsPipelineCreateInfo();
	create_info.setRenderPass(vkc->m_renderpass);

	auto vk_viewport = utils::viewport(static_cast<float>(viewport.size.x), static_cast<float>(viewport.size.y), { static_cast<float>(viewport.offset.x), static_cast<float>(viewport.offset.y) });
	auto vk_scissor = vk::Rect2D().setExtent({ viewport.size.x, viewport.size.y }).setOffset({ viewport.offset.x, viewport.offset.y });

	std::vector<vk::DynamicState> dynamic_states;
	auto viewport_info = vk::PipelineViewportStateCreateInfo().setViewportCount(1).setScissorCount(1);

	if (m_dynamic_states & DynamicStates::Viewport)
		dynamic_states.push_back(vk::DynamicState::eViewport);
	else
		viewport_info.setPViewports(&vk_viewport);

	if (m_dynamic_states & DynamicStates::Scissor)
		dynamic_states.push_back(vk::DynamicState::eScissor);
	else
		viewport_info.setPScissors(&vk_scissor);

	create_info.setPViewportState(&viewport_info);

	auto dynamic_state_info = vk::PipelineDynamicStateCreateInfo().setDynamicStates(dynamic_states);
	create_info.setPDynamicState(&dynamic_state_info);

	createPipelines(create_info, shader, blends);
}

void Pipeline::createPipelines(vk::GraphicsPipelineCreateInfo& create_info, const ShaderSource& shader, VecBlendTypes blends)
{
	destroyPipelines();

	const auto cache_return = vkc->m_device.createPipelineCache(vk::PipelineCacheCreateInfo());
	assert(cache_return.result == vk::Result::eSuccess);
	m_pipeline_cache = cache_return.value;

	std::vector<vk::PipelineShaderStageCreateInfo> shader_stages;
	if (!shader.spirv_vert.empty())
		shader_stages.push_back(utils::pipelineShaderStageCreateInfo(shader.spirv_vert, vk::ShaderStageFlagBits::eVertex));
	if (!shader.spirv_frag.empty())
		shader_stages.push_back(utils::pipelineShaderStageCreateInfo(shader.spirv_frag, vk::ShaderStageFlagBits::eFragment));
	if (!shader.spirv_comp.empty())
		shader_stages.push_back(utils::pipelineShaderStageCreateInfo(shader.spirv_comp, vk::ShaderStageFlagBits::eCompute));

	create_info.setStages(shader_stages);

	const auto input_info = vk::PipelineInputAssemblyStateCreateInfo().setTopology(vk::PrimitiveTopology::eTriangleList);
	create_info.setPInputAssemblyState(&input_info);

	const auto rasterization_info =
	  vk::PipelineRasterizationStateCreateInfo()
		.setDepthClampEnable(VK_FALSE)
		.setRasterizerDiscardEnable(VK_FALSE)
		.setPolygonMode(vk::PolygonMode::eFill)
		.setCullMode(vk::CullModeFlagBits::eNone)
		.setFrontFace(vk::FrontFace::eClockwise)
		.setDepthBiasEnable(VK_FALSE)
		.setLineWidth(1.0f);
	create_info.setPRasterizationState(&rasterization_info);

	const auto multisample_info = vk::PipelineMultisampleStateCreateInfo();
	create_info.setPMultisampleState(&multisample_info);

	const auto depth_stencil_info = vk::PipelineDepthStencilStateCreateInfo().setStencilTestEnable(VK_FALSE).setDepthTestEnable(VK_FALSE);
	create_info.setPDepthStencilState(&depth_stencil_info);

	const auto binding_description = vk::VertexInputBindingDescription().setBinding(0).setStride(sizeof(Vertex)).setInputRate(vk::VertexInputRate::eVertex);
	const auto attribute_descriptions = vertexInputAttributeDescription();
	const auto vertex_input_info =
	  vk::PipelineVertexInputStateCreateInfo()
		.setFlags(vk::PipelineVertexInputStateCreateFlags())
		.setVertexBindingDescriptionCount(1)
		.setPVertexBindingDescriptions(&binding_description)
		.setVertexAttributeDescriptionCount(attribute_descriptions.size())
		.setPVertexAttributeDescriptions(attribute_descriptions.data());
	create_info.setPVertexInputState(&vertex_input_info);

	create_info.setLayout(m_pipeline_layout);

	std::vector<vk::GraphicsPipelineCreateInfo> pipeline_create_infos;

	const auto independent_blend = vkc->m_physical_device.features.independentBlend;

	std::vector<vk::PipelineColorBlendStateCreateInfo> color_blend_info;
	std::vector<std::vector<vk::PipelineColorBlendAttachmentState>> attachments;
	color_blend_info.resize(blends.size());
	attachments.resize(blends.size());

	for (size_t i = 0; i < blends.size(); i++) {
		attachments[i].push_back(utils::pipelineColorBlendAttachmentState(blends[i].a1));
		if (blends[i].a2 != BlendType::Undefined)
			attachments[i].push_back(utils::pipelineColorBlendAttachmentState(independent_blend ? blends[i].a2 : blends[i].a1));

		color_blend_info[i] = vk::PipelineColorBlendStateCreateInfo().setAttachments(attachments[i]);
		create_info.setPColorBlendState(&color_blend_info[i]);
		pipeline_create_infos.push_back(create_info);
	}

	auto pipeline_return = vkc->m_device.createGraphicsPipelines(m_pipeline_cache, pipeline_create_infos);
	assert(pipeline_return.result == vk::Result::eSuccess);
	m_pipelines = pipeline_return.value;

	for (auto& stage : shader_stages)
		vkc->m_device.destroyShaderModule(stage.module);
}

void Pipeline::destroyPipelines()
{
	if (m_pipeline_cache)
		vkc->m_device.destroyPipelineCache(m_pipeline_cache);
	for (auto& pipeline : m_pipelines)
		vkc->m_device.destroyPipeline(pipeline);
}

std::vector<vk::VertexInputAttributeDescription> Pipeline::vertexInputAttributeDescription()
{
	std::vector<vk::VertexInputAttributeDescription> attributes;

	if (std::find(m_vertex_members.begin(), m_vertex_members.end(), VxMember::Position) != m_vertex_members.end())
		attributes.push_back(utils::vertexInputAttributeDescription(0, offsetof(Vertex, position), vk::Format::eR32G32Sfloat));
	if (std::find(m_vertex_members.begin(), m_vertex_members.end(), VxMember::TexCoord) != m_vertex_members.end())
		attributes.push_back(utils::vertexInputAttributeDescription(1, offsetof(Vertex, tex_coord), vk::Format::eR32G32Sfloat));
	if (std::find(m_vertex_members.begin(), m_vertex_members.end(), VxMember::Color1) != m_vertex_members.end())
		attributes.push_back(utils::vertexInputAttributeDescription(2, offsetof(Vertex, color1), vk::Format::eR8G8B8A8Unorm));
	if (std::find(m_vertex_members.begin(), m_vertex_members.end(), VxMember::Color2) != m_vertex_members.end())
		attributes.push_back(utils::vertexInputAttributeDescription(3, offsetof(Vertex, color2), vk::Format::eR8G8B8A8Unorm));
	if (std::find(m_vertex_members.begin(), m_vertex_members.end(), VxMember::TexIds) != m_vertex_members.end())
		attributes.push_back(utils::vertexInputAttributeDescription(4, offsetof(Vertex, texture_ids), vk::Format::eR16G16Sint));
	if (std::find(m_vertex_members.begin(), m_vertex_members.end(), VxMember::Flags) != m_vertex_members.end())
		attributes.push_back(utils::vertexInputAttributeDescription(5, offsetof(Vertex, flags), vk::Format::eR8G8B8A8Sint));
	if (std::find(m_vertex_members.begin(), m_vertex_members.end(), VxMember::Extra) != m_vertex_members.end())
		attributes.push_back(utils::vertexInputAttributeDescription(6, offsetof(Vertex, extra), vk::Format::eR32G32Sfloat));

	return attributes;
}

}