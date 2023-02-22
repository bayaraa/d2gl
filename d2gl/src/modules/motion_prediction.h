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

#include "d2/structs.h"

namespace d2gl::modules {

struct UnitMotion {
	uint32_t frame = 0;
	d2::UnitAny* unit = nullptr;
	glm::ivec2 screen_pos = { 0, 0 };
	glm::ivec2 offset = { 0, 0 };
	bool offset_update = false;

	glm::ivec2 last_pos = { 0, 0 };
	glm::ivec2 predicted_pos = { 0, 0 };
	glm::ivec2 corrected_pos = { 0, 0 };
	glm::ivec2 velocity = { 0, 0 };
	int64_t dt_last_pos_change = 0;
};

struct ParticleMotion {
	uint32_t frame = 0;
	glm::ivec2 offset = { 0, 0 };

	glm::ivec2 last_pos = { 0, 0 };
	glm::ivec2 predicted_pos = { 0, 0 };
	glm::vec2 velocity = { 0.0f, 0.0f };
};

class MotionPrediction {
	bool m_active = false;
	float m_frame_time = 0.0f;
	glm::ivec2 m_global_offset = { 0, 0 };

	std::unordered_map<uint32_t, UnitMotion> m_units;
	UnitMotion m_player_motion;
	bool m_perspective = false;

	D2DrawFn m_text_fn = D2DrawFn::None;
	D2DrawFn m_draw_fn = D2DrawFn::None;

	std::array<ParticleMotion, 512> m_particles;
	uint32_t m_last_particle_index = 0;

	MotionPrediction() = default;
	~MotionPrediction() = default;

public:
	static MotionPrediction& Instance()
	{
		static MotionPrediction instance;
		return instance;
	}

	inline bool isActive() { return m_active; }
	void toggle(bool active = true);
	void update();

	glm::ivec2 getGlobalOffset(bool skip = false);
	glm::ivec2 getGlobalOffsetPerspective();

	glm::ivec2 drawImage(int x, int y, D2DrawFn fn, uint32_t gamma = 0, int draw_mode = 0);
	glm::ivec2 drawLine(int start_x, int start_y);
	glm::ivec2 drawSolidRect();
	glm::ivec2 drawText(const wchar_t* str, int x, int y, D2DrawFn fn);

	void unitHoverTextMotion();
	void altItemsTextMotion();

	inline void textMotion(D2DrawFn fn) { m_text_fn = fn; }

private:
	void setUnitMotion(UnitMotion* unit_motion, int32_t delta);
	glm::ivec2 getUnitOffset(UnitMotion* unit_motion);

	inline bool isAvailable() { return m_active && App.game.screen == GameScreen::InGame; }
};

}