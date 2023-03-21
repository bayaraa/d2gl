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
	glm::uvec2 size = { 0, 0 };
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
	void clearBuffer();

	inline const GLuint getId() const { return m_id; };
	inline Texture* getTexture(uint32_t index = 0) { return m_textures[index].get(); };
	inline const uint32_t getWidth() const { return m_width; }
	inline const uint32_t getHeight() const { return m_height; }
};

}