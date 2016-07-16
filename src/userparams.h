#pragma once

#include <windows.h>
#include <dwrite_3.h>


enum class HookTargetEnum {
	All, ExcludeMode, IncludeMode, None
};

struct _GeneralParams {
	HookTargetEnum HookTarget = HookTargetEnum::None;
	bool isHookTarget = false;
};

struct RenderingParams {
	FLOAT Gamma = 1.5f;
	FLOAT EnhancedContrast = 1.0f;
	FLOAT ClearTypeLevel = 1.0f;
	DWRITE_PIXEL_GEOMETRY PixelGeometry = DWRITE_PIXEL_GEOMETRY_RGB;
	DWRITE_RENDERING_MODE RenderingMode = DWRITE_RENDERING_MODE_NATURAL_SYMMETRIC;
	FLOAT GrayscaleEnhancedContrast = 1.0f;
	DWRITE_GRID_FIT_MODE GridFitMode = DWRITE_GRID_FIT_MODE_DISABLED;
	DWRITE_RENDERING_MODE1 RenderingMode1 = DWRITE_RENDERING_MODE1_NATURAL_SYMMETRIC;
	IDWriteRenderingParams* pDWriteRenderingParams;
};


extern _GeneralParams GeneralParams;
extern RenderingParams DirectWriteParams;
extern RenderingParams Direct2DParams;

extern void loadUserParams(HMODULE hDllModule);
