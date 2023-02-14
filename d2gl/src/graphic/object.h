#pragma once

#include "types.h"

namespace d2gl {

class Object {
	Vertex m_vertices[4];
	glm::vec2 m_size;

public:
	Object(glm::vec2 position = { 0.0f, 0.0f }, glm::vec2 size = { 0.0f, 0.0f });
	~Object() = default;

	void setPosition(glm::vec2 position);
	void setSize(glm::vec2 size);
	void setTexCoord(glm::vec4 tex_coord);
	void setTexIds(glm::vec<2, int16_t> tex_ids);
	void setColor(uint32_t color, int num = 1);
	void setFlags(glm::vec<4, int8_t> flags);
	void setExtra(glm::vec2 extra);

	inline const Vertex* getVertices() { return m_vertices; };
};

}