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

#ifdef VERTEX

layout(location = 0) in vec2 Position;
layout(location = 1) in vec2 TexCoord;
layout(location = 2) in vec4 Color1;
layout(location = 3) in vec4 Color2;
layout(location = 4) in uint TexNum;
layout(location = 5) in uint Flags;

uniform mat4 u_MVP;

out vec2 v_TexCoord;
out vec4 v_Color1;
out vec4 v_Color2;
flat out uint v_TexNum;
flat out uint v_Flags;

void main()
{
	gl_Position = u_MVP * vec4(Position, 0.0, 1.0);
	v_TexCoord = TexCoord;
	v_Color1 = Color1.bgra;
	v_Color2 = Color2.abgr;
	v_TexNum = TexNum;
	v_Flags = Flags;
}

// =============================================================
#elif FRAGMENT

layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 FragColorMap;

layout(std140) uniform ubo_Colors {
	vec4 u_Palette[256];
	vec4 u_Gamma[256];
};

uniform sampler2DArray u_Texture;

in vec2 v_TexCoord;
in vec4 v_Color1;
in vec4 v_Color2;
flat in uint v_TexNum;
flat in uint v_Flags;

#define checkFlag(a, b) (a & b) == b

void main()
{
	if (checkFlag(v_Flags, 0x02u))
		FragColor = v_Color2;
	else
	{
		float red = texture(u_Texture, vec3(v_TexCoord, v_TexNum)).r;

		if (checkFlag(v_Flags, 0x01u) && red == 0.0)
			discard;

		FragColor = v_Color1 * u_Palette[int(red * 255)];
	}

	FragColor.r = u_Gamma[int(FragColor.r * 255)].r;
	FragColor.g = u_Gamma[int(FragColor.g * 255)].g;
	FragColor.b = u_Gamma[int(FragColor.b * 255)].b;
	FragColor.a = checkFlag(v_Flags, 0x04u) ? v_Color2.a : 1.0;
	
	FragColorMap = vec4(0.0);
	if (checkFlag(v_Flags, 0x08u))
	{
		FragColorMap = vec4(FragColor.rgb, 0.9);
		if (checkFlag(v_Flags, 0x10u))
			FragColor = vec4(0.0);
	}
}

#endif
