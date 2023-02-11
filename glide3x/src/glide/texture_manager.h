#pragma once

#include "graphic/texture.h"

namespace d2gl {

struct SubTextureInfo {
	uint8_t shift;
	uint16_t tex_num;
	glm::vec<2, uint16_t> offset;
};

struct TextureCache {
	uint32_t last_used_frame = 0;
	std::unordered_map<uint32_t, uint16_t> items;
};

struct TextureManagerData {
	uint16_t tex_count = 0;
	std::vector<SubTextureInfo> sub_texure_info;
	std::unordered_map<uint16_t, bool> available;
	std::unordered_map<uint32_t, TextureCache> cache;
};

typedef std::vector<std::pair<uint16_t, uint16_t>> SubTextureCounts;

struct GlideTexture {
	uint8_t* memory = nullptr;
	std::map<uint32_t, uint32_t> hash;
};

extern GlideTexture g_glide_texture;

class TextureManager {
	std::unique_ptr<Texture> m_texture;
	std::map<uint16_t, TextureManagerData> m_data;

public:
	TextureManager(const SubTextureCounts& size_counts);
	~TextureManager() = default;

	inline Texture* getTexture() { return m_texture.get(); }
	inline size_t getUsage(uint16_t size) { return m_data[size].tex_count - m_data[size].available.size(); }

	const SubTextureInfo* getSubTextureInfo(uint32_t address, uint16_t size, uint32_t width, uint32_t height, uint32_t frame_count);
};

}