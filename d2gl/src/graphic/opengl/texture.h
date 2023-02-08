#pragma once

#include "graphic/texture.h"

namespace d2gl::opengl {

class Texture final : public d2gl::Texture {
	GLuint m_id = 0;
	GLenum m_format, m_target, m_type;
	GLint m_internal_format;
	uint32_t m_width, m_height, m_channel, m_layer_count, m_slot;

public:
	Texture(const TextureCreateInfo& info);
	~Texture();

	void fill(const uint8_t* pixels, uint32_t width, uint32_t height, uint32_t offset_x = 0, uint32_t offset_y = 0, uint32_t layer = 0);
	void fillArray(const uint8_t* pixels, uint32_t div_x = 1, uint32_t div_y = 1);

public:
	void bind();
	void fillFromBuffer(uint32_t width, uint32_t height);

	inline const GLuint& getId() const { return m_id; };
	inline const uint32_t& getSlot() const { return m_slot; };
	inline const uint32_t& getWidth() const { return m_width; }
	inline const uint32_t& getHeight() const { return m_height; }
};

}