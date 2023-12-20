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

#include "pch.h"
#include "upscaler.h"
#include "helpers.h"
#include "option/ini.h"

#include <glslang/glslang.h>

namespace d2gl {

Upscaler::Upscaler()
{
	auto buffer = helpers::loadFile("shaders\\list.txt");
	if (!buffer.size)
		return;

	std::string preset_list((const char*)buffer.data, buffer.size);
	delete[] buffer.data;

	auto lines = helpers::strToLines(preset_list);
	if (App.direct) {
		std::string shader_path = helpers::getCurrentDir() + "data\\shaders\\";
		if (std::filesystem::is_directory(shader_path)) {
			for (const auto& p : std::filesystem::recursive_directory_iterator(shader_path)) {
				if (std::filesystem::is_directory(p))
					continue;

				std::string extension = p.path().extension().string();
				helpers::strToLower(extension);
				if (extension == ".slangp") {
					std::string path = p.path().string();
					path.erase(0, shader_path.length());
					lines.push_back(path);
				}
			}
		}
	}

	helpers::replaceAll(App.shader.preset, "/", "\\");
	for (auto& line : lines) {
		helpers::trimString(line, "\t\n\v\f\r ");
		auto buf = helpers::loadFile("shaders\\" + line);
		if (!buf.size)
			return;

		size_t pass_count = 1;
		std::string preset_source((const char*)buf.data, buf.size);
		delete[] buf.data;

		auto pos = preset_source.find("shaders =");
		if (pos == std::string::npos)
			pos = preset_source.find("shaders=");
		if (pos != std::string::npos) {
			auto pos2 = preset_source.find("\n", pos);
			auto count_str = preset_source.substr(pos, pos2 - pos);
			helpers::trimString(count_str, "\t\n\v\f\r ");
			count_str.erase(std::remove_if(count_str.begin(), count_str.end(), (int (*)(int))std::isspace), count_str.end());
			count_str = count_str.substr(8);
			count_str.erase(std::remove(count_str.begin(), count_str.end(), '"'), count_str.end());
			pass_count = std::stoul(count_str);
		}

		std::string name = line + " (" + std::to_string(pass_count) + " Pass" + (pass_count > 1 ? "es" : "") + ")";
		App.shader.presets.items.push_back({ name, line });
		if (App.shader.preset == line) {
			App.shader.presets.selected = App.shader.presets.items.size() - 1;
			App.shader.selected = App.shader.presets.selected;
		}
	}
}

bool Upscaler::loadPreset()
{
	const auto preset_name = App.shader.presets.items[App.shader.presets.selected].value;
	std::string preset_path = "shaders\\" + preset_name;

	auto buffer = helpers::loadFile(preset_path);
	if (!buffer.size)
		return false;

	std::string preset_source((const char*)buffer.data, buffer.size);
	delete[] buffer.data;

	m_passes.clear();
	m_textures.clear();
	std::unordered_map<std::string, TextureInfo> texture_info;
	std::unordered_map<std::string, float> preset_params;

	auto lines = helpers::strToLines(preset_source);
	for (auto& line : lines) {
		line = line.substr(0, line.find("#", 0));
		line.erase(std::remove_if(line.begin(), line.end(), (int (*)(int))std::isspace), line.end());
		if (line.size() == 0)
			continue;

		size_t pos = line.find('=');
		if (pos == std::string::npos)
			continue;

		helpers::trimString(line, "\t\n\v\f\r ");
		std::string value = line.substr(pos + 1, line.size());
		value.erase(std::remove(value.begin(), value.end(), '"'), value.end());
		if (value.size() == 0)
			continue;

		std::string var_str = line.substr(0, pos);

		if (m_passes.empty() && var_str == "shaders") {
			const size_t count = std::stoul(value);
			for (size_t i = 0; i < count; i++)
				m_passes.push_back({ "Pass #" + std::to_string(i + 1) });
			continue;
		}

		std::string var_name, index_s;
		for (const auto& c : var_str) {
			if (isdigit(c))
				index_s += c;
			else
				var_name += c;
		}

		int index = 0;
		if (!index_s.empty())
			index = std::stoul(index_s);
		bool pass_data = index < (int)m_passes.size();

		if (var_name == "shader" && pass_data) {
			auto shader_path = helpers::filePathFix(preset_path, value);
			if (!prepareShader(m_passes[index], shader_path))
				return false;
		} else if (var_name == "alias" && pass_data)
			m_passes[index].name = value;
		else if (var_name == "filter_linear" && pass_data)
			m_passes[index].linear_filter = (value == "true" || value == "1");
		else if (var_name == "scale_type" && pass_data) {
			if (value == "viewport")
				m_passes[index].scale_type = { ScaleType::Viewport, ScaleType::Viewport };
			else if (value == "absolute")
				m_passes[index].scale_type = { ScaleType::Absolute, ScaleType::Absolute };
		} else if (var_name == "scale_type_x" && pass_data) {
			if (value == "viewport")
				m_passes[index].scale_type.x = ScaleType::Viewport;
			else if (value == "absolute")
				m_passes[index].scale_type.x = ScaleType::Absolute;
		} else if (var_name == "scale_type_y" && pass_data) {
			if (value == "viewport")
				m_passes[index].scale_type.y = ScaleType::Viewport;
			else if (value == "absolute")
				m_passes[index].scale_type.y = ScaleType::Absolute;
		} else if (var_name == "scale" && pass_data)
			m_passes[index].scale_size = { std::stof(value), std::stof(value) };
		else if (var_name == "scale_x" && pass_data)
			m_passes[index].scale_size.x = std::stof(value);
		else if (var_name == "scale_y" && pass_data)
			m_passes[index].scale_size.y = std::stof(value);
		else if (var_str == "parameters") {
			const auto segmets = helpers::splitToVector(value, ';');
			for (auto& p : segmets)
				preset_params.insert({ p, 0.0f });
		} else if (var_str == "textures") {
			const auto segmets = helpers::splitToVector(value, ';');
			for (auto& p : segmets)
				texture_info.insert({ p, {} });
		} else {
			if (preset_params.find(var_str) != preset_params.end())
				preset_params[var_str] = std::stof(value);
			else if (texture_info.find(var_str) != texture_info.end())
				texture_info[var_str].path = helpers::filePathFix(preset_path, value);
			else {
				std::string tex_name;
				uint8_t tex_var = 0;
				if (auto pos = var_str.find("_linear"); pos != std::string::npos) {
					tex_name = var_str.substr(0, pos);
					tex_var = 1;
				} else if (auto pos = var_str.find("_mipmap"); pos != std::string::npos) {
					tex_name = var_str.substr(0, pos);
					tex_var = 2;
				} else if (auto pos = var_str.find("_wrap_mode"); pos != std::string::npos) {
					tex_name = var_str.substr(0, pos);
					tex_var = 3;
				}
				if (tex_name != "" && texture_info.find(tex_name) != texture_info.end()) {
					switch (tex_var) {
						case 1: texture_info[tex_name].linear = (value == "true" || value == "1"); break;
						case 2: texture_info[tex_name].mip_map = (value == "true" || value == "1"); break;
						case 3: texture_info[tex_name].wrap_mode = (value == "mirrored_repeat") ? GL_MIRRORED_REPEAT : GL_CLAMP_TO_EDGE; break;
					}
				}
			}
		}
	}

	for (auto& pass : m_passes) {
		for (auto& pass_param : pass.params) {
			if (preset_params.find(pass_param.id) != preset_params.end()) {
				pass_param.value = preset_params[pass_param.id];
			}
		}
	}

	size_t tex_slot = m_passes.size() + 1;
	for (auto& p : texture_info) {
		auto image_data = helpers::loadImage(p.second.path, false);
		if (image_data.data) {
			TextureCreateInfo texture_ci;
			texture_ci.size = { image_data.width, image_data.height };
			texture_ci.slot = tex_slot++;
			if (p.second.linear)
				texture_ci.filter = { GL_LINEAR, GL_LINEAR };
			texture_ci.wrap_mode = { p.second.wrap_mode, p.second.wrap_mode };
			texture_ci.mip_map = p.second.mip_map;
			m_textures.insert({ p.first, Context::createTexture(texture_ci) });

			m_textures[p.first]->fillImage(image_data);
			helpers::clearImage(image_data);
		}
	}

	option::saveString("Graphic", "shader_preset", preset_name);
	return true;
}

void Upscaler::loadDefaultPreset()
{
	const std::string defalut_preset = "bilinear.slangp";
	for (size_t i = 0; i < App.shader.presets.items.size(); i++) {
		if (App.shader.presets.items[i].value == defalut_preset) {
			App.shader.presets.selected = i;
			App.shader.selected = i;
		}
	}

	trace_log("Loading default (%s) preset!", defalut_preset.c_str());
	loadPreset();
}

void Upscaler::setupPasses()
{
	if (m_passes.empty())
		return;

	bool complete = true;

	TextureCreateInfo texture_ci;
	texture_ci.size = App.game.tex_size;
	texture_ci.slot = TEXTURE_SLOT_DEFAULT;
	if (m_passes[0].linear_filter)
		texture_ci.filter = { GL_LINEAR, GL_LINEAR };
	m_input_texture = Context::createTexture(texture_ci);

	m_passes[0].out_size = App.game.tex_size;
	glm::uvec2 vwp_size = { (uint32_t)((float)App.game.tex_size.x * App.viewport.scale.x), (uint32_t)((float)App.game.tex_size.y * App.viewport.scale.y) };

	for (size_t i = 0; i < m_passes.size(); i++) {
		auto& pass = m_passes[i];
		auto& prev = m_passes[i - (i > 0 ? 1 : 0)];
		auto& next = m_passes[i + (i < m_passes.size() - 1 ? 1 : 0)];
		const bool is_last = (i == m_passes.size() - 1);

		if (!is_last) {
			if (pass.scale_type.x != ScaleType::Absolute)
				pass.out_size.x = (uint32_t)((pass.scale_type.x == ScaleType::Source ? prev.out_size.x : vwp_size.x) * pass.scale_size.x);
			else
				pass.out_size.x = (uint32_t)pass.scale_size.x;
			if (pass.scale_type.y != ScaleType::Absolute)
				pass.out_size.y = (uint32_t)((pass.scale_type.y == ScaleType::Source ? prev.out_size.y : vwp_size.y) * pass.scale_size.y);
			else
				pass.out_size.y = (uint32_t)pass.scale_size.y;
		} else
			pass.out_size = vwp_size;

		if (!is_last) {
			GLint filter = next.linear_filter ? GL_LINEAR : GL_NEAREST;
			FrameBufferCreateInfo frambuffer_ci;
			frambuffer_ci.size = pass.out_size;
			frambuffer_ci.attachments = { { i + 1, {}, { filter, filter }, pass.format } };
			pass.frame_buffer = Context::createFrameBuffer(frambuffer_ci);
			if (!pass.frame_buffer->isComplete()) {
				complete = false;
				break;
			}
		}

		std::vector<BindingInfo> bindings;

		const auto mvp = is_last ? glm::ortho(-App.game.tex_scale.x, App.game.tex_scale.x, -App.game.tex_scale.y, App.game.tex_scale.y) : glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f);
		if (auto u = pass.getUniformPrefixed("MVP"); u != "")
			pass.pipeline->setUniformMat4f(u, mvp);
		if (auto u = pass.getSamplerName("Original"); u != "") {
			pass.pipeline->setUniform1i(u, TEXTURE_SLOT_DEFAULT);
			bindings.push_back({ BindingType::Texture, "Original", TEXTURE_SLOT_DEFAULT, &m_input_texture });
		}
		if (auto u = pass.getSamplerName("Source"); u != "") {
			pass.pipeline->setUniform1i(u, i == 0 ? TEXTURE_SLOT_DEFAULT : i);
			if (i == 0)
				bindings.push_back({ BindingType::Texture, "Source", TEXTURE_SLOT_DEFAULT, &m_input_texture });
			else
				bindings.push_back({ BindingType::FBTexture, "Source", i, &prev.frame_buffer });
		}

		if (auto u = pass.getUniformPrefixed("SourceSize"); u != "") {
			if (i == 0)
				pass.pipeline->setUniformVec4f(u, { (float)App.game.tex_size.x, (float)App.game.tex_size.y, 1.0f / (float)App.game.tex_size.x, 1.0f / (float)App.game.tex_size.y });
			else
				pass.pipeline->setUniformVec4f(u, { (float)prev.out_size.x, (float)prev.out_size.y, 1.0f / (float)prev.out_size.x, 1.0f / (float)prev.out_size.y });
		}
		if (auto u = pass.getUniformPrefixed("OutputSize"); u != "")
			pass.pipeline->setUniformVec4f(u, { (float)pass.out_size.x, (float)pass.out_size.y, 1.0f / (float)pass.out_size.x, 1.0f / (float)pass.out_size.y });
		if (auto u = pass.getUniformPrefixed("FinalViewportSize"); u != "")
			pass.pipeline->setUniformVec4f(u, { (float)vwp_size.x, (float)vwp_size.y, 1.0f / (float)vwp_size.x, 1.0f / (float)vwp_size.y });

		for (auto p : pass.params) {
			if (auto u = pass.getUniformPrefixed(p.id); u != "")
				pass.pipeline->setUniform1f(u, p.value);
		}

		if (i > 0) {
			for (size_t j = 0; j < i; j++) {
				auto& p_pass = m_passes[j];
				const glm::vec4 out_size = { (float)p_pass.out_size.x, (float)p_pass.out_size.y, 1.0f / (float)p_pass.out_size.x, 1.0f / (float)p_pass.out_size.y };
				if (p_pass.name != "") {
					if (auto u = pass.getSamplerName(p_pass.name); u != "") {
						pass.pipeline->setUniform1i(u, j + 1);
						bindings.push_back({ BindingType::FBTexture, u, j + 1, &p_pass.frame_buffer });
					}
					if (auto u = pass.getUniformPrefixed(p_pass.name + "Size"); u != "")
						pass.pipeline->setUniformVec4f(u, out_size);
				}
				const std::string num = std::to_string(j);
				if (auto u = pass.getSamplerName("PassOutput" + num); u != "") {
					pass.pipeline->setUniform1i(u, j + 1);
					bindings.push_back({ BindingType::FBTexture, u, j + 1, &p_pass.frame_buffer });
				}
				if (auto u = pass.getUniformPrefixed("PassOutputSize" + num); u != "")
					pass.pipeline->setUniformVec4f(u, out_size);
			}
		}

		if (auto u = pass.getUniformPrefixed("FrameCount"); u != "") {
			if (pass.pipeline->getUniformLocation(u) > -1)
				pass.frame_count_uniform = u;
		}

		for (auto& p : m_textures) {
			if (auto u = pass.getSamplerName(p.first); u != "") {
				pass.pipeline->setUniform1i(u, p.second->getSlot());
				bindings.push_back({ BindingType::Texture, u, p.second->getSlot(), &p.second });
			}
		}
		if (!bindings.empty())
			pass.pipeline->updateBindings(bindings);
	}

	if (!complete) {
		loadDefaultPreset();
		setupPasses();
	}
}

void Upscaler::process(const std::unique_ptr<FrameBuffer>& in_fbo, const glm::ivec2& vp_size, const glm::ivec2& vp_offset, const std::unique_ptr<FrameBuffer>& out_fbo)
{
	Context* ctx = App.context.get();
	m_input_texture->fillFromBuffer(in_fbo);

	for (size_t i = 0; i < m_passes.size(); i++) {
		const auto& pass = m_passes[i];
		const bool is_last = (i == m_passes.size() - 1);

		if (is_last) {
			if (out_fbo) {
				ctx->bindFrameBuffer(out_fbo, false);
				ctx->setViewport(App.viewport.size);
			} else {
				ctx->bindDefaultFrameBuffer();
				ctx->setViewport(vp_size, vp_offset);
			}
		} else {
			ctx->bindFrameBuffer(pass.frame_buffer, false);
			ctx->setViewport(pass.out_size);
		}
		ctx->bindPipeline(pass.pipeline);
		if (pass.frame_count_uniform != "")
			pass.pipeline->setUniform1u(pass.frame_count_uniform, ctx->getFrameCount());
		ctx->drawQuad();
	}
}

bool Upscaler::prepareShader(ShaderPass& pass, std::string shader_path)
{
	auto buffer = helpers::loadFile(shader_path);
	if (!buffer.size)
		return false;

	std::string shader_source((const char*)buffer.data, buffer.size);
	delete[] buffer.data;
	resolveInclude(shader_source, shader_path);

	uint8_t stage = 0;
	std::string vert_source, frag_source;

	auto lines = helpers::strToLines(shader_source);
	for (auto& line : lines) {
		if (line.substr(0, 7) == "#pragma") {
			helpers::trimString(line, "\t\n\v\f\r ");
			auto pragma_name = line.substr(8, line.find(' ', 8) - 8);
			auto pragma_value = line.substr(9 + pragma_name.length());

			if (pragma_name == "stage")
				stage = pragma_value == "vertex" ? 1 : 2;
			else if (pragma_name == "name") {
				pass.name = pragma_value;
				pass.label += " (" + pragma_value + ")";
			} else if (pragma_name == "format")
				pass.format = getFramebufferFormat(pragma_value);
			else if (pragma_name == "parameter") {
				std::string::iterator new_end = std::unique(pragma_value.begin(), pragma_value.end(), [](char lhs, char rhs) { return (lhs == rhs) && (lhs == ' '); });
				pragma_value.erase(new_end, pragma_value.end());
				std::string desc = "";
				size_t s_pos = pragma_value.find("\"");
				if (s_pos != std::string::npos) {
					size_t e_pos = pragma_value.rfind("\"");
					if (e_pos != std::string::npos) {
						desc = pragma_value.substr(s_pos, e_pos - s_pos + 1);
						pragma_value.replace(s_pos, e_pos - s_pos + 1, "");
						helpers::trimString(desc, "\"");
					}
				}
				const auto seg = helpers::splitToVector(pragma_value, ' ');
				desc = desc != "" ? desc : seg[1];
				const float step = seg.size() > 5 ? std::stof(seg[5]) : 1.0f;
				pass.params.push_back({ seg[0], desc, std::stof(seg[2]), std::stof(seg[3]), std::stof(seg[4]), step });
			}
			continue;
		}
		if (stage == 0 || stage == 1)
			vert_source += line + "\n";
		if (stage == 0 || stage == 2)
			frag_source += line + "\n";
	}

	glslang::Result res1, res2;
	try {
		res1 = glslang::getGLSLCode(glslang::ShaderStage::Vertex, { App.gl_ver.x, App.gl_ver.y }, vert_source);
		if (!res1.result) {
			error_log("Vertex shader compile failed! %s: %s", pass.name.c_str(), res1.err_msg.c_str());
			return false;
		}
		res2 = glslang::getGLSLCode(glslang::ShaderStage::Fragment, { App.gl_ver.x, App.gl_ver.y }, frag_source);
		if (!res2.result) {
			error_log("Fragment shader compile failed! %s: %s", pass.name.c_str(), res2.err_msg.c_str());
			return false;
		}
	} catch (...) {
		error_log("Slang to GLSL shader compile failed! %s", pass.name.c_str());
		return false;
	}
	res1.source = res1.source.erase(0, res1.source.find("\n") + 1);
	res2.source = res2.source.erase(0, res2.source.find("\n") + 1);
	std::string source = "#ifdef VERTEX\n" + res1.source + "\n#elif FRAGMENT\n" + res2.source + "\n#endif";

	PipelineCreateInfo pipeline_ci = { pass.name };
	pipeline_ci.shader = source.c_str();
	pipeline_ci.version = App.gl_ver;
	pass.pipeline = Context::createPipeline(pipeline_ci);
	if (!pass.pipeline->isCompileSuccess())
		return false;

	for (const auto& p : res1.samplers)
		pass.samplers.push_back(p);
	for (const auto& p : res2.samplers)
		pass.samplers.push_back(p);

	for (const auto& p : res1.uniforms)
		pass.uniforms[p.first] = p.second;
	for (const auto& p : res2.uniforms)
		pass.uniforms[p.first] = p.second;

	return true;
}

void Upscaler::resolveInclude(std::string& source, std::string file_path)
{
	bool dqm = false;
	int s1 = -1, s2 = -1;
	std::deque<std::pair<size_t, size_t>> cm_occurrences;

	for (size_t i = 0; i < source.size() - 1; i++) {
		const auto c0 = source[i];
		const auto c1 = source[i + 1];
		const bool end = i + 2 == source.size();
		if (!dqm) {
			if (s1 > -1 && (end || (c0 == '*' && c1 == '/'))) {
				i++;
				cm_occurrences.push_front({ s1, i + 1 - s1 });
				s1 = -1;
			} else if (s2 > -1 && (end || c0 == '\n' || c0 == '\r')) {
				cm_occurrences.push_front({ s2, i + 1 - s2 });
				s2 = -1;
			} else if (s1 == -1 && s2 == -1 && c0 == '/') {
				if (c1 == '*')
					s1 = i;
				else if (c1 == '/')
					s2 = i;
			}
		}
		if (s1 == -1 && s2 == -1 && c0 == '"')
			dqm = !dqm;
	}

	for (auto& p : cm_occurrences)
		source.erase(p.first, p.second);

	size_t start = 0;
	std::deque<std::tuple<size_t, size_t, std::string>> inc_occurrences;

	while ((start = source.find("#include", start)) != std::string::npos) {
		size_t s_pos = start + 9;
		size_t e_pos = source.find("\n", s_pos);
		e_pos = e_pos == std::string::npos ? source.length() : e_pos;
		std::string inc_file = source.substr(s_pos, e_pos - s_pos);

		helpers::trimString(inc_file, "\t\n\v\f\r ");
		inc_file.erase(std::remove(inc_file.begin(), inc_file.end(), '"'), inc_file.end());
		inc_occurrences.push_front({ start, e_pos - start, inc_file });
		start += 9;
	}

	for (auto& p : inc_occurrences) {
		auto inc_file = helpers::filePathFix(file_path, std::get<2>(p));
		std::string inc_file_source = "";
		auto buffer = helpers::loadFile(inc_file);
		if (buffer.size) {
			inc_file_source = std::string((const char*)buffer.data, buffer.size);
			delete[] buffer.data;
			resolveInclude(inc_file_source, inc_file);
			source.replace(std::get<0>(p), std::get<1>(p), inc_file_source);
		}
	}
}

std::pair<GLint, GLenum> Upscaler::getFramebufferFormat(const std::string& format)
{
	static const std::map<std::string, std::pair<GLint, GLenum>> formats = {
		{ "R8_UNORM", { GL_R8, GL_RED } },
		{ "R8_UINT", { GL_R8UI, GL_RED_INTEGER } },
		{ "R8_SINT", { GL_R8I, GL_RED_INTEGER } },
		{ "R8G8_UNORM", { GL_RG8, GL_RG } },
		{ "R8G8_UINT", { GL_RG8UI, GL_RG_INTEGER } },
		{ "R8G8_SINT", { GL_RG8I, GL_RG_INTEGER } },
		{ "R8G8B8A8_UNORM", { GL_RGBA8, GL_RGBA } },
		{ "R8G8B8A8_UINT", { GL_RGBA8UI, GL_RGBA_INTEGER } },
		{ "R8G8B8A8_SINT", { GL_RGBA8I, GL_RGBA_INTEGER } },
		{ "R8G8B8A8_SRGB", { GL_SRGB8_ALPHA8, GL_RGBA } },
		{ "A2B10G10R10_UNORM_PACK32", { GL_RGB10_A2, GL_RGBA } },
		{ "A2B10G10R10_UINT_PACK32", { GL_RGB10_A2UI, GL_RGBA_INTEGER } },
		{ "R16_UINT", { GL_R16UI, GL_RED_INTEGER } },
		{ "R16_SINT", { GL_R16I, GL_RED_INTEGER } },
		{ "R16_SFLOAT", { GL_R16F, GL_RED } },
		{ "R16G16_UINT", { GL_RG16UI, GL_RG_INTEGER } },
		{ "R16G16_SINT", { GL_RG16I, GL_RG_INTEGER } },
		{ "R16G16_SFLOAT", { GL_RG16F, GL_RG } },
		{ "R16G16B16A16_UINT", { GL_RGBA16UI, GL_RGBA_INTEGER } },
		{ "R16G16B16A16_SINT", { GL_RGBA16I, GL_RGBA_INTEGER } },
		{ "R16G16B16A16_SFLOAT", { GL_RGBA16F, GL_RGBA } },
		{ "R32_UINT", { GL_R32UI, GL_RED_INTEGER } },
		{ "R32_SINT", { GL_R32I, GL_RED_INTEGER } },
		{ "R32_SFLOAT", { GL_R32F, GL_RED } },
		{ "R32G32_UINT", { GL_RG32UI, GL_RG_INTEGER } },
		{ "R32G32_SINT", { GL_RG32I, GL_RG_INTEGER } },
		{ "R32G32_SFLOAT", { GL_RG32F, GL_RG } },
		{ "R32G32B32A32_UINT", { GL_RGBA32UI, GL_RGBA_INTEGER } },
		{ "R32G32B32A32_SINT", { GL_RGBA32I, GL_RGBA_INTEGER } },
		{ "R32G32B32A32_SFLOAT", { GL_RGBA32F, GL_RGBA } },
	};

	if (formats.find(format) != formats.end())
		return formats.at(format);

	return { GL_RGBA8, GL_RGBA };
}

}