/*
	Hyllian's xBR-lv2-noblend Shader
   
	Copyright (C) 2011-2016 Hyllian - sergiogdb@gmail.com
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
	Incorporates some of the ideas from SABR shader. Thanks to Joshua Street.
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

#define CoefLuma vec3(0.2126, 0.7152, 0.0722)

vec4 df(vec4 A, vec4 B)
{
	return vec4(abs(A - B));
}

vec4 diff(vec4 A, vec4 B)
{
	return vec4(notEqual(A, B));
}

vec4 wd(vec4 a, vec4 b, vec4 c, vec4 d, vec4 e, vec4 f, vec4 g, vec4 h)
{
	return (df(a, b) + df(a, c) + df(d, e) + df(d, f) + 4.0 * df(g, h));
}

#define P(x) texture(u_Texture, x).rgb

void main()
{
	vec2 _tc = v_TexCoord;
	vec2 _rs = u_RelSize;

	vec4 t1 = _tc.xxxy + vec4(      -_rs.x,    0.0, _rs.x, -2.0 * _rs.y); // A1 B1 C1
	vec4 t2 = _tc.xxxy + vec4(      -_rs.x,    0.0, _rs.x,       -_rs.y); //  A  B  C
	vec4 t3 = _tc.xxxy + vec4(      -_rs.x,    0.0, _rs.x,          0.0); //  D  E  F
	vec4 t4 = _tc.xxxy + vec4(      -_rs.x,    0.0, _rs.x,        _rs.y); //  G  H  I
	vec4 t5 = _tc.xxxy + vec4(      -_rs.x,    0.0, _rs.x,  2.0 * _rs.y); // G5 H5 I5
	vec4 t6 = _tc.xyyy + vec4(-2.0 * _rs.x, -_rs.y,   0.0,        _rs.y); // A0 D0 G0
	vec4 t7 = _tc.xyyy + vec4( 2.0 * _rs.x, -_rs.y,   0.0,        _rs.y); // C4 F4 I4

	vec4 edri, edr, edr_l, edr_u;
	vec4 irlv1, irlv2l, irlv2u, block_3d;
	bvec4 nc, px;
	vec4 fx, fx_l, fx_u;

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

	vec4 b = CoefLuma * mat4x3(B, D, H, F);
	vec4 c = CoefLuma * mat4x3(C, A, G, I);
	vec4 e = CoefLuma * mat4x3(E, E, E, E);
	vec4 d = b.yzwx;
	vec4 f = b.wxyz;
	vec4 g = c.zwxy;
	vec4 h = b.zwxy;
	vec4 i = c.wxyz;
	
	vec4 i4 = CoefLuma * mat4x3(I4, C1, A0, G5);
	vec4 i5 = CoefLuma * mat4x3(I5, C4, A1, G0);
	vec4 h5 = CoefLuma * mat4x3(H5, F4, B1, D0);
	vec4 f4 = h5.yzwx;
	
	vec4 Ao = vec4(1.0, -1.0, -1.0,  1.0);
	vec4 Bo = vec4(1.0,  1.0, -1.0, -1.0);
	vec4 Co = vec4(1.5,  0.5, -0.5,  0.5);
	vec4 Ax = vec4(1.0, -1.0, -1.0,  1.0);
	vec4 Bx = vec4(0.5,  2.0, -0.5, -2.0);
	vec4 Cx = vec4(1.0,  1.0, -0.5,  0.0);
	vec4 Ay = vec4(1.0, -1.0, -1.0,  1.0);
	vec4 By = vec4(2.0,  0.5, -2.0, -0.5);
	vec4 Cy = vec4(2.0,  0.0, -1.0,  0.5);

	fx   = vec4(greaterThan(Ao * fp.y + Bo * fp.x, Co)); 
	fx_l = vec4(greaterThan(Ax * fp.y + Bx * fp.x, Cx));
	fx_u = vec4(greaterThan(Ay * fp.y + By * fp.x, Cy));

	irlv1 = diff(e, f) * diff(e, h);

	irlv2l = diff(e, g) * diff(d, g);
	irlv2u = diff(e, c) * diff(b, c);

	vec4 wd1 = wd(e, c,  g, i, h5, f4, h, f);
	vec4 wd2 = wd(h, d, i5, f, i4,  b, e, i);

	edri  = step(wd1, wd2) * irlv1;
	edr   = step(wd1 + vec4(0.1, 0.1, 0.1, 0.1), wd2) * step(vec4(0.5, 0.5, 0.5, 0.5), irlv1);
	edr_l = step(2.0 * df(f, g), df(h, c)) * irlv2l * edr;
	edr_u = step(2.0 * df(h, c), df(f, g)) * irlv2u * edr;

	nc = bvec4(edr * (fx + edr_l * (fx_l)) + edr_u * fx_u);
	
	px = lessThanEqual(df(e, f), df(e, h));

	vec3 res1 = nc.x ? px.x ? F : H : nc.y ? px.y ? B : F : nc.z ? px.z ? D : B : E;
	vec3 res2 = nc.w ? px.w ? H : D : nc.z ? px.z ? D : B : nc.y ? px.y ? B : F : E;

	vec2 df12 = abs(CoefLuma * mat2x3(res1, res2) - e.xy);
	
	FragColor = vec4(mix(res1, res2, step(df12.x, df12.y)), 1.0);
}

#endif
