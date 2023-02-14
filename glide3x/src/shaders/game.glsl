#ifdef VERTEX

layout(location = 0) in vec2 Position;
layout(location = 1) in vec2 TexCoord;
layout(location = 2) in vec4 Color1;
layout(location = 3) in vec4 Color2;
layout(location = 4) in ivec2 TexIds;
layout(location = 5) in ivec4 Flags;

uniform mat4 u_MVP;

out vec2 v_TexCoord;
out vec4 v_Color1;
out vec4 v_Color2;
flat out ivec2 v_TexIds;
flat out ivec4 v_Flags;

void main()
{
	gl_Position = u_MVP * vec4(Position, 0.0, 1.0);
	v_TexCoord = TexCoord;
	v_Color1 = Color1.bgra;
	v_Color2 = Color2.abgr;
	v_TexIds = TexIds;
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
flat in ivec2 v_TexIds;
flat in ivec4 v_Flags;

void main()
{
	if (v_Flags.y == 1)
		FragColor = v_Color2;
	else
	{
		float red = texture(u_Texture, vec3(v_TexCoord, v_TexIds.y)).r;
		if (v_Flags.x == 1 && red == 0.0)
			discard;

		FragColor = v_Color1 * u_Palette[int(red * 255)];
	}

	FragColor.r = u_Gamma[int(FragColor.r * 255)].r;
	FragColor.g = u_Gamma[int(FragColor.g * 255)].g;
	FragColor.b = u_Gamma[int(FragColor.b * 255)].b;
	FragColor.a = (v_Flags.z == 1) ? v_Color2.a : 1.0;

	FragColorMap = vec4(0.0);
	if (v_Flags.w > 0)
	{
		FragColorMap = vec4(FragColor.rgb, 0.9);
		if (v_Flags.w > 1)
			FragColor = vec4(0.0);
	}
}

#endif
