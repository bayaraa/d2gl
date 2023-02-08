#pragma once

namespace d2gl {

enum class BindingType {
	UniformBuffer,
	FrameBuffer,
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
	void* ptr = nullptr;
	int index = 0;
};

typedef std::vector<std::vector<BlendType>> AttachmentBlends;

struct PipelineCreateInfo {
	ShaderSource* shader;
	std::vector<BindingInfo> bindings;
	AttachmentBlends attachment_blends = { { BlendType::Default } };
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
	GLint getUniformLocation(const std::string& name);

	static GLuint createShader(const char* source, int type);
};

extern const ShaderSource g_shader_movie;
extern const ShaderSource g_shader_postfx;
extern const std::vector<std::pair<std::string, ShaderSource>> g_shader_upscale;

}