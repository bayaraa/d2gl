#pragma once

namespace d2gl {

class UniformBuffer;

enum class BindingType {
	UniformBuffer,
	Texture,
	Image,
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
	const char* shader = nullptr;
	std::vector<BindingInfo> bindings;
	AttachmentBlends attachment_blends = { { BlendType::NoBlend } };
	bool compute = false;
};

class Pipeline {
	GLuint m_id = 0;
	AttachmentBlends m_attachment_blends;
	std::unordered_map<std::string, GLint> m_uniform_cache;
	bool m_compute = true;

public:
	Pipeline(const PipelineCreateInfo& info);
	~Pipeline();

	void bind(uint32_t index = 0);
	void dispatchCompute(int flag, glm::ivec2 work_size, GLbitfield barrier = 0);

	void setUniform1i(const std::string& name, int value);
	void setUniformVec2f(const std::string& name, const glm::vec2& value);
	void setUniformVec4f(const std::string& name, const glm::vec4& value);
	void setUniformMat4f(const std::string& name, const glm::mat4& matrix);

	inline const GLuint getId() const { return m_id; }

private:
	void setBlendState(uint32_t index = 0);
	GLint getUniformLocation(const std::string& name);

	static BlendFactors blendFactor(BlendType type);
	static GLuint createShader(const char* source, int type);
};

extern const char* g_shader_movie;
extern const char* g_shader_postfx;
extern const char* g_shader_mod;
extern const std::vector<std::pair<std::string, const char*>> g_shader_upscale;

}