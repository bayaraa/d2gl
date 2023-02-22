#include "pch.h"
#include "mini_map.h"
#include "d2/common.h"

namespace d2gl::modules {

MiniMap::MiniMap()
{
	m_bg = std::make_unique<Object>(m_pos, m_size);
	m_bg->setColor(0x000000CC, 1);
	m_bg->setColor(0x222222DD, 2);
	m_bg->setFlags({ 1, 2, 0, 0 });

	m_map = std::make_unique<Object>(m_pos, m_size);
	m_map->setFlags({ 5, 0, 0, 0 });
}

void MiniMap::resize()
{
	m_size = { 220.0f, 150.0f };
	m_pos = { App.game.size.x - m_size.x - 5.0f, 5.0f };

	const glm::vec2 zoom = App.viewport.scale;
	float r_x = ((float)App.game.size.x / zoom.x - m_size.x) / 2 / ((float)App.game.size.x / zoom.x);
	float r_y = ((float)App.game.size.y / zoom.y - m_size.y) / 2 / ((float)App.game.size.y / zoom.y);

	m_bg->setSize(m_size);
	m_bg->setPosition(m_pos);
	m_bg->setExtra(m_size);

	m_map->setSize(m_size);
	m_map->setPosition(m_pos);
	m_map->setTexCoord({ r_x, r_y, 1.0f - r_x, 1.0f - r_y });
}

void MiniMap::draw()
{
	static wchar_t time_str[20] = { 0 };
	static tm gmt_time;

	if (*d2::screen_shift == 0) {
		App.context->pushObject(m_bg);
		App.context->pushObject(m_map);

		if (App.hd_text) {
			time_t now = time(0);
			localtime_s(&gmt_time, &now);
			swprintf_s(time_str, L"%.2d:%.2d", gmt_time.tm_hour, gmt_time.tm_min);

			d2::setTextSizeHooked(99);
			d2::drawNormalTextHooked(time_str, App.game.size.x - 220, 20, 4, 0);
		}
	}
}

}