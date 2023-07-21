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

#include "pipeline.h"

namespace d2gl {

enum class ScaleType {
	Source,
	Viewport,
	Absolute,
};

struct ShaderParam {
	std::string id;
	std::string desc;
	float value = 0.0f;
	float min = 0.0f;
	float max = 0.0f;
	float step = 1.0f;
};

struct TextureInfo {
	std::string path;
	bool linear = true;
	bool mip_map = false;
	GLint wrap_mode = GL_CLAMP_TO_EDGE;
};

struct ShaderPass {
	std::string label;
	std::string name = "";
	glm::vec<2, uint32_t> out_size = { 0, 0 };
	glm::vec<2, ScaleType> scale_type = { ScaleType::Source, ScaleType::Source };
	glm::vec2 scale_size = { 1.0f, 1.0f };
	bool linear_filter = false;
	std::string frame_count_uniform = "";

	std::vector<ShaderParam> params;
	std::unique_ptr<Pipeline> pipeline;
	std::pair<GLint, GLenum> format = { GL_RGBA8, GL_RGBA };
	std::unique_ptr<FrameBuffer> frame_buffer;
	std::vector<std::string> samplers;
	std::unordered_map<std::string, std::string> uniforms;

	inline std::string getSamplerName(const std::string& sampler) const { return std::find(samplers.begin(), samplers.end(), sampler) != samplers.end() ? sampler : ""; }
	inline std::string getUniformPrefixed(const std::string& uniform) const { return uniforms.find(uniform) != uniforms.end() ? uniforms.at(uniform) + "." + uniform : ""; }
};

class Upscaler {
	std::vector<ShaderPass> m_passes = {};
	std::unique_ptr<Texture> m_input_texture;
	std::unordered_map<std::string, std::unique_ptr<Texture>> m_textures;

	Upscaler();
	~Upscaler() = default;

public:
	static Upscaler& Instance()
	{
		static Upscaler instance;
		return instance;
	}

	bool loadPreset();
	void loadDefaultPreset();
	void setupPasses();
	void process(const std::unique_ptr<FrameBuffer>& in_fbo, const glm::ivec2& vp_size, const glm::ivec2& vp_offset, const std::unique_ptr<FrameBuffer>& out_fbo = nullptr);

private:
	bool prepareShader(ShaderPass& pass, std::string shader_path);

	static void resolveInclude(std::string& source, std::string file_path);
	static std::pair<GLint, GLenum> getFramebufferFormat(const std::string& format);
};

}