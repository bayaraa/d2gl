/*
	Hyllian's 2xBR v3.8c+ReverseAA (squared) Shader - Dithering preserved
	Copyright (C) 2011/2012 Hyllian/Jararaca - sergiogdb@gmail.com
	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.
	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.
	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/
/*
	ReverseAA part of the code

	Copyright (c) 2012, Christoph Feck <christoph@maxiom.de>
	All Rights reserved.

	Redistribution and use in source and binary forms, with or without
	modification, are permitted provided that the following conditions are met:

	* Redistributions of source code must retain the above copyright notice,
		this list of conditions and the following disclaimer.

	* Redistributions in binary form must reproduce the above copyright
		notice, this list of conditions and the following disclaimer in the
		documentation and/or other materials provided with the distribution.

	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
	AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
	IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
	ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
	LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
	CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
	SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
	INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
	CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
	ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
	POSSIBILITY OF SUCH DAMAGE.
*/

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

#define mul(a,b) (b*a)

const float coef = 2.0;
const vec4 eq_threshold = vec4(15.0, 15.0, 15.0, 15.0);
const float y_weight = 48.0;
const float u_weight = 7.0;
const float v_weight = 6.0;
const mat3x3 yuv = mat3x3(0.299, 0.587, 0.114, -0.169, -0.331, 0.499, 0.499, -0.418, -0.0813);
const mat3x3 yuv_weighted = mat3x3(y_weight*yuv[0], u_weight*yuv[1], v_weight*yuv[2]);
const vec4 delta = vec4(0.5, 0.5, 0.5, 0.5);
const float sharpness = 0.65;

float lum(vec3 A, vec3 B)
{
	return abs(dot(A-B, yuv_weighted[0]));
}

vec4 df(vec4 A, vec4 B)
{
	return vec4(abs(A-B));
}

bvec4 eq(vec4 A, vec4 B)
{
	return lessThan(df(A, B), vec4(15.0, 15.0, 15.0, 15.0));
}

bvec4 eq2(vec4 A, vec4 B)
{
	return lessThan(df(A, B), vec4(2.0, 2.0, 2.0, 2.0));
}

bvec4 eq3(vec4 A, vec4 B)
{
	return lessThan(df(A, B), vec4(5.0, 5.0, 5.0, 5.0));
}

vec4 weighted_distance(vec4 a, vec4 b, vec4 c, vec4 d, vec4 e, vec4 f, vec4 g, vec4 h)
{
	return (df(a,b) + df(a,c) + df(d,e) + df(d,f) + 4.0*df(g,h));
}

bvec4 and(bvec4 A, bvec4 B)
{
	return bvec4(A.x && B.x, A.y && B.y, A.z && B.z, A.w && B.w);
}

bvec4 or(bvec4 A, bvec4 B)
{
	return bvec4(A.x || B.x, A.y || B.y, A.z || B.z, A.w || B.w);
}

#define P(x) texture(u_Texture, x).rgb

void main()
{
	vec2 _tc = v_TexCoord;
	vec2 _rs = u_RelSize;

	//    A1 B1 C1
	// A0  A  B  C C4
	// D0  D  E  F F4
	// G0  G  H  I I4
	//    G5 H5 I5

	vec4 t1 = _tc.xxxy + vec4(      -_rs.x,     0, _rs.x,-2.0 * _rs.y); // A1 B1 C1
	vec4 t2 = _tc.xxxy + vec4(      -_rs.x,     0, _rs.x,      -_rs.y); //  A  B  C
	vec4 t3 = _tc.xxxy + vec4(      -_rs.x,     0, _rs.x,           0); //  D  E  F
	vec4 t4 = _tc.xxxy + vec4(      -_rs.x,     0, _rs.x,       _rs.y); //  G  H  I
	vec4 t5 = _tc.xxxy + vec4(      -_rs.x,     0, _rs.x, 2.0 * _rs.y); // G5 H5 I5
	vec4 t6 = _tc.xyyy + vec4(-2.0 * _rs.x,-_rs.y,     0,       _rs.y); // A0 D0 G0
	vec4 t7 = _tc.xyyy + vec4( 2.0 * _rs.x,-_rs.y,     0,       _rs.y); // C4 F4 I4

	bvec4 edr, edr_left, edr_up, px; // px = pixel, edr = edge detection rule
	bvec4 interp_restriction_lv1, interp_restriction_lv2_left, interp_restriction_lv2_up;
	bvec4 nc, nc30, nc60, nc45; // new_color
	vec4 fx, fx_left, fx_up, final_fx; // inequations of straight lines.

	vec2 fp = fract(_tc * u_TexSize);

	vec3 A1 = P(t1.xw);
	vec3 B1 = P(t1.yw);
	vec3 C1 = P(t1.zw);

	vec3 A  = P(t2.xw);
	vec3 B  = P(t2.yw);
	vec3 C  = P(t2.zw);

	vec3 D  = P(t3.xw);
	vec3 E  = P(t3.yw);
	vec3 F  = P(t3.zw);

	vec3 G  = P(t4.xw);
	vec3 H  = P(t4.yw);
	vec3 I  = P(t4.zw);

	vec3 G5 = P(t5.xw);
	vec3 H5 = P(t5.yw);
	vec3 I5 = P(t5.zw);

	vec3 A0 = P(t6.xy);
	vec3 D0 = P(t6.xz);
	vec3 G0 = P(t6.xw);

	vec3 C4 = P(t7.xy);
	vec3 F4 = P(t7.xz);
	vec3 I4 = P(t7.xw);

	vec4 b = mul(mat4x3(B, D, H, F), yuv_weighted[0]);
	vec4 c = mul(mat4x3(C, A, G, I), yuv_weighted[0]);
	vec4 e = mul(mat4x3(E, E, E, E), yuv_weighted[0]);
	vec4 a = c.yzwx;
	vec4 d = b.yzwx;
	vec4 f = b.wxyz;
	vec4 g = c.zwxy;
	vec4 h = b.zwxy;
	vec4 i = c.wxyz;

	vec4 i4 = mul(mat4x3(I4, C1, A0, G5), yuv_weighted[0]);
	vec4 i5 = mul(mat4x3(I5, C4, A1, G0), yuv_weighted[0]);
	vec4 h5 = mul(mat4x3(H5, F4, B1, D0), yuv_weighted[0]);
	vec4 f4 = h5.yzwx;

	vec4 Ao = vec4( 1.0, -1.0, -1.0, 1.0 );
	vec4 Bo = vec4( 1.0,  1.0, -1.0,-1.0 );
	vec4 Co = vec4( 1.5,  0.5, -0.5, 0.5 );
	vec4 Ax = vec4( 1.0, -1.0, -1.0, 1.0 );
	vec4 Bx = vec4( 0.5,  2.0, -0.5,-2.0 );
	vec4 Cx = vec4( 1.0,  1.0, -0.5, 0.0 );
	vec4 Ay = vec4( 1.0, -1.0, -1.0, 1.0 );
	vec4 By = vec4( 2.0,  0.5, -2.0,-0.5 );
	vec4 Cy = vec4( 2.0,  0.0, -1.0, 0.5 );

	// These inequations define the line below which interpolation occurs.
	fx      = (Ao*fp.y+Bo*fp.x);
	fx_left = (Ax*fp.y+Bx*fp.x);
	fx_up   = (Ay*fp.y+By*fp.x);

	//this is the only way I can keep these comparisons straight '-_-
	bvec4 block1 = and(not(eq(h,h5)) , not(eq(h,i5)));
	bvec4 block2 = and(not(eq(f,f4)) , not(eq(f,i4)));
	bvec4 block3 = and(not(eq(h, d)) , not(eq(h, g)));
	bvec4 block4 = and(not(eq(f, b)) , not(eq(f, c)));
	bvec4 block5 = and(eq(e, i) , or(block2 , block1));

	bvec4 block_comp = or(block4 , or(block3 , or(block5 , or(eq(e,g) , eq(e,c)))));

	interp_restriction_lv1      = and(notEqual(e,f) , and(notEqual(e,h) , block_comp));
	interp_restriction_lv2_left = and(notEqual(e,g) , notEqual(d,g));
	interp_restriction_lv2_up   = and(notEqual(e,c) , notEqual(b,c));

	vec4 fx45 = smoothstep(Co - delta, Co + delta, fx);
	vec4 fx30 = smoothstep(Cx - delta, Cx + delta, fx_left);
	vec4 fx60 = smoothstep(Cy - delta, Cy + delta, fx_up);

	edr      = and(lessThan((weighted_distance(e, c, g, i, h5, f4, h, f) + 3.5) , weighted_distance(h, d, i5, f, i4, b, e, i)) , interp_restriction_lv1);
	edr_left = and(lessThanEqual((coef*df(f,g)), df(h,c)), interp_restriction_lv2_left);
	edr_up   = and(greaterThanEqual(df(f,g), (coef*df(h,c))), interp_restriction_lv2_up);

	nc45 = and(edr,               bvec4(fx45));
	nc30 = and(edr, and(edr_left, bvec4(fx30)));
	nc60 = and(edr, and(edr_up  , bvec4(fx60)));

	px = lessThanEqual(df(e,f) , df(e,h));

	vec3 res = E;

	vec3 n1, n2, n3, n4, s, aa, bb, cc, dd;

	n1 = B1; n2 = B; s = E; n3 = H; n4 = H5;
	aa = n2-n1; bb = s-n2; cc = n3-s; dd = n4-n3;

	vec3 t = (7. * (bb + cc) - 3. * (aa + dd)) / 16.;

	vec3 m;
	m.x = (s.x < 0.5) ? 2.*s.x : 2.*(1.0-s.x);
	m.y = (s.y < 0.5) ? 2.*s.y : 2.*(1.0-s.y);
	m.z = (s.z < 0.5) ? 2.*s.z : 2.*(1.0-s.z);

	m = min(m, sharpness*abs(bb));
	m = min(m, sharpness*abs(cc));

	t = clamp(t, -m, m);

	vec3 s1 = (2.*fp.y-1.)*t + s;

	n1 = D0; n2 = D; s = s1; n3 = F; n4 = F4;
	aa = n2-n1; bb = s-n2; cc = n3-s; dd = n4-n3;

	t = (7. * (bb + cc) - 3. * (aa + dd)) / 16.;

	m.x = (s.x < 0.5) ? 2.*s.x : 2.*(1.0-s.x);
	m.y = (s.y < 0.5) ? 2.*s.y : 2.*(1.0-s.y);
	m.z = (s.z < 0.5) ? 2.*s.z : 2.*(1.0-s.z);

	m = min(m, sharpness*abs(bb));
	m = min(m, sharpness*abs(cc));

	t = clamp(t, -m, m);

	vec3 s0 = (2.*fp.x-1.)*t + s;

	nc = or(nc30 , or(nc60 , nc45));

	float blend = 0.0;
	vec3 pix;

	vec4 r1 = mix(e, f, edr);

	bool yeseq3=false;
	bvec4 yes;

	if (all(eq3(r1,e)))
	{
		yeseq3 = true;
		pix = res = s0;
	}
	else
	{
		pix = res = E;
	}

	yes = and(bvec4(yeseq3) , eq2(e, mix(f, h, px)));

	vec4 final45 = vec4(nc45) * fx45;
	vec4 final30 = vec4(nc30) * fx30;
	vec4 final60 = vec4(nc60) * fx60;

	vec4 maximo = max(max(final30, final60), final45);

		 if (nc.x) { pix = px.x ? F : H; blend = maximo.x; if (yes.x) pix = res = s0; else res=E; }
	else if (nc.y) { pix = px.y ? B : F; blend = maximo.y; if (yes.y) pix = res = s0; else res=E; }
	else if (nc.z) { pix = px.z ? D : B; blend = maximo.z; if (yes.z) pix = res = s0; else res=E; }
	else if (nc.w) { pix = px.w ? H : D; blend = maximo.w; if (yes.w) pix = res = s0; else res=E; }

	FragColor = vec4(clamp(mix(res, pix, blend), 0.0, 1.0), 1.0);
}

#endif
