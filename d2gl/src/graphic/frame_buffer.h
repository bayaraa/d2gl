#pragma once

namespace d2gl {

class Texture;

struct FrameBufferAttachment {
	uint32_t slot = 0;
	std::array<float, 4> clear_color = { 0.0f, 0.0f, 0.0f, 1.0f };
	GLint min_filter = GL_NEAREST;
	GLint mag_filter = GL_NEAREST;
	GLenum format = GL_RGBA;
};

struct FrameBufferCreateInfo {
	glm::uvec2 size;
	std::vector<FrameBufferAttachment> attachments;
};

class FrameBuffer {
	GLuint m_id = 0;
	uint32_t m_width, m_height;
	std::vector<std::unique_ptr<Texture>> m_textures;
	std::vector<std::array<float, 4>> m_clear_colors;

public:
	FrameBuffer(const FrameBufferCreateInfo& info);
	~FrameBuffer();

	void bind(bool clear = true);
	static void unBind();

	inline const GLuint getId() const { return m_id; };
	inline Texture* getTexture(uint32_t index = 0) { return m_textures[index].get(); };
	inline const uint32_t getWidth() const { return m_width; }
	inline const uint32_t getHeight() const { return m_height; }

private:
	void clearBuffer();
};

}