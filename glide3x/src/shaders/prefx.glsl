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

layout(std140) uniform ubo_Metrics {
	float u_BloomExp;
	float u_BloomGamma;
	vec2 u_RelSize;
};

uniform sampler2D u_Texture;
uniform sampler2D u_BloomTexture1;
uniform sampler2D u_BloomTexture2;
uniform sampler2DArray u_LUTTexture;

in vec2 v_TexCoord;
flat in ivec4 v_Flags;

#define CoefLuma vec3(0.2126, 0.7152, 0.0722)

vec4 BloomOut()
{
	vec3 color = texture(u_Texture, v_TexCoord).rgb;
	float luma = dot(color, CoefLuma);

	if (luma > 0.97) {
		color = mix(vec3(dot(color, CoefLuma)), color, 50.0);
		return vec4(color, 1.0);
	}

	return vec4(0.0);
}

vec4 BlurPass13(sampler2D tex, int dir)
{
	vec4 res = vec4(0.0);
	vec2 pos[13];

	if (dir == 0) {
		for (int i = -6; i <= 6; i++)
			pos[i + 6] = v_TexCoord + vec2(u_RelSize.x * i, 0.0);
	} else {
		for (int i = -5; i <= 6; i++)
			pos[i + 6] = v_TexCoord + vec2(0.0, u_RelSize.y * i);
	}
	
	res += texture(tex, pos[ 0]) * 0.000244140625;
	res += texture(tex, pos[ 1]) * 0.0029296875;
	res += texture(tex, pos[ 2]) * 0.01611328125;
	res += texture(tex, pos[ 3]) * 0.0537109375;
	res += texture(tex, pos[ 4]) * 0.120849609375;
	res += texture(tex, pos[ 5]) * 0.193359375;
	res += texture(tex, pos[ 6]) * 0.2255859375;
	res += texture(tex, pos[ 7]) * 0.193359375;
	res += texture(tex, pos[ 8]) * 0.120849609375;
	res += texture(tex, pos[ 9]) * 0.0537109375;
	res += texture(tex, pos[10]) * 0.01611328125;
	res += texture(tex, pos[11]) * 0.0029296875;
	res += texture(tex, pos[12]) * 0.000244140625;

	return res;
}

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

	vec4 color1 = texture(u_LUTTexture, vec3(vec2(blue1, green), v_Flags.y - 1));
	vec4 color2 = texture(u_LUTTexture, vec3(vec2(blue2, green), v_Flags.y - 1));
	return mixfix(color1, color2, mixer);
}

void main()
{
	switch(v_Flags.x) {
		case 0: FragColor = BloomOut(); break;
		case 1: FragColor = BlurPass13(u_BloomTexture2, 0); break;
		case 2: FragColor = BlurPass13(u_BloomTexture2, 1); break;
		case 3:
			vec3 out_color = texture(u_Texture, v_TexCoord).rgb;
			if (v_Flags.z == 1) {
				vec3 bloom_color = texture(u_BloomTexture1, v_TexCoord).rgb;
				bloom_color = vec3(1.0) - exp(-bloom_color * u_BloomExp);
				bloom_color = pow(bloom_color, vec3(1.0 / u_BloomGamma));
				out_color = clamp(out_color + bloom_color, 0.0, 1.0);
			}

			FragColor = vec4(out_color, 1.0);
			FragColor = v_Flags.y > 0 ? LUT(FragColor) : FragColor;
		break;
	}

	FragColorMap = vec4(0.0, 0.0, 0.0, 0.0);
}

// =============================================================
#elif COMPUTE
//[
layout(local_size_x = 16, local_size_y = 16) in;
//]
readonly uniform sampler2D u_InTexture;
writeonly uniform image2D u_OutTexture;

uniform int u_Flag = 0;

vec4 BlurPass17(sampler2D tex, ivec2 tc, int dir)
{
	vec4 res = vec4(0.0);
	ivec2 max_size = textureSize(tex, 0) - 1;
	ivec2 pos[17];

	if (dir == 0)
	{
		for (int i = -8; i <= 8; i++)
			pos[i + 8] = ivec2(clamp(tc.x + i, 1, max_size.x), tc.y);
	}
	else
	{
		for (int i = -8; i <= 8; i++)
			pos[i + 8] = ivec2(tc.x, clamp(tc.y + i, 1, max_size.y));
	}

	res += texelFetch(tex, pos[ 0], 0) * 0.00007775057148979088;
	res += texelFetch(tex, pos[ 1], 0) * 0.0004886523773990668;
	res += texelFetch(tex, pos[ 2], 0) * 0.002402783593203998;
	res += texelFetch(tex, pos[ 3], 0) * 0.009244812937837047;
	res += texelFetch(tex, pos[ 4], 0) * 0.027835276522173394;
	res += texelFetch(tex, pos[ 5], 0) * 0.06559213033177563;
	res += texelFetch(tex, pos[ 6], 0) * 0.12098003019548202;
	res += texelFetch(tex, pos[ 7], 0) * 0.17467018127877265;
	res += texelFetch(tex, pos[ 8], 0) * 0.1974167643837327;
	res += texelFetch(tex, pos[ 9], 0) * 0.17467018127877265;
	res += texelFetch(tex, pos[10], 0) * 0.12098003019548202;
	res += texelFetch(tex, pos[11], 0) * 0.06559213033177563;
	res += texelFetch(tex, pos[12], 0) * 0.027835276522173394;
	res += texelFetch(tex, pos[13], 0) * 0.009244812937837047;
	res += texelFetch(tex, pos[14], 0) * 0.002402783593203998;
	res += texelFetch(tex, pos[15], 0) * 0.0004886523773990668;
	res += texelFetch(tex, pos[16], 0) * 0.00007775057148979088;

	return res;
}

void main()
{
	ivec2 v_TexCoord = ivec2(gl_GlobalInvocationID.xy);
	vec4 color = vec4(0.0);

	switch (u_Flag) {
		case 0: color = BlurPass17(u_InTexture, v_TexCoord, 0); break;
		case 1: color = BlurPass17(u_InTexture, v_TexCoord, 1); break;
	}

	imageStore(u_OutTexture, v_TexCoord, color);
}

#endif
