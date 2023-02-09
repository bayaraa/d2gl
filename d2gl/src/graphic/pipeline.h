#pragma once

namespace d2gl {

class UniformBuffer;

enum class BindingType {
	UniformBuffer,
	Texture,
};

enum class BlendType : int8_t {
	Undefined,
	NoBlend,
	Default,
	One_Zero,
	Zero_SColor,
	One_One,
	SAlpha_OneMinusSAlpha,
};

struct ShaderSource {
	const char* vert;
	const char* frag;
	const char* comp;
};

struct BindingInfo {
	BindingType type;
	std::string name;
	uint32_t value;
};

struct BlendFactors {
	GLenum src_color;
	GLenum dst_color;
	GLenum src_alpha;
	GLenum dst_alpha;
};

typedef std::vector<std::vector<BlendType>> AttachmentBlends;

struct PipelineCreateInfo {
	ShaderSource* shader;
	std::vector<BindingInfo> bindings;
	AttachmentBlends attachment_blends = { { BlendType::NoBlend } };
};

class Pipeline {
	GLuint m_id = 0;
	AttachmentBlends m_attachment_blends;
	std::unordered_map<std::string, GLint> m_uniform_cache;

public:
	Pipeline(const PipelineCreateInfo& info);
	~Pipeline();

	void bind(uint32_t index = 0);

	void setUniform1i(const std::string& name, int value);
	void setUniformMat4f(const std::string& name, const glm::mat4& matrix);

	inline const GLuint getId() const { return m_id; }

private:
	void setBlendState(uint32_t index = 0);
	GLint getUniformLocation(const std::string& name);

	static BlendFactors blendFactor(BlendType type);
	static GLuint createShader(const char* source, int type);
};

extern const ShaderSource g_shader_movie;
extern const ShaderSource g_shader_postfx;
extern const std::vector<std::pair<std::string, ShaderSource>> g_shader_upscale;

}