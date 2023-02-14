/*
   Copyright (C) 2006 guest(r) - guest.r@gmail.com

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
   The AdvancedAA shader is well used to:
   - AA 2xscaled gfx. to its 4x absolute size,   
   - AA hi-res "screens" (640x480) to their 2x size or,
   - AA gfx. back to it's original size (looks nice above 640x480, set scaling to 1.0) 
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

vec3 dt = vec3(1.,1.,1.);

void main()
{
	float dx = u_RelSize.x * 0.5;
	float dy = u_RelSize.y * 0.5;
	
	vec4 t1;
	vec4 t2;
	vec4 t3;
	vec4 t4;

	t1.xy = v_TexCoord + vec2(-dx,  0);
	t2.xy = v_TexCoord + vec2( dx,  0);
	t3.xy = v_TexCoord + vec2(  0,-dy);
	t4.xy = v_TexCoord + vec2(  0, dy);
	t1.zw = v_TexCoord + vec2(-dx,-dy);
	t2.zw = v_TexCoord + vec2(-dx, dy);
	t3.zw = v_TexCoord + vec2( dx,-dy);
	t4.zw = v_TexCoord + vec2( dx, dy);

	vec3 c00 = texture(u_Texture, t1.zw).xyz; 
	vec3 c10 = texture(u_Texture, t3.xy).xyz;
	vec3 c20 = texture(u_Texture, t3.zw).xyz;
	vec3 c01 = texture(u_Texture, t1.xy).xyz;
	vec3 c11 = texture(u_Texture, v_TexCoord).xyz;
	vec3 c21 = texture(u_Texture, t2.xy).xyz;
	vec3 c02 = texture(u_Texture, t2.zw).xyz;
	vec3 c12 = texture(u_Texture, t4.xy).xyz;
	vec3 c22 = texture(u_Texture, t4.zw).xyz;

	float d1=dot(abs(c00-c22),dt)+0.0001;
	float d2=dot(abs(c20-c02),dt)+0.0001;
	float hl=dot(abs(c01-c21),dt)+0.0001;
	float vl=dot(abs(c10-c12),dt)+0.0001;
   
	float k1=0.5*(hl+vl);
	float k2=0.5*(d1+d2);

	vec3 t1a=(hl*(c10+c12)+vl*(c01+c21)+k1*c11)/(2.5*(hl+vl));
	vec3 t2a=(d1*(c20+c02)+d2*(c00+c22)+k2*c11)/(2.5*(d1+d2));

	k1=dot(abs(t1a-c11),dt)+0.0001;
	k2=dot(abs(t2a-c11),dt)+0.0001;
   
	FragColor = vec4((k1*t2a+k2*t1a)/(k1+k2), 1.0);
}

#endif
