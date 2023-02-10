#version 330

/*
 	FXAA @license
 	Copyright (c) 2011 NVIDIA Corporation. All rights reserved.
 	
 	TO  THE MAXIMUM  EXTENT PERMITTED  BY APPLICABLE  LAW, THIS SOFTWARE  IS PROVIDED
 	*AS IS*  AND NVIDIA AND  ITS SUPPLIERS DISCLAIM  ALL WARRANTIES,  EITHER  EXPRESS
 	OR IMPLIED, INCLUDING, BUT NOT LIMITED  TO, NONINFRINGEMENT,IMPLIED WARRANTIES OF
 	MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  IN NO EVENT SHALL  NVIDIA 
 	OR ITS SUPPLIERS BE  LIABLE  FOR  ANY  DIRECT, SPECIAL,  INCIDENTAL,  INDIRECT,  OR  
 	CONSEQUENTIAL DAMAGES WHATSOEVER (INCLUDING, WITHOUT LIMITATION,  DAMAGES FOR LOSS 
 	OF BUSINESS PROFITS, BUSINESS INTERRUPTION, LOSS OF BUSINESS INFORMATION, OR ANY 
 	OTHER PECUNIARY LOSS) ARISING OUT OF THE  USE OF OR INABILITY  TO USE THIS SOFTWARE, 
 	EVEN IF NVIDIA HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
*/
/*
	LumaSharpen
	by Christian Cann Schuldt Jensen ~ CeeJay.dk
	
	It blurs the original pixel with the surrounding pixels and then subtracts this blur to sharpen the image.
	It does this in luma to avoid color artifacts and allows limiting the maximum sharpning to avoid or lessen halo artifacts.
	This is similar to using Unsharp Mask in Photoshop.
*/

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

layout(std140) uniform ubo_Metrics {
	float u_SharpStrength;
	float u_SharpClamp;
	float u_Radius;
	float pad1;
	vec2 u_RelSize;
};

uniform sampler2D u_Textures[2];

in vec2 v_TexCoord;
flat in ivec4 v_Flags;

float FxaaLuma(vec3 rgb)
{
	return rgb.y * (0.587 / 0.299) + rgb.x;
}

#define P(x) texture(u_Textures[v_Flags.y], x).rgb
#define e_t_div 16.0
#define s_steps 16

vec3 FxaaPass(vec3 rgb)
{
	vec2 pos = v_TexCoord;
	vec2 rs = u_RelSize;

	vec3 rgbN = P(pos + vec2(0.0, -rs.y));
	vec3 rgbW = P(pos + vec2(-rs.x, 0.0));
	vec3 rgbM = rgb;
	vec3 rgbE = P(pos + vec2( rs.x, 0.0));
	vec3 rgbS = P(pos + vec2(0.0,  rs.y));
	
	float lumaN = FxaaLuma(rgbN);
	float lumaW = FxaaLuma(rgbW);
	float lumaM = FxaaLuma(rgbM);
	float lumaE = FxaaLuma(rgbE);
	float lumaS = FxaaLuma(rgbS);
	float rangeMin = min(lumaM, min(min(lumaN, lumaW), min(lumaS, lumaE)));
	float rangeMax = max(lumaM, max(max(lumaN, lumaW), max(lumaS, lumaE)));
	
	float range = rangeMax - rangeMin;
	if(range < max(1.0 / e_t_div, rangeMax * (1.0 / 8.0)))
		return rgbM;
	
	vec3 rgbL = rgbN + rgbW + rgbM + rgbE + rgbS;
	
	float lumaL = (lumaN + lumaW + lumaE + lumaS) * 0.25;
	float rangeL = abs(lumaL - lumaM);
	float blendL = max(0.0, (rangeL / range) - 0.25) * (1.0 / 0.75); 
	blendL = min(3.0 / 4.0, blendL);

	vec3 rgbNW = P(pos + vec2(-rs.x, -rs.y));
	vec3 rgbNE = P(pos + vec2( rs.x, -rs.y));
	vec3 rgbSW = P(pos + vec2(-rs.x,  rs.y));
	vec3 rgbSE = P(pos + vec2( rs.x,  rs.y));

	rgbL += (rgbNW + rgbNE + rgbSW + rgbSE);
	rgbL *= vec3(1.0 / 9.0);
	
	float lumaNW = FxaaLuma(rgbNW);
	float lumaNE = FxaaLuma(rgbNE);
	float lumaSW = FxaaLuma(rgbSW);
	float lumaSE = FxaaLuma(rgbSE);
	
	float edgeVert = 
		abs((0.25 * lumaNW) + (-0.5 * lumaN) + (0.25 * lumaNE)) +
		abs((0.50 * lumaW ) + (-1.0 * lumaM) + (0.50 * lumaE )) +
		abs((0.25 * lumaSW) + (-0.5 * lumaS) + (0.25 * lumaSE));
	float edgeHorz = 
		abs((0.25 * lumaNW) + (-0.5 * lumaW) + (0.25 * lumaSW)) +
		abs((0.50 * lumaN ) + (-1.0 * lumaM) + (0.50 * lumaS )) +
		abs((0.25 * lumaNE) + (-0.5 * lumaE) + (0.25 * lumaSE));
		
	bool horzSpan = edgeHorz >= edgeVert;
	float lengthSign = horzSpan ? -rs.y : -rs.x;
	
	if(!horzSpan)
	{
		lumaN = lumaW;
		lumaS = lumaE;
	}
	
	float gradientN = abs(lumaN - lumaM);
	float gradientS = abs(lumaS - lumaM);
	lumaN = (lumaN + lumaM) * 0.5;
	lumaS = (lumaS + lumaM) * 0.5;
	
	if (gradientN < gradientS)
	{
		lumaN = lumaS;
		lumaN = lumaS;
		gradientN = gradientS;
		lengthSign *= -1.0;
	}
	
	vec2 posN;
	posN.x = pos.x + (horzSpan ? 0.0 : lengthSign * 0.5);
	posN.y = pos.y + (horzSpan ? lengthSign * 0.5 : 0.0);
	
	gradientN *= (1.0 / 4.0);
	
	vec2 posP = posN;
	vec2 offNP = horzSpan ? vec2(rs.x, 0.0) : vec2(0.0, rs.y); 
	float lumaEndN = lumaN;
	float lumaEndP = lumaN;
	bool doneN = false;
	bool doneP = false;
	posN += offNP * vec2(-1.0, -1.0);
	posP += offNP * vec2( 1.0,  1.0);
	
	for (int i = 0; i < s_steps; i++)
	{
		if(!doneN)
			lumaEndN = FxaaLuma(P(posN.xy));

		if(!doneP)
			lumaEndP = FxaaLuma(P(posP.xy));
		
		doneN = doneN || (abs(lumaEndN - lumaN) >= gradientN);
		doneP = doneP || (abs(lumaEndP - lumaN) >= gradientN);
		
		if(doneN && doneP)
			break;

		if(!doneN)
			posN -= offNP;

		if(!doneP)
			posP += offNP;
	}
	
	float dstN = horzSpan ? pos.x - posN.x : pos.y - posN.y;
	float dstP = horzSpan ? posP.x - pos.x : posP.y - pos.y;
	bool directionN = dstN < dstP;
	lumaEndN = directionN ? lumaEndN : lumaEndP;
	
	if(((lumaM - lumaN) < 0.0) == ((lumaEndN - lumaN) < 0.0))
		lengthSign = 0.0;
 
	float spanLength = (dstP + dstN);
	dstN = directionN ? dstN : dstP;
	float subPixelOffset = (0.5 + (dstN * (-1.0 / spanLength))) * lengthSign;
	vec3 rgbF = P(vec2(pos.x + (horzSpan ? 0.0 : subPixelOffset), pos.y + (horzSpan ? subPixelOffset : 0.0)));

	return (vec3(-blendL) * rgbF) + ((rgbL * vec3(blendL)) + rgbF);
}

#define CoefLuma vec3(0.2126, 0.7152, 0.0722)

vec3 LumaSharpen(vec3 rgb)
{
	vec3 p1 = texture(u_Textures[v_Flags.y], v_TexCoord + u_RelSize * u_Radius).rgb;
	vec3 p2 = texture(u_Textures[v_Flags.y], v_TexCoord - u_RelSize * u_Radius).rgb;
	vec3 color = (p1 + p2) / 2.0;

	vec3 sharp = rgb - color;
	vec3 sharp_str = (CoefLuma * u_SharpStrength) * 1.5;
	vec4 sharp_clamp = vec4(sharp_str * (0.5 / u_SharpClamp), 0.5);

	float sharp_luma = (u_SharpClamp * 2.0) * clamp(dot(vec4(sharp, 1.0), sharp_clamp), 0.0, 1.0) - u_SharpClamp;

	return clamp(rgb + sharp_luma, 0.0, 1.0);
}

void main()
{
	vec3 color = texture(u_Textures[v_Flags.y], v_TexCoord).rgb;

	if (v_Flags.x == 0)
		FragColor = vec4(LumaSharpen(color), 1.0);
	else
		FragColor = vec4(FxaaPass(color), 1.0);
}

#endif