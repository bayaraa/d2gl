/*
	Hyllian's xBR-vertex code and texel mapping
   
	Copyright (C) 2011/2016 Hyllian - sergiogdb@gmail.com
	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is 
	furnished to do so, subject to the following conditions:
	The above copyright notice and this permission notice shall be included in
	all copies or substantial portions of the Software.
	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
	THE SOFTWARE.
*/

// This shader also uses code and/or concepts from xBRZ as it appears
// in the Desmume source code. The license for which is as follows:

// ****************************************************************************
// * This file is part of the HqMAME project. It is distributed under         *
// * GNU General Public License: http://www.gnu.org/licenses/gpl-3.0          *
// * Copyright (C) Zenju (zenju AT gmx DOT de) - All Rights Reserved          *
// *                                                                          *
// * Additionally and as a special exception, the author gives permission     *
// * to link the code of this program with the MAME library (or with modified *
// * versions of MAME that use the same license as MAME), and distribute      *
// * linked combinations including the two. You must obey the GNU General     *
// * Public License in all respects for all of the code used other than MAME. *
// * If you modify this file, you may extend this exception to your version   *
// * of the file, but you are not obligated to do so. If you do not wish to   *
// * do so, delete this exception statement from your version.                *
// ****************************************************************************

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

float DistCalc(vec3 pixA, vec3 pixB)
{
	const vec3 w = vec3(0.2627, 0.6780, 0.0593);
	const float scaleB = 0.5 / (1.0 - w.b);
	const float scaleR = 0.5 / (1.0 - w.r);
	vec3 diff = pixA - pixB;
	float Y = dot(diff.rgb, w);
	float Cb = scaleB * (diff.b - Y);
	float Cr = scaleR * (diff.r - Y);

	return sqrt((Y * Y) + (Cb * Cb) + (Cr * Cr));
}

bool IsPixEq(const vec3 pixA, const vec3 pixB)
{
	return (DistCalc(pixA, pixB) < 30.0 / 255.0);
}

float GetLeftRatio(vec2 center, vec2 origin, vec2 dir, vec2 scale)
{
	vec2 P0 = center - origin;
	vec2 proj = dir * (dot(P0, dir) / dot(dir, dir));
	vec2 distv = P0 - proj;
	vec2 orth = vec2(-dir.y, dir.x);
	float side = sign(dot(P0, orth));
	float v = side * length(distv * scale);

	return smoothstep(-sqrt(2.0) / 2.0, sqrt(2.0) / 2.0, v);
}

#define  eq(a,b) (a == b)
#define neq(a,b) (a != b)

#define P(x,y) texture(u_Texture, coord + u_RelSize * vec2(x, y)).rgb

void main()
{
	//---------------------------------------
	// Input Pixel Mapping:  -|x|x|x|-
	//                       x|A|B|C|x
	//                       x|D|E|F|x
	//                       x|G|H|I|x
	//                       -|x|x|x|-

	vec2 scale = u_OutSize * u_RelSize;
	vec2 pos = fract(v_TexCoord * u_TexSize) - vec2(0.5, 0.5);
	vec2 coord = v_TexCoord - pos * u_RelSize;

	vec3 A = P(-1.,-1.);
	vec3 B = P( 0.,-1.);
	vec3 C = P( 1.,-1.);
	vec3 D = P(-1., 0.);
	vec3 E = P( 0., 0.);
	vec3 F = P( 1., 0.);
	vec3 G = P(-1., 1.);
	vec3 H = P( 0., 1.);
	vec3 I = P( 1., 1.);

	// blend_res Mapping: x|y|
	//                      w|z|
	ivec4 blend_res = ivec4(0, 0, 0, 0);

	// Preprocess corners
	// Pixel Tap Mapping: -|-|-|-|-
	//                    -|-|B|C|-
	//                    -|D|E|F|x
	//                    -|G|H|I|x
	//                    -|-|x|x|-
	if (!((eq(E,F) && eq(H,I)) || (eq(E,H) && eq(F,I))))
	{
		float dist_hf = DistCalc(G, E) + DistCalc(E, C) + DistCalc(P(0,2), I) + DistCalc(I, P(2.,0.)) + (4.0 * DistCalc(H, F));
		float dist_ei = DistCalc(D, H) + DistCalc(H, P(1,2)) + DistCalc(B, F) + DistCalc(F, P(2.,1.)) + (4.0 * DistCalc(E, I));
		bool dom_grad = (3.6 * dist_hf) < dist_ei;
		blend_res.z = ((dist_hf < dist_ei) && neq(E,F) && neq(E,H)) ? ((dom_grad) ? 2 : 1) : 0;
	}

	// Pixel Tap Mapping: -|-|-|-|-
	//                    -|A|B|-|-
	//                    x|D|E|F|-
	//                    x|G|H|I|-
	//                    -|x|x|-|-
	if (!((eq(D,E) && eq(G,H)) || (eq(D,G) && eq(E,H))))
	{
		float dist_ge = DistCalc(P(-2.,1.), D) + DistCalc(D, B) + DistCalc(P(-1.,2.), H) + DistCalc(H, F) + (4.0 * DistCalc(G, E));
		float dist_dh = DistCalc(P(-2.,0.), G) + DistCalc(G, P(0.,2.)) + DistCalc(A, E) + DistCalc(E, I) + (4.0 * DistCalc(D, H));
		bool dom_grad = (3.6 * dist_dh) < dist_ge;
		blend_res.w = ((dist_ge > dist_dh) && neq(E,D) && neq(E,H)) ? ((dom_grad) ? 2 : 1) : 0;
	}

	// Pixel Tap Mapping: -|-|x|x|-
	//                    -|A|B|C|x
	//                    -|D|E|F|x
	//                    -|-|H|I|-
	//                    -|-|-|-|-
	if (!((eq(B,C) && eq(E,F)) || (eq(B,E) && eq(C,F))))
	{
		float dist_ec = DistCalc(D, B) + DistCalc(B, P(1.,-2.)) + DistCalc(H, F) + DistCalc(F, P(2.,-1.)) + (4.0 * DistCalc(E, C));
		float dist_bf = DistCalc(A, E) + DistCalc(E, I) + DistCalc(P(0.,-2.), C) + DistCalc(C, P(2.,0.)) + (4.0 * DistCalc(B, F));
		bool dom_grad = (3.6 * dist_bf) < dist_ec;
		blend_res.y = ((dist_ec > dist_bf) && neq(E,B) && neq(E,F)) ? ((dom_grad) ? 2 : 1) : 0;
	}

	// Pixel Tap Mapping: -|x|x|-|-
	//                    x|A|B|C|-
	//                    x|D|E|F|-
	//                    -|G|H|-|-
	//                    -|-|-|-|-
	if (!((eq(A,B) && eq(D,E)) || (eq(A,D) && eq(B,E))))
	{
		float dist_db = DistCalc(P(-2.,0.), A) + DistCalc(A, P(0.,-2.)) + DistCalc(G, E) + DistCalc(E, C) + (4.0 * DistCalc(D, B));
		float dist_ae = DistCalc(P(-2.,-1.), D) + DistCalc(D, H) + DistCalc(P(-1.,-2.), B) + DistCalc(B, F) + (4.0 * DistCalc(A, E));
		bool dom_grad = (3.6 * dist_db) < dist_ae;
		blend_res.x = ((dist_db < dist_ae) && neq(E,D) && neq(E,B)) ? ((dom_grad) ? 2 : 1) : 0;
	}

	vec3 res = E;

	// Pixel Tap Mapping: -|-|-|-|-
	//                    -|-|B|C|-
	//                    -|D|E|F|x
	//                    -|G|H|I|x
	//                    -|-|x|x|-
	if(blend_res.z != 0)
	{
		float dist_fg = DistCalc(F, G);
		float dist_hc = DistCalc(H, C);
		bool line_blend = (blend_res.z == 2 ||
						!((blend_res.y != 0 && !IsPixEq(E, G)) || (blend_res.w != 0 && !IsPixEq(E, C)) ||
						(IsPixEq(G, H) && IsPixEq(H, I) && IsPixEq(I, F) && IsPixEq(F, C) && !IsPixEq(E, I))));

		vec2 origin = vec2(0.0, 1.0 / sqrt(2.0));
		vec2 dir = vec2(1.0, -1.0);

		if(line_blend)
		{
			bool curve_line = (2.2 * dist_fg <= dist_hc) && neq(E,G) && neq(D,G);
			bool steep_line = (2.2 * dist_hc <= dist_fg) && neq(E,C) && neq(B,C);
			origin = curve_line ? vec2(0.0, 0.25) : vec2(0.0, 0.5);
			dir.x += curve_line ? 1.0: 0.0;
			dir.y -= steep_line ? 1.0: 0.0;
		}

		vec3 blend_pix = mix(H, F, step(DistCalc(E, F), DistCalc(E, H)));
		res = mix(res, blend_pix, GetLeftRatio(pos, origin, dir, scale));
	}

	// Pixel Tap Mapping: -|-|-|-|-
	//                    -|A|B|-|-
	//                    x|D|E|F|-
	//                    x|G|H|I|-
	//                    -|x|x|-|-
	if(blend_res.w != 0)
	{
		float dist_ha = DistCalc(H, A);
		float dist_di = DistCalc(D, I);
		bool line_blend = (blend_res.w == 2 ||
						!((blend_res.z != 0 && !IsPixEq(E, A)) || (blend_res.x != 0 && !IsPixEq(E, I)) ||
						(IsPixEq(A, D) && IsPixEq(D, G) && IsPixEq(G, H) && IsPixEq(H, I) && !IsPixEq(E, G))));

		vec2 origin = vec2(-1.0 / sqrt(2.0), 0.0);
		vec2 dir = vec2(1.0, 1.0);

		if(line_blend)
		{
			bool curve_line = (2.2 * dist_ha <= dist_di) && neq(E,A) && neq(B,A);
			bool steep_line = (2.2 * dist_di <= dist_ha) && neq(E,I) && neq(F,I);
			origin = curve_line ? vec2(-0.25, 0.0) : vec2(-0.5, 0.0);
			dir.y += curve_line ? 1.0: 0.0;
			dir.x += steep_line ? 1.0: 0.0;
		}

		vec3 blend_pix = mix(H, D, step(DistCalc(E, D), DistCalc(E, H)));
		res = mix(res, blend_pix, GetLeftRatio(pos, origin, dir, scale));
	}

	// Pixel Tap Mapping: -|-|x|x|-
	//                    -|A|B|C|x
	//                    -|D|E|F|x
	//                    -|-|H|I|-
	//                    -|-|-|-|-
	if(blend_res.y != 0)
	{
		float dist_bi = DistCalc(B, I);
		float dist_fa = DistCalc(F, A);
		bool line_blend = (blend_res.y == 2 ||
						!((blend_res.x != 0 && !IsPixEq(E, I)) || (blend_res.z != 0 && !IsPixEq(E, A)) ||
						(IsPixEq(I, F) && IsPixEq(F, C) && IsPixEq(C, B) && IsPixEq(B, A) && !IsPixEq(E, C))));

		vec2 origin = vec2(1.0 / sqrt(2.0), 0.0);
		vec2 dir = vec2(-1.0, -1.0);

		if(line_blend)
		{
			bool curve_line = (2.2 * dist_bi <= dist_fa) && neq(E,I) && neq(H,I);
			bool steep_line = (2.2 * dist_fa <= dist_bi) && neq(E,A) && neq(D,A);
			origin = curve_line ? vec2(0.25, 0.0) : vec2(0.5, 0.0);
			dir.y -= curve_line ? 1.0: 0.0;
			dir.x -= steep_line ? 1.0: 0.0;
		}

		vec3 blend_pix = mix(F, B, step(DistCalc(E, B), DistCalc(E, F)));
		res = mix(res, blend_pix, GetLeftRatio(pos, origin, dir, scale));
	}

	// Pixel Tap Mapping: -|x|x|-|-
	//                    x|A|B|C|-
	//                    x|D|E|F|-
	//                    -|G|H|-|-
	//                    -|-|-|-|-
	if(blend_res.x != 0)
	{
		float dist_dc = DistCalc(D, C);
		float dist_bg = DistCalc(B, G);
		bool line_blend = (blend_res.x == 2 ||
						!((blend_res.w != 0 && !IsPixEq(E, C)) || (blend_res.y != 0 && !IsPixEq(E, G)) ||
						(IsPixEq(C, B) && IsPixEq(B, A) && IsPixEq(A, D) && IsPixEq(D, G) && !IsPixEq(E, A))));

		vec2 origin = vec2(0.0, -1.0 / sqrt(2.0));
		vec2 dir = vec2(-1.0, 1.0);

		if(line_blend)
		{
			bool curve_line = (2.2 * dist_dc <= dist_bg) && neq(E,C) && neq(F,C);
			bool steep_line = (2.2 * dist_bg <= dist_dc) && neq(E,G) && neq(H,G);
			origin = curve_line ? vec2(0.0, -0.25) : vec2(0.0, -0.5);
			dir.x -= curve_line ? 1.0: 0.0;
			dir.y += steep_line ? 1.0: 0.0;
		}

		vec3 blend_pix = mix(D, B, step(DistCalc(E, B), DistCalc(E, D)));
		res = mix(res, blend_pix, GetLeftRatio(pos, origin, dir, scale));
	}

	FragColor = vec4(res, 1.0);
}

#endif
