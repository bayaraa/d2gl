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

#include "pch.h"
#include "texture.h"
#include "frame_buffer.h"

namespace d2gl {

uint32_t active_texture_slot = 0;
GLuint current_binded_texture[32] = { 0 };

Texture::Texture(const TextureCreateInfo& info)
	: m_width(info.size.x), m_height(info.size.y), m_layer_count(info.layer_count), m_format(info.format), m_internal_format(Texture::getInternalFormat(info.format)),
	  m_slot(info.slot), m_target(info.layer_count == 1 ? GL_TEXTURE_2D : GL_TEXTURE_2D_ARRAY), m_type(GL_UNSIGNED_BYTE), m_channel(info.format == GL_RED ? 1 : 4)
{
	glGenTextures(1, &m_id);
	bind();

	glTexParameteri(m_target, GL_TEXTURE_MIN_FILTER, info.min_filter);
	glTexParameteri(m_target, GL_TEXTURE_MAG_FILTER, info.mag_filter);
	glTexParameteri(m_target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(m_target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	if (m_target == GL_TEXTURE_2D)
		glTexImage2D(m_target, 0, m_internal_format, m_width, m_height, 0, m_format, m_type, 0);
	else
		glTexImage3D(m_target, 0, m_internal_format, m_width, m_height, m_layer_count, 0, m_format, m_type, 0);

	if (info.min_filter != GL_LINEAR && info.mag_filter != GL_NEAREST)
		glGenerateMipmap(m_target);
}

Texture::~Texture()
{
	glDeleteTextures(1, &m_id);
}

void Texture::bind()
{
	// if (active_texture_slot != m_slot) {
	glActiveTexture(GL_TEXTURE0 + m_slot);
	active_texture_slot = m_slot;
	//}

	// if (current_binded_texture[m_slot] == m_id)
	// return;

	glBindTexture(m_target, m_id);
	current_binded_texture[m_slot] = m_id;
}

void Texture::bindImage(uint32_t unit)
{
	glBindImageTexture(unit, m_id, 0, GL_FALSE, 0, GL_WRITE_ONLY, m_internal_format);
}

void Texture::fill(const uint8_t* pixels, uint32_t width, uint32_t height, uint32_t offset_x, uint32_t offset_y, uint32_t layer)
{
	bind();

	if (m_target == GL_TEXTURE_2D)
		glTexSubImage2D(m_target, 0, offset_x, offset_y, width, height, m_format, m_type, pixels);
	else
		glTexSubImage3D(m_target, 0, offset_x, offset_y, layer, width, height, 1, m_format, m_type, pixels);
}

void Texture::fillFromBuffer(const std::unique_ptr<FrameBuffer>& fbo, uint32_t index)
{
	App.context->flushVertices();

	fbo->bind(false);
	glReadBuffer(GL_COLOR_ATTACHMENT0 + index);
	const auto width = fbo->getWidth();
	const auto height = fbo->getHeight();

	bind();
	glCopyTexSubImage2D(m_target, 0, (m_width - width) / 2, (m_height - height) / 2, 0, 0, width, height);
}

TextureData Texture::fillImage(ImageData image, uint32_t div_x, uint32_t div_y)
{
	TextureData texture_data = { m_next_layer };

	int width = glm::min(image.width / div_x, m_width);
	int height = glm::min(image.height / div_y, m_height);
	texture_data.coord = { (float)width / (float)m_width, (float)height / (float)m_height };

	if (div_x == 1 && div_y == 1) {
		if (m_target == GL_TEXTURE_2D)
			fill(image.data, image.width, image.height);
		else {
			fill(image.data, image.width, image.height, 0, 0, m_next_layer);
			m_next_layer++;
		}
	} else {
		if (div_x == 1) {
			for (size_t y = 0; y < div_y; y++) {
				int offset_buffer = y * height * image.width;
				fill(image.data + offset_buffer * m_channel, width, height, 0, 0, m_next_layer);
				m_next_layer++;
			}
		} else {
			uint8_t* pixels = new uint8_t[width * height * m_channel];
			for (size_t x = 0; x < div_x; x++) {
				for (int y = 0; y < height; y++) {
					int offset_pixel = y * width;
					int offset_buffer = x * width + y * image.width;
					std::memcpy(pixels + offset_pixel * m_channel, image.data + offset_buffer * m_channel, width * m_channel);
				}
				fill(pixels, width, height, 0, 0, m_next_layer);
				m_next_layer++;
			}
			delete[] pixels;
		}
	}

	return texture_data;
}

GLint Texture::getInternalFormat(GLenum format)
{
	switch (format) {
		case GL_RGBA: return GL_RGBA8;
		case GL_BGRA: return GL_RGBA8;
		case GL_RED: return GL_R8;
	}
	return GL_RGBA8;
}

}