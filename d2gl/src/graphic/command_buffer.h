#pragma once

namespace d2gl {

enum class CommandType {
	None,
	Begin,
	UBOUpdate,
	SetBlendState,
	DrawIndexed,
	PreFx,
	Submit,
	TakeScreenShot,
};

struct Command {
	CommandType type = CommandType::None;
	union {
		uint32_t index = 0;
		struct {
			uint32_t start;
			uint32_t count;
		} draw;
	};
};

struct TexData {
	uint32_t offset;
	uint16_t tex_num;
	glm::vec<2, uint16_t> tex_size;
	glm::vec<2, uint16_t> tex_offset;
};

struct TexUpdateQueue {
	uint32_t count = 0;
	uint32_t data_offset = 0;
	std::array<TexData, 4096> tex_data = {};
};

enum class UBOType {
	Gamma,
	Palette,
};

struct UBOData {
	UBOType type = UBOType::Gamma;
	glm::vec4 value[256] = { glm::vec4(0.0f) };
};

struct UBOUpdateQueue {
	uint32_t count = 0;
	std::array<UBOData, 16> data;
};

struct GameTexUpdate {
	uint32_t bit = 0;
	glm::vec<2, uint16_t> size = { 0, 0 };
};

#define MAX_OCCLUDERS 64 // must match the u_Occluders/u_OccluderAtten array size in mod.glsl
#define MOD_COVERAGE_CELLS 32 // module content occupancy grid, one bit per cell in a uint32 row

// Opaque/translucent panel backgrounds emitted into the game vertex stream. The module pass is
// composited after the whole game frame is resolved, so module content submitted *before* one of
// these panels would otherwise draw on top of it. Each module vertex records how many panels had
// been registered when it was pushed, and the module shader attenuates fragments that fall inside
// a panel registered later than that.
struct Occluders {
	uint32_t count = 0;
	glm::vec4 rects[MAX_OCCLUDERS] = {};    // clip space, matching the u_TextMask convention
	float attenuations[MAX_OCCLUDERS] = {}; // 1.0 - panel alpha
};

struct HDTextMasking {
	bool active = false;
	bool masking = false;
	glm::vec4 metrics;
};

class CommandBuffer {
	uint32_t m_count = 0;
	Command* m_command = nullptr;
	std::array<Command, 2048> m_commands;

	UBOUpdateQueue m_ubo_update_queue;
	TexUpdateQueue m_tex_update_queue;
	uint32_t m_vertex_count = 0;
	uint32_t m_vertex_mod_count = 0;
	GameScreen m_screen = GameScreen::InGame;

	bool m_resized = false;
	glm::uvec2 m_window_size = { 0, 0 };
	glm::uvec2 m_game_size = { 0, 0 };
	uint32_t m_game_tex_bpp = 8;

	uint8_t* m_tex_buffer = nullptr;
	GameTexUpdate m_tex_update;
	HDTextMasking m_hd_text_mask;
	Occluders m_occluders;

	friend class Context;

public:
	CommandBuffer();
	~CommandBuffer();

	void reset();
	void next();

	void pushCommand(CommandType type, uint32_t index = 0);
	void drawIndexed(uint32_t start, uint32_t count);
	void resize();

	void colorUpdate(UBOType type, const void* data);
	void textureUpdate(uint8_t* data, uint16_t tex_num, glm::vec<2, uint16_t> size, glm::vec<2, uint16_t> offset);
	void gameTextureUpdate(uint8_t* data, glm::vec<2, uint16_t> size, uint32_t bit = 1);
	void setHDTextMasking(bool masking, glm::vec4 metrics);
};

}