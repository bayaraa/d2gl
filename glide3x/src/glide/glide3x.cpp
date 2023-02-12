#include "pch.h"
#include "wrapper.h"

using namespace d2gl;

#ifdef __cplusplus
extern "C" {
#endif

FX_ENTRY void FX_CALL grBufferClear(GrColor_t color, GrAlpha_t alpha, FxU32 depth)
{
	GlideWrapper->onBufferClear();
}

FX_ENTRY void FX_CALL grBufferSwap(FxU32 swap_interval)
{
	GlideWrapper->onBufferSwap();
}

FX_ENTRY void FX_CALL grDrawPoint(const void* pt)
{
	GlideWrapper->grDrawPoint(pt);
}

FX_ENTRY void FX_CALL grDrawLine(const void* v1, const void* v2)
{
	GlideWrapper->grDrawLine(v1, v2);
}

FX_ENTRY void FX_CALL grDrawVertexArray(FxU32 mode, FxU32 Count, void* pointers)
{
	GlideWrapper->grDrawVertexArray(mode, Count, (void**)pointers);
}

FX_ENTRY void FX_CALL grDrawVertexArrayContiguous(FxU32 mode, FxU32 Count, void* pointers, FxU32 stride)
{
	GlideWrapper->grDrawVertexArrayContiguous(mode, Count, pointers);
}

FX_ENTRY void FX_CALL grAlphaBlendFunction(GrAlphaBlendFnc_t rgb_sf, GrAlphaBlendFnc_t rgb_df, GrAlphaBlendFnc_t alpha_sf, GrAlphaBlendFnc_t alpha_df)
{
	GlideWrapper->grAlphaBlendFunction(rgb_df);
}

FX_ENTRY void FX_CALL grAlphaCombine(GrCombineFunction_t function, GrCombineFactor_t factor, GrCombineLocal_t local, GrCombineOther_t other, FxBool invert)
{
	GlideWrapper->grAlphaCombine(function);
}

FX_ENTRY void FX_CALL grChromakeyMode(GrChromakeyMode_t mode)
{
	GlideWrapper->grChromakeyMode(mode);
}

FX_ENTRY void FX_CALL grColorCombine(GrCombineFunction_t function, GrCombineFactor_t factor, GrCombineLocal_t local, GrCombineOther_t other, FxBool invert)
{
	GlideWrapper->grColorCombine(function);
}

FX_ENTRY void FX_CALL grConstantColorValue(GrColor_t value)
{
	GlideWrapper->grConstantColorValue(value);
}

FX_ENTRY void FX_CALL grLoadGammaTable(FxU32 nentries, FxU32* red, FxU32* green, FxU32* blue)
{
	GlideWrapper->grLoadGammaTable(nentries, red, green, blue);
}

FX_ENTRY void FX_CALL guGammaCorrectionRGB(FxFloat red, FxFloat green, FxFloat blue)
{
	GlideWrapper->guGammaCorrectionRGB(red, green, blue);
}

FX_ENTRY void FX_CALL grTexSource(GrChipID_t tmu, FxU32 startAddress, FxU32 evenOdd, GrTexInfo* info)
{
	GlideWrapper->grTexSource(tmu, startAddress, info);
}

FX_ENTRY void FX_CALL grTexDownloadMipMap(GrChipID_t tmu, FxU32 startAddress, FxU32 evenOdd, GrTexInfo* info)
{
	GlideWrapper->grTexDownloadMipMap(tmu, startAddress, info);
}

FX_ENTRY void FX_CALL grTexDownloadTable(GrTexTable_t type, void* data)
{
	GlideWrapper->grTexDownloadTable(data);
}

FX_ENTRY FxBool FX_CALL grLfbLock(GrLock_t type, GrBuffer_t buffer, GrLfbWriteMode_t writeMode, GrOriginLocation_t origin, FxBool pixelPipeline, GrLfbInfo_t* info)
{
	return GlideWrapper->grLfbLock(writeMode, origin, info);
}

FX_ENTRY FxBool FX_CALL grLfbUnlock(GrLock_t type, GrBuffer_t buffer)
{
	return GlideWrapper->grLfbUnlock();
}

FX_ENTRY GrContext_t FX_CALL grSstWinOpen(FxU32 hWnd, GrScreenResolution_t screen_resolution, GrScreenRefresh_t refresh_rate, GrColorFormat_t color_format, GrOriginLocation_t origin_location, int nColBuffers, int nAuxBuffers)
{
	return Wrapper::grSstWinOpen(hWnd, screen_resolution);
}

FX_ENTRY FxU32 FX_CALL grGet(FxU32 pname, FxU32 plength, FxI32* params)
{
	return Wrapper::grGet(pname, *params);
}

FX_ENTRY FxU32 FX_CALL grTexMaxAddress(GrChipID_t tmu)
{
	return GLIDE_TEX_MEMORY;
}

FX_ENTRY FxU32 FX_CALL grTexMinAddress(GrChipID_t tmu)
{
	return 0;
}

FX_ENTRY const char* FX_CALL grGetString(FxU32 pname)
{
	return Wrapper::grGetString(pname);
}

// Called but unnecessary functions

FX_ENTRY FxBool FX_CALL grSstWinClose(GrContext_t context)
{
	return FXTRUE;
}

FX_ENTRY FxBool FX_CALL grSelectContext(GrContext_t context)
{
	return FXTRUE;
}

FX_ENTRY void FX_CALL grVertexLayout(FxU32 param, FxI32 offset, FxU32 mode)
{
}

FX_ENTRY void FX_CALL grRenderBuffer(GrBuffer_t buffer)
{
}

FX_ENTRY void FX_CALL grErrorSetCallback(GrErrorCallbackFnc_t fnc)
{
}

FX_ENTRY void FX_CALL grFinish()
{
}

FX_ENTRY void FX_CALL grFlush()
{
}

FX_ENTRY void FX_CALL grSstSelect(int which_sst)
{
}

FX_ENTRY void FX_CALL grChromakeyValue(GrColor_t value)
{
}

FX_ENTRY void FX_CALL grColorMask(FxBool rgb, FxBool a)
{
}

FX_ENTRY void FX_CALL grDepthMask(FxBool mask)
{
}

FX_ENTRY void FX_CALL grDitherMode(GrDitherMode_t mode)
{
}

FX_ENTRY void FX_CALL grCoordinateSpace(GrCoordinateSpaceMode_t mode)
{
}

FX_ENTRY void FX_CALL grViewport(FxI32 x, FxI32 y, FxI32 width, FxI32 height)
{
}

FX_ENTRY void FX_CALL grTexClampMode(GrChipID_t tmu, GrTextureClampMode_t s_clampmode, GrTextureClampMode_t t_clampmode)
{
}

FX_ENTRY void FX_CALL grTexCombine(GrChipID_t tmu, GrCombineFunction_t rgb_function, GrCombineFactor_t rgb_factor, GrCombineFunction_t alpha_function, GrCombineFactor_t alpha_factor, FxBool rgb_invert, FxBool alpha_invert)
{
}

FX_ENTRY void FX_CALL grTexFilterMode(GrChipID_t tmu, GrTextureFilterMode_t minfilter_mode, GrTextureFilterMode_t magfilter_mode)
{
}

FX_ENTRY void FX_CALL grTexMipMapMode(GrChipID_t tmu, GrMipMapMode_t mode, FxBool lodBlend)
{
}

FX_ENTRY void FX_CALL grGlideInit(void)
{
}

FX_ENTRY void FX_CALL grGlideShutdown(void)
{
}

// Unused functions

FX_ENTRY FxI32 FX_CALL grQueryResolutions(const GrResolution* resTemplate, GrResolution* output)
{
	assert(false && "grQueryResolutions");
	return 0;
}

FX_ENTRY FxBool FX_CALL grReset(FxU32 what)
{
	assert(false && "grReset");
	return FXTRUE;
}

FX_ENTRY void FX_CALL grEnable(GrEnableMode_t mode)
{
	assert(false && "grEnable");
}

FX_ENTRY void FX_CALL grDisable(GrEnableMode_t mode)
{
	assert(false && "grDisable");
}

FX_ENTRY void FX_CALL grGlideGetState(void* state)
{
	assert(false && "grGlideGetState");
}

FX_ENTRY void FX_CALL grGlideSetState(const void* state)
{
	assert(false && "grGlideSetState");
}

FX_ENTRY void FX_CALL grGlideGetVertexLayout(void* layout)
{
	assert(false && "grGlideGetVertexLayout");
}

FX_ENTRY void FX_CALL grGlideSetVertexLayout(const void* layout)
{
	assert(false && "grGlideSetVertexLayout");
}

FX_ENTRY float FX_CALL guFogTableIndexToW(int i)
{
	assert(false && "guFogTableIndexToW");
	return 0.0f;
}

FX_ENTRY void FX_CALL guFogGenerateExp(GrFog_t* fogtable, float density)
{
	assert(false && "guFogGenerateExp");
}

FX_ENTRY void FX_CALL guFogGenerateExp2(GrFog_t* fogtable, float density)
{
	assert(false && "guFogGenerateExp2");
}

FX_ENTRY void FX_CALL guFogGenerateLinear(GrFog_t* fogtable, float nearZ, float farZ)
{
	assert(false && "guFogGenerateLinear");
}

FX_ENTRY FxBool FX_CALL gu3dfGetInfo(const char* filename, Gu3dfInfo* info)
{
	assert(false && "gu3dfGetInfo");
	return FXFALSE;
}

FX_ENTRY FxBool FX_CALL gu3dfLoad(const char* filename, Gu3dfInfo* data)
{
	assert(false && "gu3dfLoad");
	return FXFALSE;
}

FX_ENTRY void FX_CALL grCheckForRoom(FxI32 n)
{
	assert(false && "grCheckForRoom");
}

FX_ENTRY int FX_CALL guEncodeRLE16(void* dst, void* src, FxU32 width, FxU32 height)
{
	assert(false && "guEncodeRLE16");
	return 0;
}

FX_ENTRY void FX_CALL grSstVidMode(FxU32 whichSst, void* vidTimings)
{
	assert(false && "grSstVidMode");
}

FX_ENTRY void FX_CALL grAlphaControlsITRGBLighting(FxBool enable)
{
	assert(false && "grAlphaControlsITRGBLighting");
}

FX_ENTRY void FX_CALL grAlphaTestFunction(GrCmpFnc_t function)
{
	assert(false && "grAlphaTestFunction");
}

FX_ENTRY void FX_CALL grAlphaTestReferenceValue(GrAlpha_t value)
{
	assert(false && "grAlphaTestReferenceValue");
}

FX_ENTRY void FX_CALL grLfbConstantAlpha(GrAlpha_t alpha)
{
	assert(false && "grLfbConstantAlpha");
}

FX_ENTRY void FX_CALL grLfbConstantDepth(FxU32 depth)
{
	assert(false && "grLfbConstantDepth");
}

FX_ENTRY void FX_CALL grLfbWriteColorSwizzle(FxBool swizzleBytes, FxBool swapWords)
{
	assert(false && "grLfbWriteColorSwizzle");
}

FX_ENTRY void FX_CALL grLfbWriteColorFormat(GrColorFormat_t colorFormat)
{
	assert(false && "grLfbWriteColorFormat");
}

FX_ENTRY FxBool FX_CALL grLfbWriteRegion(GrBuffer_t dst_buffer, FxU32 dst_x, FxU32 dst_y, GrLfbSrcFmt_t src_format, FxU32 src_width, FxU32 src_height, FxBool pixelPipeline, FxI32 src_stride, void* src_data)
{
	assert(false && "grLfbWriteRegion");
	return FXFALSE;
}

FX_ENTRY FxBool FX_CALL grLfbReadRegion(GrBuffer_t src_buffer, FxU32 src_x, FxU32 src_y, FxU32 src_width, FxU32 src_height, FxU32 dst_stride, void* dst_data)
{
	assert(false && "grLfbReadRegion");
	return FXFALSE;
}

FX_ENTRY void FX_CALL grTexMultibase(GrChipID_t tmu, FxBool enable)
{
	assert(false && "grTexMultibase");
}

FX_ENTRY void FX_CALL grTexMultibaseAddress(GrChipID_t tmu, GrTexBaseRange_t range, FxU32 startAddress, FxU32 evenOdd, GrTexInfo* info)
{
	assert(false && "grTexMultibaseAddress");
}

FX_ENTRY void FX_CALL grDrawTriangle(const void* a, const void* b, const void* c)
{
	assert(false && "grDrawTriangle");
}

FX_ENTRY void FX_CALL grAADrawTriangle(const void* a, const void* b, const void* c, FxBool ab_antialias, FxBool bc_antialias, FxBool ca_antialias)
{
	assert(false && "grAADrawTriangle");
}

FX_ENTRY void FX_CALL grClipWindow(FxU32 minx, FxU32 miny, FxU32 maxx, FxU32 maxy)
{
	assert(false && "grClipWindow");
}

FX_ENTRY void FX_CALL grSstOrigin(GrOriginLocation_t origin)
{
	assert(false && "grSstOrigin");
}

FX_ENTRY void FX_CALL grCullMode(GrCullMode_t mode)
{
	assert(false && "grCullMode");
}

FX_ENTRY void FX_CALL grDepthBiasLevel(FxI32 level)
{
	assert(false && "grDepthBiasLevel");
}

FX_ENTRY void FX_CALL grDepthBufferFunction(GrCmpFnc_t function)
{
	assert(false && "grDepthBufferFunction");
}

FX_ENTRY void FX_CALL grDepthBufferMode(GrDepthBufferMode_t mode)
{
	assert(false && "grDepthBufferMode");
}

FX_ENTRY void FX_CALL grFogColorValue(GrColor_t fogcolor)
{
	assert(false && "grFogColorValue");
}

FX_ENTRY void FX_CALL grFogMode(GrFogMode_t mode)
{
	assert(false && "grFogMode");
}

FX_ENTRY void FX_CALL grFogTable(const GrFog_t ft[])
{
	assert(false && "grFogTable");
}

FX_ENTRY void FX_CALL grSplash(float x, float y, float width, float height, FxU32 frame)
{
	assert(false && "grSplash");
}

FX_ENTRY GrProc FX_CALL grGetProcAddress(char* procName)
{
	assert(false && "grGetProcAddress");
	return NULL;
}

FX_ENTRY void FX_CALL grDepthRange(FxFloat n, FxFloat f)
{
	assert(false && "grDepthRange");
}

FX_ENTRY void FX_CALL grTexNCCTable(GrNCCTable_t table)
{
	assert(false && "grTexNCCTable");
}

FX_ENTRY void FX_CALL grTexDetailControl(GrChipID_t tmu, int lod_bias, FxU8 detail_scale, float detail_max)
{
	assert(false && "grTexDetailControl");
}

FX_ENTRY void FX_CALL grTexLodBiasValue(GrChipID_t tmu, float bias)
{
	assert(false && "grTexLodBiasValue");
}

FX_ENTRY void FX_CALL grTexDownloadMipMapLevel(GrChipID_t tmu, FxU32 startAddress, GrLOD_t thisLod, GrLOD_t largeLod, GrAspectRatio_t aspectRatio, GrTextureFormat_t format, FxU32 evenOdd, void* data)
{
	assert(false && "grTexDownloadMipMapLevel");
}

FX_ENTRY FxBool FX_CALL grTexDownloadMipMapLevelPartial(GrChipID_t tmu, FxU32 startAddress, GrLOD_t thisLod, GrLOD_t largeLod, GrAspectRatio_t aspectRatio, GrTextureFormat_t format, FxU32 evenOdd, void* data, int start, int end)
{
	assert(false && "grTexDownloadMipMapLevelPartial");
	return FXFALSE;
}

FX_ENTRY void FX_CALL grTexDownloadTablePartial(GrTexTable_t type, void* data, int start, int end)
{
	assert(false && "grTexDownloadTablePartial");
}

FX_ENTRY FxU32 FX_CALL grTexCalcMemRequired(GrLOD_t lodmin, GrLOD_t lodmax, GrAspectRatio_t aspect, GrTextureFormat_t fmt)
{
	assert(false && "grTexCalcMemRequired");
	return 0;
}

FX_ENTRY FxU32 FX_CALL grTexTextureMemRequired(FxU32 evenOdd, GrTexInfo* info)
{
	assert(false && "grTexTextureMemRequired");
	return 0;
}

FX_ENTRY void FX_CALL grDisableAllEffects(void)
{
	assert(false && "grDisableAllEffects");
}

#ifdef __cplusplus
}
#endif