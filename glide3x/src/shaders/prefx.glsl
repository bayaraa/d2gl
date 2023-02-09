#version 330

// =============================================================
#ifdef VERTEX

layout(location = 0) in vec2 Position;
layout(location = 1) in vec2 TexCoord;
layout(location = 5) in ivec4 Flags;

uniform mat4 u_MVP;

out vec2 v_TexCoord;
flat out ivec4 v_Flags;

void main()
{
	gl_Position = u_MVP * vec4(Position, 0.0, 1.0);
	v_TexCoord = TexCoord;
	v_Flags = Flags;
}

// =============================================================
#elif FRAGMENT

layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 FragColorMap;

uniform sampler2D u_Texture;
uniform sampler2DArray u_LUTTexture;

in vec2 v_TexCoord;
flat in ivec4 v_Flags;

vec4 mixfix(vec4 a, vec4 b, float c)
{
	return (a.z < 1.0) ? mix(a, b, c) : a;
}

#define LUT_Size 32.0

vec4 LUT(vec4 color)
{
	float red = (color.r * (LUT_Size - 1.0) + 0.4999) / (LUT_Size * LUT_Size);
	float green = (color.g * (LUT_Size - 1.0) + 0.4999) / LUT_Size;
	float blue1 = (floor(color.b  * (LUT_Size - 1.0)) / LUT_Size) + red;
	float blue2 = (ceil(color.b + 0.000001 * (LUT_Size - 1.0)) / LUT_Size) + red;

	float mixer = clamp(max((color.b - blue1) / (blue2 - blue1), 0.0), 0.0, 32.0);

	vec4 color1 = texture(u_LUTTexture, vec3(vec2(blue1, green), v_Flags.x));
	vec4 color2 = texture(u_LUTTexture, vec3(vec2(blue2, green), v_Flags.x));
	return mixfix(color1, color2, mixer);
}

void main()
{
	FragColor = texture(u_Texture, v_TexCoord);
	FragColor = LUT(FragColor);

	FragColorMap = vec4(0.0, 0.0, 0.0, 0.0);
}

#endif