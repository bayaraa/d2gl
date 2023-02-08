#pragma once

#include "graphic/frame_buffer.h"

namespace d2gl::opengl {

class Texture;

class FrameBuffer final : public d2gl::FrameBuffer {
	GLuint m_id = 0;
	uint32_t m_width = 0, m_height = 0;
	std::vector<std::unique_ptr<Texture>> m_textures;

public:
	FrameBuffer(const FrameBufferCreateInfo& info);
	~FrameBuffer();

public:
	inline const GLuint& getId() const { return m_id; };
	inline Texture* getTexture(uint32_t index = 0) { return m_textures[index].get(); };
	inline const uint32_t& getWidth() const { return m_width; }
	inline const uint32_t& getHeight() const { return m_height; }
};

}