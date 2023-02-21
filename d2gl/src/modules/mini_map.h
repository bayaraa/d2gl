#pragma once

namespace d2gl::modules {

class MiniMap {
	std::unique_ptr<Object> m_bg, m_map;
	glm::vec2 m_pos = { 0.0f, 0.0f };
	glm::vec2 m_size = { 0.0f, 0.0f };

	MiniMap();
	~MiniMap() = default;

public:
	static MiniMap& Instance()
	{
		static MiniMap instance;
		return instance;
	}

	void resize();
	void draw();
};

}