/*
	D2GL: Diablo 2 LoD Glide/DDraw to OpenGL Wrapper.
	Copyright (C) 2023  Bayaraa

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once

#include "types.h"

namespace d2gl {

class FrameBuffer;

struct TextureData {
	uint32_t start_layer = 0;
	glm::vec2 coord = { 1.0f, 1.0f };
};

struct TextureCreateInfo {
	uint32_t slot = 0;
	glm::uvec2 size = { 0, 0 };
	std::pair<GLint, GLint> filter = { GL_NEAREST, GL_NEAREST };
	uint32_t layer_count = 1;
	std::pair<GLint, GLenum> format = { GL_RGBA8, GL_RGBA };
	std::pair<GLint, GLint> wrap_mode = { GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE };
	bool mip_map = false;
};

class Texture {
	GLuint m_id = 0;
	GLint m_internal_format;
	GLenum m_format, m_target, m_type;
	uint32_t m_width, m_height, m_channel, m_layer_count, m_slot;
	uint32_t m_next_layer = 0;

public:
	Texture(const TextureCreateInfo& info);
	~Texture();

	void bind(bool force = false);
	void bindImage(uint32_t unit = 0);

	void fill(const uint8_t* pixels, uint32_t width, uint32_t height, uint32_t offset_x = 0, uint32_t offset_y = 0, uint32_t layer = 0);
	void fillFromBuffer(const std::unique_ptr<FrameBuffer>& fbo, uint32_t index = 0);
	TextureData fillImage(ImageData image, uint32_t div_x = 1, uint32_t div_y = 1);

	inline const GLuint getId() const { return m_id; };
	inline const uint32_t getSlot() const { return m_slot; };
	inline const uint32_t getWidth() const { return m_width; }
	inline const uint32_t getHeight() const { return m_height; }
};

}