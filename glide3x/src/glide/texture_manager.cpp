#include "pch.h"
#include "texture_manager.h"

namespace d2gl {

GlideTexture g_glide_texture;

TextureManager::TextureManager(const SubTextureCounts& size_counts)
{
	uint16_t tex_start = 0;

	for (auto& size_count : size_counts) {
		auto size = size_count.first;
		auto count = size_count.second;

		uint16_t div = 512 / size;
		uint16_t tex_count = count * div * div;

		uint8_t shift = 0;
		switch (size) {
			case 8: shift = 5; break;
			case 16: shift = 4; break;
			case 32: shift = 3; break;
			case 64: shift = 2; break;
			case 128: shift = 1; break;
		}

		m_data.insert({ size, {} });
		m_data[size].sub_texure_info.assign(tex_count + 1, { 0 });
		m_data[size].tex_count = tex_count;

		for (uint16_t i = 0; i < count; i++) {
			uint16_t n = i * div * div;
			uint16_t tex_num = tex_start + i;

			for (uint16_t y = 0; y < div; y++) {
				uint16_t ny = y * div;

				for (uint16_t x = 0; x < div; x++) {
					uint16_t ix = n + ny + x + 1;

					m_data[size].sub_texure_info[ix].tex_num = tex_num;
					m_data[size].sub_texure_info[ix].offset = { x * size, y * size };
					m_data[size].sub_texure_info[ix].shift = shift;

					m_data[size].available[ix] = true;
				}
			}
		}

		tex_start += count;
	}

	TextureCreateInfo texture_create_info;
	texture_create_info.size = { 512, 512 };
	texture_create_info.layer_count = tex_start;
	texture_create_info.format = GL_RED;
	m_texture = std::make_unique<Texture>(texture_create_info);
}

const SubTextureInfo* TextureManager::getSubTextureInfo(uint32_t address, uint16_t size, uint32_t width, uint32_t height, uint32_t frame_count)
{
	if (g_glide_texture.hash.find(address) == g_glide_texture.hash.end())
		return nullptr;

	auto hash = g_glide_texture.hash[address];
	auto& data = m_data[size];

	if (data.cache.find(address) == data.cache.end())
		data.cache.insert({ address, { frame_count } });
	auto& cache = data.cache[address];

	if (cache.last_used_frame != frame_count) {
		for (auto it = cache.items.begin(); it != cache.items.end();) {
			if ((*it).first != hash) {
				data.available[(*it).second] = true;
				it = cache.items.erase(it);
			} else
				it++;
		}
		cache.last_used_frame = frame_count;
	}

	if (cache.items.find(hash) == cache.items.end()) {
		if (data.available.begin() == data.available.end())
			return nullptr;

		const auto id = data.available.begin()->first;
		const SubTextureInfo* texture_info = &data.sub_texure_info[id];
		m_texture->fill(g_glide_texture.memory + address, width, height, texture_info->offset.x, texture_info->offset.y, texture_info->tex_num);

		cache.items.insert({ hash, id });
		data.available.erase(id);
		return texture_info;
	}

	return &data.sub_texure_info[cache.items[hash]];
}

}