#pragma once

namespace d2gl {

class FrameBuffer;

struct TextureCreateInfo {
	uint32_t slot = 0;
	glm::uvec2 size;
	GLint min_filter = GL_NEAREST;
	GLint mag_filter = GL_NEAREST;
	uint32_t layer_count = 1;
	GLenum format = GL_RGBA;
};

class Texture {
	GLuint m_id = 0;
	GLint m_internal_format;
	GLenum m_format, m_target, m_type;
	uint32_t m_width, m_height, m_channel, m_layer_count, m_slot;

public:
	Texture(const TextureCreateInfo& info);
	~Texture();

	void bind();
	void fill(const uint8_t* pixels, uint32_t width, uint32_t height, uint32_t offset_x = 0, uint32_t offset_y = 0, uint32_t layer = 0);
	void fillArray(const uint8_t* pixels, uint32_t div_x = 1, uint32_t div_y = 1);
	void fillFromBuffer(const std::unique_ptr<FrameBuffer>& fbo, uint32_t index = 0);

	inline const GLuint getId() const { return m_id; };
	inline const uint32_t getSlot() const { return m_slot; };
	inline const uint32_t getWidth() const { return m_width; }
	inline const uint32_t getHeight() const { return m_height; }

private:
	static GLint getInternalFormat(GLenum format);
};

}