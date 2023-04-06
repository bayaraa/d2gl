#pragma once

namespace d2gl {

struct Vertex {
	glm::vec<2, int16_t> position;
	glm::vec2 tex_coord;
	uint32_t color1;
	uint32_t color2;
	uint16_t tex_num;
	uint16_t flags;

	static void bindingDescription()
	{
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_HALF_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, position));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, tex_coord));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), (const void*)offsetof(Vertex, color1));
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), (const void*)offsetof(Vertex, color2));
		glEnableVertexAttribArray(4);
		glVertexAttribIPointer(4, 1, GL_UNSIGNED_SHORT, sizeof(Vertex), (const void*)offsetof(Vertex, tex_num));
		glEnableVertexAttribArray(5);
		glVertexAttribIPointer(5, 1, GL_UNSIGNED_SHORT, sizeof(Vertex), (const void*)offsetof(Vertex, flags));
	}
};

struct VertexMod {
	glm::vec<2, int16_t> position;
	glm::vec2 tex_coord;
	uint32_t color1;
	uint32_t color2;
	uint16_t tex_num;
	glm::vec<4, uint8_t> flags;
	glm::vec<2, int16_t> extra;

	static void bindingDescription()
	{
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_HALF_FLOAT, GL_FALSE, sizeof(VertexMod), (const void*)offsetof(VertexMod, position));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(VertexMod), (const void*)offsetof(VertexMod, tex_coord));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(VertexMod), (const void*)offsetof(VertexMod, color1));
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(VertexMod), (const void*)offsetof(VertexMod, color2));
		glEnableVertexAttribArray(4);
		glVertexAttribIPointer(4, 1, GL_UNSIGNED_SHORT, sizeof(VertexMod), (const void*)offsetof(VertexMod, tex_num));
		glEnableVertexAttribArray(5);
		glVertexAttribIPointer(5, 4, GL_UNSIGNED_BYTE, sizeof(VertexMod), (const void*)offsetof(VertexMod, flags));
		glEnableVertexAttribArray(6);
		glVertexAttribPointer(6, 2, GL_HALF_FLOAT, GL_FALSE, sizeof(VertexMod), (const void*)offsetof(VertexMod, extra));
	}
};

struct GlideVertex {
	float x, y;
	uint32_t pargb;
	float scale;
	float s, t;
	uint32_t unused;
};

}