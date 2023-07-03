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
/*
	Part of this file is part of D2DX.
	https://github.com/bolrog/d2dx/blob/main/src/d2dx/UnitMotionPredictor.cpp
	https://github.com/bolrog/d2dx/blob/main/src/d2dx/WeatherMotionPredictor.cpp

	Copyright (C) 2021  Bolrog

	D2DX is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	D2DX is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with D2DX. If not, see <https://www.gnu.org/licenses/>.
*/

#include "pch.h"
#include "motion_prediction.h"
#include "d2/common.h"
#include "d2/funcs.h"
#include "d2/stubs.h"
#include "helpers.h"

#include <detours/detours.h>

namespace d2gl::modules {

MotionPrediction::MotionPrediction()
{
	m_units.reserve(1024);
}

void MotionPrediction::toggle(bool active)
{
	if (!m_active && active) {
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		DetourAttach(&(PVOID&)d2::drawUnit, isVerMax(V_110) || isVer(V_114d) ? d2::drawUnitStubESI : d2::drawUnitStubStack);
		if (isVerNot(V_109d) && isVerNot(V_110))
			DetourAttach(&(PVOID&)d2::drawMissile, d2::drawMissileStub);
		DetourAttach(&(PVOID&)d2::drawWeatherParticles, isVer(V_114d) ? d2::drawWeatherParticlesStub114d : d2::drawWeatherParticlesStub);
		DetourTransactionCommit();

		m_active = true;
		d2::patch_motion_prediction->toggle(m_active);
	} else if (m_active && !active) {
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		DetourDetach(&(PVOID&)d2::drawUnit, isVerMax(V_110) || isVer(V_114d) ? d2::drawUnitStubESI : d2::drawUnitStubStack);
		if (isVerNot(V_109d) && isVerNot(V_110))
			DetourDetach(&(PVOID&)d2::drawMissile, d2::drawMissileStub);
		DetourDetach(&(PVOID&)d2::drawWeatherParticles, isVer(V_114d) ? d2::drawWeatherParticlesStub114d : d2::drawWeatherParticlesStub);
		DetourTransactionCommit();

		m_active = false;
		d2::patch_motion_prediction->toggle(m_active);
	}
	m_global_offset = { 0, 0 };
	m_player_motion.offset = { 0, 0 };
}

void MotionPrediction::update()
{
	if (!isAvailable()) {
		m_global_offset = { 0, 0 };
		m_player_motion.offset = { 0, 0 };
		return;
	}

	m_frame_time = d2::isEscMenuOpen() ? 0.0f : (float)(App.context->getFrameTime() / 1000.0);

	const auto frame_time_ms = (int64_t)((App.vsync || App.foreground_fps.active ? App.context->getAvgFrameTime() : App.context->getFrameTime()) * (65536.0 / 1000.0));
	int32_t delta = (int32_t)glm::max((int64_t)INT_MIN, glm::min((int64_t)INT_MAX, frame_time_ms));

	m_player_motion.unit = d2::getPlayerUnit();
	setUnitMotion(&m_player_motion, delta);

	const auto frame = App.context->getFrameCount() - 1;
	for (auto it = m_units.begin(); it != m_units.end();) {
		if (it->second.frame != frame)
			it = m_units.erase(it);
		else {
			if (it->second.unit = d2::findUnit(it->first)) {
				setUnitMotion(&it->second, delta);
				it++;
			} else
				it = m_units.erase(it);
		}
	}

	m_perspective = d2::isPerspective();
	m_global_offset = getUnitOffset(&m_player_motion);
}

glm::ivec2 MotionPrediction::getGlobalOffset(bool skip)
{
	if (App.game.draw_stage == DrawStage::World && (!skip || !m_perspective))
		return m_global_offset;

	return { 0, 0 };
}

glm::ivec2 MotionPrediction::getGlobalOffsetPerspective()
{
	if (App.game.draw_stage == DrawStage::World && m_draw_fn == D2DrawFn::PerspectiveImage)
		return m_global_offset;

	return { 0, 0 };
}

glm::ivec2 MotionPrediction::getUnitOffset(uint32_t type_id)
{
	if (isActive())
		return m_units[type_id].offset;

	return { 0, 0 };
}

glm::ivec2 MotionPrediction::drawImage(int x, int y, D2DrawFn fn, uint32_t gamma, int draw_mode)
{
	glm::ivec2 pos = { x, y };
	m_draw_fn = fn;

	if (!isAvailable() || fn == D2DrawFn::PerspectiveImage)
		return pos;

	if (fn == D2DrawFn::Shadow || fn == D2DrawFn::ImageFast) {
		if (glm::max(abs(pos.x - m_player_motion.screen_pos.x), abs(pos.y - m_player_motion.screen_pos.y)) < 16)
			return pos;
		else {
			for (auto& item : m_units) {
				if (glm::max(abs(item.second.screen_pos.x - pos.x), abs(item.second.screen_pos.y - pos.y)) < 16) {
					pos += getUnitOffset(&item.second);
					break;
				}
			}
		}
		return pos - m_global_offset;
	}

	if (d2::currently_drawing_unit) {
		if (d2::currently_drawing_unit == d2::getPlayerUnit()) {
			m_player_motion.screen_pos = pos;
			return pos;
		} else {
			if (d2::currently_drawing_unit->dwType == d2::UnitType::Player || d2::currently_drawing_unit->dwType == d2::UnitType::Monster || d2::currently_drawing_unit->dwType == d2::UnitType::Missile) {
				const uint32_t type_id = d2::getUnitID(d2::currently_drawing_unit) | ((uint8_t)d2::currently_drawing_unit->dwType << 24);
				UnitMotion* unit_motion = &m_units[type_id];
				const uint32_t frame = App.context->getFrameCount();

				if (unit_motion->frame != frame) {
					unit_motion->unit = d2::currently_drawing_unit;
					unit_motion->frame = frame;
					unit_motion->screen_pos = pos;
				}
				if (fn == D2DrawFn::Image)
					pos += getUnitOffset(unit_motion);
			}
			return pos - m_global_offset;
		}
	} else {
		if (App.game.draw_stage == DrawStage::World && fn == D2DrawFn::Image && draw_mode == 3 && gamma == 0xFFFFFFFF)
			return pos - m_global_offset;
	}

	return pos;
}

glm::ivec2 MotionPrediction::drawLine(int start_x, int start_y)
{
	if (!isAvailable() || !d2::currently_drawing_weather_particles)
		return { 0, 0 };

	uint32_t particle_index = *d2::currently_drawing_weather_particle_index_ptr;
	if (particle_index == m_last_particle_index)
		particle_index += 256;

	const auto frame = App.context->getFrameCount();
	ParticleMotion& particle = m_particles[particle_index & 511];
	m_last_particle_index = particle_index;

	if (particle.frame != frame) {
		const glm::ivec2 pos = { start_x, start_y };
		const glm::ivec2 diff = pos - particle.last_pos;
		const int error = glm::max(abs(diff.x), abs(diff.y));

		if (frame - particle.frame > 2 || error > 100) {
			particle.last_pos = pos;
			particle.predicted_pos = pos;
			particle.velocity = { 0.0f, 0.0f };
		} else {
			if (error > 0) {
				particle.velocity = { 25.0f * diff.x, 25.0f * diff.y };
				particle.last_pos = pos;
			}
		}

		particle.predicted_pos += particle.velocity * m_frame_time;
		particle.offset = -glm::ivec2(particle.predicted_pos - particle.last_pos);
		particle.frame = frame;
	}

	return particle.offset;
}

glm::ivec2 MotionPrediction::drawSolidRect()
{
	if (isAvailable() && m_text_fn == D2DrawFn::NormalText)
		return m_global_offset;

	return { 0, 0 };
}

glm::ivec2 MotionPrediction::drawText(const wchar_t* str, int x, int y, D2DrawFn fn)
{
	glm::ivec2 pos = { x, y };
	if (!isAvailable() || !str)
		return pos;

	if (fn == m_text_fn)
		pos -= m_global_offset;

	return pos;
}

void MotionPrediction::unitHoverTextMotion()
{
	if (m_active) {
		*d2::hover_text_x -= m_global_offset.x;
		*d2::hover_text_y -= m_global_offset.y;
	}
}

void MotionPrediction::altItemsTextMotion()
{
	if (!m_active || !d2::alt_item_pos)
		return;

	static int* x1 = nullptr;
	static int* x2 = nullptr;
	static int* y1 = nullptr;
	static int* y2 = nullptr;

	switch (helpers::getVersion()) {
		case Version::V_109d:
			x1 = (int*)((uintptr_t)d2::alt_item_pos + 0x0C);
			x2 = (int*)((uintptr_t)d2::alt_item_pos + 0x20);
			y1 = (int*)((uintptr_t)d2::alt_item_pos + 0x18);
			y2 = (int*)((uintptr_t)d2::alt_item_pos + 0x10);
			break;
		case Version::V_110:
			x1 = (int*)((uintptr_t)d2::alt_item_pos + 0x30);
			x2 = (int*)((uintptr_t)d2::alt_item_pos + 0x2C);
			y1 = (int*)((uintptr_t)d2::alt_item_pos + 0x14);
			y2 = (int*)((uintptr_t)d2::alt_item_pos + 0x24);
			break;
		case Version::V_114d:
			x1 = (int*)((uintptr_t)d2::alt_item_pos + 0x4C);
			x2 = (int*)((uintptr_t)d2::alt_item_pos + 0x2C);
			y1 = (int*)((uintptr_t)d2::alt_item_pos + 0x40);
			y2 = (int*)((uintptr_t)d2::alt_item_pos + 0x34);
			break;
		default:
			x1 = (int*)((uintptr_t)d2::alt_item_pos + 0x34);
			x2 = (int*)((uintptr_t)d2::alt_item_pos + 0x30);
			y1 = (int*)((uintptr_t)d2::alt_item_pos + 0x18);
			y2 = (int*)((uintptr_t)d2::alt_item_pos + 0x28);
	}
	*x1 -= m_global_offset.x;
	*y1 -= m_global_offset.y;
	*x2 -= m_global_offset.x;
	*y2 -= m_global_offset.y;
}

void MotionPrediction::setUnitMotion(UnitMotion* unit_motion, int32_t delta)
{
	const d2::Path* path = d2::getUnitPath(unit_motion->unit);
	const glm::ivec2 unit_pos = { (int32_t)path->x, (int32_t)path->y };

	glm::ivec2 pos_whole = { unit_pos.x >> 16, unit_pos.y >> 16 };
	glm::ivec2 last_pos_whole = { unit_motion->last_pos.x >> 16, unit_motion->last_pos.y >> 16 };
	glm::ivec2 predicted_pos_whole = { unit_motion->predicted_pos.x >> 16, unit_motion->predicted_pos.y >> 16 };

	int32_t last_pos_md = std::max(abs(pos_whole.x - last_pos_whole.x), abs(pos_whole.y - last_pos_whole.y));
	int32_t predicted_pos_md = std::max(abs(pos_whole.x - predicted_pos_whole.x), abs(pos_whole.y - predicted_pos_whole.y));

	if (last_pos_md > 2 || predicted_pos_md > 2) {
		unit_motion->predicted_pos = unit_pos;
		unit_motion->corrected_pos = unit_pos;
		unit_motion->last_pos = unit_pos;
		unit_motion->velocity = { 0, 0 };
	}

	const int32_t dx = unit_pos.x - unit_motion->last_pos.x;
	const int32_t dy = unit_pos.y - unit_motion->last_pos.y;

	unit_motion->dt_last_pos_change += delta;

	if (dx != 0 || dy != 0 || unit_motion->dt_last_pos_change >= (65536 / 25)) {
		unit_motion->corrected_pos.x = ((int64_t)unit_pos.x + unit_motion->last_pos.x) >> 1;
		unit_motion->corrected_pos.y = ((int64_t)unit_pos.y + unit_motion->last_pos.y) >> 1;

		unit_motion->velocity.x = 25 * dx;
		unit_motion->velocity.y = 25 * dy;

		unit_motion->last_pos = unit_pos;
		unit_motion->dt_last_pos_change = 0;
	}

	if (unit_motion->velocity.x != 0 || unit_motion->velocity.y != 0) {
		if (unit_motion->dt_last_pos_change < (65536 / 25)) {
			glm::ivec2 step = { (int32_t)(((int64_t)delta * unit_motion->velocity.x) >> 16), (int32_t)(((int64_t)delta * unit_motion->velocity.y) >> 16) };

			const int32_t correction = 7000;
			const int32_t one_minus_correction = 65536 - correction;

			unit_motion->predicted_pos.x = (int32_t)(((int64_t)unit_motion->predicted_pos.x * one_minus_correction + (int64_t)unit_motion->corrected_pos.x * correction) >> 16);
			unit_motion->predicted_pos.y = (int32_t)(((int64_t)unit_motion->predicted_pos.y * one_minus_correction + (int64_t)unit_motion->corrected_pos.y * correction) >> 16);

			unit_motion->predicted_pos.x += step.x;
			unit_motion->predicted_pos.y += step.y;

			unit_motion->corrected_pos.x += step.x;
			unit_motion->corrected_pos.y += step.y;
		}
	}

	unit_motion->offset_update = true;
}

glm::ivec2 MotionPrediction::getUnitOffset(UnitMotion* unit_motion)
{
	if (unit_motion->offset_update) {
		const glm::vec2 offset = { (unit_motion->predicted_pos.x - unit_motion->last_pos.x) / 65536.0f, (unit_motion->predicted_pos.y - unit_motion->last_pos.y) / 65536.0f };
		const glm::vec2 scale_factors = { 32.0f / sqrtf(2.0f), 16.0f / sqrtf(2.0f) };
		const glm::vec2 screen_offset = scale_factors * glm::vec2(offset.x - offset.y, offset.x + offset.y) + 0.5f;
		unit_motion->offset = { (int)screen_offset.x, (int)screen_offset.y };
		unit_motion->offset_update = false;
	}
	return unit_motion->offset;
}

}