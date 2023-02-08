#version 450

#ifdef SPIRV
#define layout_binding_std140(a) layout(binding = a, std140)
#define layout_location(a) layout(location = a)
#define layout_binding(a) layout(binding = a)
#else
#define layout_binding_std140(a) layout(std140)
#define layout_location(a)
#define layout_binding(a)
#endif

// =============================================================
#ifdef VERTEX

layout_binding_std140(0) uniform ubo_MVPs {
	mat4 u_mvp_game;
	mat4 u_mvp_upscale;
	mat4 u_mvp_movie;
	mat4 u_mvp_normal;
};

layout(location = 0) in vec2 Position;
layout(location = 1) in vec2 TexCoord;
layout(location = 5) in ivec4 Flags;

layout_location(0) out vec2 v_TexCoord;
layout_location(1) flat out ivec4 v_Flags;

void main()
{
	gl_Position = u_mvp_normal * vec4(Position, 0.0, 1.0);
	v_TexCoord = TexCoord;
	v_Flags = Flags;
}

// =============================================================
#elif FRAGMENT

layout_binding(1) uniform sampler2D u_Texture;
layout_binding(2) uniform sampler2DArray u_LUTTexture;

layout_location(0) in vec2 v_TexCoord;
layout_location(1) flat in ivec4 v_Flags;

layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 FragColorMap;

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