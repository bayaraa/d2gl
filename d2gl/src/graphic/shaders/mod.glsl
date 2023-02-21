#ifdef VERTEX

layout(location = 0) in vec2 Position;
layout(location = 1) in vec2 TexCoord;
layout(location = 2) in vec4 Color1;
layout(location = 3) in vec4 Color2;
layout(location = 4) in ivec2 TexIds;
layout(location = 5) in ivec4 Flags;
layout(location = 6) in vec2 Extra;

uniform mat4 u_MVP;

out vec4 v_Position;
out vec2 v_TexCoord;
out vec4 v_Color1;
out vec4 v_Color2;
flat out ivec2 v_TexIds;
flat out ivec4 v_Flags;
out vec2 v_Extra;

void main()
{
	v_Position = u_MVP * vec4(Position, 0.0, 1.0);
	gl_Position = v_Position;
	v_TexCoord = TexCoord;
	v_Color1 = Color1.abgr;
	v_Color2 = Color2.abgr;
	v_TexIds = TexIds;
	v_Flags = Flags;
	v_Extra = Extra;
}

// =============================================================
#elif FRAGMENT

layout(location = 0) out vec4 FragColor;

uniform sampler2D u_MapTexture;
uniform sampler2DArray u_Textures[4];

in vec4 v_Position;
in vec2 v_TexCoord;
in vec4 v_Color1;
in vec4 v_Color2;
flat in ivec2 v_TexIds;
flat in ivec4 v_Flags;
in vec2 v_Extra;

uniform vec2 u_Size;
uniform vec2 u_Scale;
uniform vec4 u_TextMask;
uniform bool u_IsMasking = false;

float msdf(vec3 rgb, float smoothess, float weight)
{
	float msd = max(min(rgb.r, rgb.g), min(max(rgb.r, rgb.g), rgb.b));
	float screen_px_dist = max(smoothess, 1.0) * (msd - (1.0 - 0.5 * weight));
	return clamp(screen_px_dist + 0.5, 0.0, 1.0);
}

vec3 greyscale(vec3 color, float str)
{
    float g = dot(color, vec3(0.299, 0.587, 0.114));
    return mix(color, vec3(g), str);
}

void main()
{
	switch(v_Flags.x) {
		case 1: FragColor = texture(u_Textures[v_TexIds.x], vec3(v_TexCoord, v_TexIds.y)) * v_Color1; break;
		case 2: FragColor = v_Color1; break;
		case 3:
			if (v_Flags.w == 1) {
				float opacity1 = msdf(texture(u_Textures[v_TexIds.x], vec3(v_TexCoord, v_TexIds.y)).rgb, v_Extra.x, v_Extra.y + 0.05);
				float opacity2 = msdf(texture(u_Textures[v_TexIds.x], vec3(v_TexCoord, v_TexIds.y)).rgb, v_Extra.x, 0.95);
				FragColor = vec4(mix(v_Color2.rgb, v_Color1.rgb, opacity2), v_Color1.a * opacity1);
			} else {
				float opacity = msdf(texture(u_Textures[v_TexIds.x], vec3(v_TexCoord, v_TexIds.y)).rgb, v_Extra.x, v_Extra.y);
				FragColor = vec4(v_Color1.rgb, v_Color1.a * opacity);
			}
			if (u_IsMasking && v_Flags.z == 0) {
				if (u_TextMask.x < v_Position.x && u_TextMask.z > v_Position.x && u_TextMask.y > v_Position.y && u_TextMask.w < v_Position.y)
					FragColor.a = 0.0;
				else
					FragColor.a *= 0.7;
			}
		break;
		case 4: FragColor = vec4(0.0); break;
		case 5:
			FragColor = texture(u_MapTexture, v_TexCoord);
			FragColor.rgb = greyscale(FragColor.rgb, 0.3);
		break;
	}

	float border = 1.00001;
	vec2 size = vec2(border / u_Scale.x / v_Extra.x, border / u_Scale.y / v_Extra.y);
	vec2 size2 = size * 2.0, size3 = size * 3.0;
	switch (v_Flags.y) {
		case 1:
			if(v_TexCoord.x < size.x || v_TexCoord.x > 1.0 - size.x || v_TexCoord.y < size.y || v_TexCoord.y > 1.0 - size.y)
				FragColor = v_Color2;
		case 2:
			if ((v_TexCoord.x > size2.x && v_TexCoord.x < size3.x) || (v_TexCoord.x < 1.0 - size2.x && v_TexCoord.x > 1.0 - size3.x) ||
				(v_TexCoord.y > size2.y && v_TexCoord.y < size3.y) || (v_TexCoord.y < 1.0 - size2.y && v_TexCoord.y > 1.0 - size3.y))
				FragColor = v_Color2;
		break;
		case 3:
			if(v_TexCoord.x < size3.x || v_TexCoord.x >= 1.0 - size3.x || v_TexCoord.y < size3.y || v_TexCoord.y >= 1.0 - size3.y)
				FragColor = v_Color1;
			if(v_TexCoord.x < size.x || v_TexCoord.x >= 1.0 - size.x || v_TexCoord.y < size.y || v_TexCoord.y >= 1.0 - size.y)
				FragColor = v_Color2;
			if ((v_TexCoord.x >= size2.x && v_TexCoord.x < size3.x) || (v_TexCoord.x < 1.0 - size2.x && v_TexCoord.x >= 1.0 - size3.x) ||
				(v_TexCoord.y >= size2.y && v_TexCoord.y < size3.y) || (v_TexCoord.y < 1.0 - size2.y && v_TexCoord.y >= 1.0 - size3.y))
				FragColor = v_Color2;
		break;
	}
}

#endif
