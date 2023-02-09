#version 330

/*
   4xGLSLHqFilter shader
   
   Copyright (C) 2005 guest(r) - guest.r@gmail.com
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

uniform sampler2D u_Texture;

in vec2 v_TexCoord;

vec2 u_TexSize = vec2(640.0, 480.0);
vec2 u_RelSize = 1.0 / u_TexSize;

#define CoefLuma vec3(0.2126, 0.7152, 0.0722)

float u_Saturation = 1.20; //Saturation 0.0 <=> 1.0
float u_BlackPoint = 0.06; //Levels Black Point >= 0.0
float u_WhitePoint = 1.12; //Levels White Point >= 1.0

vec3 SaturationPass(vec3 _rgb)
{
	return mix(vec3(dot(_rgb, CoefLuma)), _rgb, u_Saturation);
}

vec3 LevelsPass(vec3 _rgb)
{
	return _rgb * u_WhitePoint - (u_BlackPoint * u_WhitePoint);
}

float mx = 1.0; // start smoothing wt.
float k = -1.10; // wt. decrease factor
float max_w = 0.75; // max filter weight
float min_w = 0.03; // min filter weight
float lum_add = 0.33; // affects smoothing
vec3 dt = vec3(1.0);

#define P(x) texture(u_Texture, x).rgb

vec3 Upscale()
{
	vec2 tc = v_TexCoord;
	vec2 ps = u_TexSize;
	
	float x = 0.5 * u_RelSize.x;
	float y = 0.5 * u_RelSize.y;
	vec2 dg1 = vec2( x, y);
	vec2 dg2 = vec2(-x, y);
	vec2 sd1 = dg1 * 0.5;
	vec2 sd2 = dg2 * 0.5;
	vec2 ddx = vec2(x, 0.0);
	vec2 ddy = vec2(0.0, y);
	vec4 t1 = vec4(tc - sd1, tc - ddy);
	vec4 t2 = vec4(tc - sd2, tc + ddx);
	vec4 t3 = vec4(tc + sd1, tc + ddy);
	vec4 t4 = vec4(tc + sd2, tc - ddx);
	vec4 t5 = vec4(tc - dg1, tc - dg2);
	vec4 t6 = vec4(tc + dg1, tc + dg2);

	vec3 c  = P(tc);
	vec3 i1 = P(t1.xy);
	vec3 i2 = P(t2.xy);
	vec3 i3 = P(t3.xy);
	vec3 i4 = P(t4.xy);
	vec3 o1 = P(t5.xy);
	vec3 o3 = P(t6.xy);
	vec3 o2 = P(t5.zw);
	vec3 o4 = P(t6.zw);
	vec3 s1 = P(t1.zw);
	vec3 s2 = P(t2.zw);
	vec3 s3 = P(t3.zw);
	vec3 s4 = P(t4.zw);

	float ko1=dot(abs(o1-c),dt);
	float ko2=dot(abs(o2-c),dt);
	float ko3=dot(abs(o3-c),dt);
	float ko4=dot(abs(o4-c),dt);

	float k1=min(dot(abs(i1-i3),dt),max(ko1,ko3));
	float k2=min(dot(abs(i2-i4),dt),max(ko2,ko4));

	float w1 = k2; if(ko3<ko1) w1*=ko3/ko1;
	float w2 = k1; if(ko4<ko2) w2*=ko4/ko2;
	float w3 = k2; if(ko1<ko3) w3*=ko1/ko3;
	float w4 = k1; if(ko2<ko4) w4*=ko2/ko4;

	c=(w1*o1+w2*o2+w3*o3+w4*o4+0.001*c)/(w1+w2+w3+w4+0.001);
	w1 = k*dot(abs(i1-c)+abs(i3-c),dt)/(0.125*dot(i1+i3,dt)+lum_add);
	w2 = k*dot(abs(i2-c)+abs(i4-c),dt)/(0.125*dot(i2+i4,dt)+lum_add);
	w3 = k*dot(abs(s1-c)+abs(s3-c),dt)/(0.125*dot(s1+s3,dt)+lum_add);
	w4 = k*dot(abs(s2-c)+abs(s4-c),dt)/(0.125*dot(s2+s4,dt)+lum_add);

	w1 = clamp(w1+mx,min_w,max_w);
	w2 = clamp(w2+mx,min_w,max_w);
	w3 = clamp(w3+mx,min_w,max_w);
	w4 = clamp(w4+mx,min_w,max_w);

	return (w1*(i1+i3)+w2*(i2+i4)+w3*(s1+s3)+w4*(s2+s4)+c)/(2.0*(w1+w2+w3+w4)+1.0);
}

void main()
{
	FragColor = vec4(Upscale(), 1.0);
	FragColor.rgb = SaturationPass(FragColor.rgb);
	FragColor.rgb = LevelsPass(FragColor.rgb);
}

#endif