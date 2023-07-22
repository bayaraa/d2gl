/*
	D2GL: Diablo 2 LoD Glide/DDraw to OpenGL Wrapper.
	Copyright (C) 2023  Bayaraa

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
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

#define P(x, y) texture(x, y).rgb

float FxaaLuma(vec3 rgb)
{
    return rgb.y * (0.587/0.299) + rgb.x;
}

vec3 FxaaLerp3(vec3 a, vec3 b, float amountOfA)
{
    return (vec3(-amountOfA) * b) + ((a * vec3(amountOfA)) + b);
}

vec3 FxaaTexOff(sampler2D tex, vec2 pos, ivec2 off, vec2 rcpFrame) {
    float x = pos.x + float(off.x) * rcpFrame.x;
    float y = pos.y + float(off.y) * rcpFrame.y;
    return P(tex, vec2(x, y));
}

vec3 FxaaPass(sampler2D tex, vec2 pos, vec2 rcpFrame, uint preset)
{
    float FXAA_EDGE_THRESHOLD = (1.0/8.0);
    float FXAA_EDGE_THRESHOLD_MIN = (1.0/16.0);
    float FXAA_SEARCH_THRESHOLD = (1.0/4.0);
    float FXAA_SUBPIX_CAP = (3.0/4.0);
    float FXAA_SUBPIX_TRIM = (1.0/4.0);
    int FXAA_SEARCH_STEPS = 16;
    if (preset == 2u) {
        FXAA_EDGE_THRESHOLD = (1.0/8.0);
        FXAA_EDGE_THRESHOLD_MIN = (1.0/24.0);
        FXAA_SEARCH_THRESHOLD = (1.0/4.0);
        FXAA_SUBPIX_CAP = (3.0/4.0);
        FXAA_SUBPIX_TRIM = (1.0/4.0);
        FXAA_SEARCH_STEPS = 24;
    } else if (preset == 3u) {
        FXAA_EDGE_THRESHOLD = (1.0/8.0);
        FXAA_EDGE_THRESHOLD_MIN = (1.0/24.0);
        FXAA_SEARCH_THRESHOLD = (1.0/4.0);
        FXAA_SUBPIX_CAP = (3.0/4.0);
        FXAA_SUBPIX_TRIM = (1.0/4.0);
        FXAA_SEARCH_STEPS = 32;
    }

    vec3 rgbN = FxaaTexOff(tex, pos.xy, ivec2( 0,-1), rcpFrame);
    vec3 rgbW = FxaaTexOff(tex, pos.xy, ivec2(-1, 0), rcpFrame);
    vec3 rgbM = FxaaTexOff(tex, pos.xy, ivec2( 0, 0), rcpFrame);
    vec3 rgbE = FxaaTexOff(tex, pos.xy, ivec2( 1, 0), rcpFrame);
    vec3 rgbS = FxaaTexOff(tex, pos.xy, ivec2( 0, 1), rcpFrame);
    
    float lumaN = FxaaLuma(rgbN);
    float lumaW = FxaaLuma(rgbW);
    float lumaM = FxaaLuma(rgbM);
    float lumaE = FxaaLuma(rgbE);
    float lumaS = FxaaLuma(rgbS);
    float rangeMin = min(lumaM, min(min(lumaN, lumaW), min(lumaS, lumaE)));
    float rangeMax = max(lumaM, max(max(lumaN, lumaW), max(lumaS, lumaE)));
    
    float range = rangeMax - rangeMin;
    if(range < max(FXAA_EDGE_THRESHOLD_MIN, rangeMax * FXAA_EDGE_THRESHOLD))
        return rgbM;
    
    vec3 rgbL = rgbN + rgbW + rgbM + rgbE + rgbS;
    
    float lumaL = (lumaN + lumaW + lumaE + lumaS) * 0.25;
    float rangeL = abs(lumaL - lumaM);
    float blendL = max(0.0, (rangeL / range) - FXAA_SUBPIX_TRIM) * (1.0/(1.0 - FXAA_SUBPIX_TRIM)); 
    blendL = min(FXAA_SUBPIX_CAP, blendL);
    
    vec3 rgbNW = FxaaTexOff(tex, pos.xy, ivec2(-1,-1), rcpFrame);
    vec3 rgbNE = FxaaTexOff(tex, pos.xy, ivec2( 1,-1), rcpFrame);
    vec3 rgbSW = FxaaTexOff(tex, pos.xy, ivec2(-1, 1), rcpFrame);
    vec3 rgbSE = FxaaTexOff(tex, pos.xy, ivec2( 1, 1), rcpFrame);
    rgbL += (rgbNW + rgbNE + rgbSW + rgbSE);
    rgbL *= vec3(1.0/9.0);
    
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
    float lengthSign = horzSpan ? -rcpFrame.y : -rcpFrame.x;
    
    if(!horzSpan) {
        lumaN = lumaW;
        lumaS = lumaE;
    }
    
    float gradientN = abs(lumaN - lumaM);
    float gradientS = abs(lumaS - lumaM);
    lumaN = (lumaN + lumaM) * 0.5;
    lumaS = (lumaS + lumaM) * 0.5;
    
    if (gradientN < gradientS) {
        lumaN = lumaS;
        lumaN = lumaS;
        gradientN = gradientS;
        lengthSign *= -1.0;
    }
    
    vec2 posN;
    posN.x = pos.x + (horzSpan ? 0.0 : lengthSign * 0.5);
    posN.y = pos.y + (horzSpan ? lengthSign * 0.5 : 0.0);
    
    gradientN *= FXAA_SEARCH_THRESHOLD;
    
    vec2 posP = posN;
    vec2 offNP = horzSpan ? vec2(rcpFrame.x, 0.0) : vec2(0.0, rcpFrame.y); 
    float lumaEndN = lumaN;
    float lumaEndP = lumaN;
    bool doneN = false;
    bool doneP = false;
    posN += offNP * vec2(-1.0, -1.0);
    posP += offNP * vec2( 1.0,  1.0);
    
    for(int i = 0; i < FXAA_SEARCH_STEPS; i++) {
        if(!doneN)
            lumaEndN = FxaaLuma(P(tex, posN.xy));
        if(!doneP)
            lumaEndP = FxaaLuma(P(tex, posP.xy));
        
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
    float subPixelOffset = (0.5 + (dstN * (-1.0/spanLength))) * lengthSign;
    vec3 rgbF = P(tex, vec2(pos.x + (horzSpan ? 0.0 : subPixelOffset), pos.y + (horzSpan ? subPixelOffset : 0.0)));
    return FxaaLerp3(rgbL, rgbF, blendL); 
}

#ifdef VERTEX

layout(location = 0) in vec2 Position;
layout(location = 1) in vec2 TexCoord;
layout(location = 5) in uvec4 Flags;

uniform mat4 u_MVP;

out vec2 v_TexCoord;
flat out uvec4 v_Flags;

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

uniform sampler2D u_Texture0;
uniform sampler2D u_Texture1;

in vec2 v_TexCoord;
flat in uvec4 v_Flags;

#define CoefLuma vec3(0.2126, 0.7152, 0.0722)

vec3 LumaSharpen(sampler2D tex, vec2 tc)
{
	vec3 rgb = P(tex, tc);
	vec3 p1 = P(tex, tc + u_RelSize * u_Radius);
	vec3 p2 = P(tex, tc - u_RelSize * u_Radius);
	vec3 color = (p1 + p2) / 2.0;

	vec3 sharp = rgb - color;
	vec3 sharp_str = (CoefLuma * u_SharpStrength) * 1.5;
	vec4 sharp_clamp = vec4(sharp_str * (0.5 / u_SharpClamp), 0.5);

	float sharp_luma = (u_SharpClamp * 2.0) * clamp(dot(vec4(sharp, 1.0), sharp_clamp), 0.0, 1.0) - u_SharpClamp;

	return clamp(rgb + sharp_luma, 0.0, 1.0);
}

void main()
{
	switch(v_Flags.x) {
		case 0u: FragColor = vec4(LumaSharpen(u_Texture0, v_TexCoord), 1.0); break;
		case 1u: FragColor = vec4(LumaSharpen(u_Texture1, v_TexCoord), 1.0); break;
		case 2u: FragColor = vec4(FxaaPass(u_Texture0, v_TexCoord, u_RelSize, v_Flags.y), 1.0); break;
		case 3u: FragColor = vec4(P(u_Texture0, v_TexCoord), 1.0); break;
	}
}

// =============================================================
#elif COMPUTE
//[
layout(local_size_x = 16, local_size_y = 16) in;
//]
uniform sampler2D u_InTexture;
writeonly uniform image2D u_OutTexture;

uniform uint u_Flag = 0;

void main()
{
	ivec2 position = ivec2(gl_GlobalInvocationID.xy);
	vec2 tex_size = vec2(textureSize(u_InTexture, 0));
	vec2 v_TexCoord = (vec2(position) + vec2(0.5)) / tex_size;

	vec3 color = FxaaPass(u_InTexture, v_TexCoord, vec2(1.0) / tex_size, u_Flag);
	imageStore(u_OutTexture, position, vec4(color, 1.0));
}

#endif
