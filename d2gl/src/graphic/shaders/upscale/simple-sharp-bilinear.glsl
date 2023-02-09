#version 330

/*
	Author: rsn8887 (based on TheMaister)
	License: Public domain
	This is an integer prescale filter that should be combined
	with a bilinear hardware filtering (GL_BILINEAR filter or some such) to achieve
	a smooth scaling result with minimum blur. This is good for pixelgraphics
	that are scaled by non-integer factors.
   
	The prescale factor and texel coordinates are precalculated
	in the vertex shader for speed.
*/

// =============================================================
#ifdef VERTEX

layout(location = 0) in vec2 Position;
layout(location = 1) in vec2 TexCoord;

uniform mat4 u_MVP;

out vec2 v_TexCoord;

void main()
{
	gl_Position = u_MVP * vec4(Position, 0.0, 1.0);
	v_TexCoord = TexCoord;
}

// =============================================================
#elif FRAGMENT

layout(location = 0) out vec4 FragColor;

layout(std140) uniform ubo_Metrics {
	vec2 u_OutSize;
	vec2 u_TexSize;
	vec2 u_RelSize;
};

uniform sampler2D u_Texture;

in vec2 v_TexCoord;

void main()
{
	vec2 texel = v_TexCoord * u_TexSize;
	vec2 scale = max(floor(u_OutSize / u_TexSize), vec2(1.0, 1.0));

	vec2 texel_floored = floor(texel);
	vec2 s = fract(texel);
	vec2 region_range = 0.5 - 0.5 / scale;

	vec2 center_dist = s - 0.5;
	vec2 f = (center_dist - clamp(center_dist, -region_range, region_range)) * scale + 0.5;

	vec2 mod_texel = texel_floored + f;

	FragColor = vec4(texture(u_Texture, mod_texel / u_TexSize).rgb, 1.0);
}

#endif