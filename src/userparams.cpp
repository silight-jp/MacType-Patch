#include "userparams.h"

#include <set>
#include <windows.h>
#include <dwrite_3.h>
#include <Shlwapi.h>



_GeneralParams GeneralParams;
RenderingParams DirectWriteParams;
RenderingParams Direct2DParams;



static void loadExeName(std::set<std::wstring, std::less<>>& set, LPCWSTR section, LPCWSTR path) {
	#define BUFFER_SIZE 4096
	wchar_t* buffer = new wchar_t[BUFFER_SIZE];
	size_t load = GetPrivateProfileSectionW(section, buffer, BUFFER_SIZE, path);
	wchar_t* p = buffer;
	while (load > 0) {
		size_t len = wcsnlen(p, load);
		if (len >= load) {
			break;
		}
		set.insert(CharLower(p));
		p += len + 1;
		load -= len + 1;
	}
	delete[] buffer;
	#undef BUFFER_SIZE
}

static void loadGeneralParams(LPCWSTR path) {
	GeneralParams.HookTarget = (HookTargetEnum)GetPrivateProfileIntW(
		L"General",
		L"HookTarget",
		(int)GeneralParams.HookTarget,
		path);
	
	wchar_t exePath[_MAX_PATH];
	GetModuleFileNameW(NULL, exePath, _MAX_PATH);
	wchar_t* exeName = PathFindFileNameW(exePath);
	CharLower(exeName);
	
	switch (GeneralParams.HookTarget) {
		case HookTargetEnum::All:
			GeneralParams.isHookTarget = true;
			break;
		case HookTargetEnum::ExcludeMode: {
			std::set<std::wstring, std::less<>> set;
			loadExeName(set, L"Exclude", path);
			GeneralParams.isHookTarget = set.count(exeName) == 0;
			break;
		}
		case HookTargetEnum::IncludeMode: {
			std::set<std::wstring, std::less<>> set;
			loadExeName(set, L"Include", path);
			GeneralParams.isHookTarget = set.count(exeName) > 0;
			break;
		}
		case HookTargetEnum::None:
			GeneralParams.isHookTarget = false;
			break;
	}
}

static void loadRenderingParams(RenderingParams& params, LPCWSTR section, LPCWSTR path) {
	params.Gamma = GetPrivateProfileIntW(
		section,
		L"Gamma",
		(int)(params.Gamma * 1000),
		path) / 1000.0f;
	params.EnhancedContrast = GetPrivateProfileIntW(
		section,
		L"EnhancedContrast",
		(int)(params.EnhancedContrast * 100),
		path) / 100.0f;
	params.ClearTypeLevel = GetPrivateProfileIntW(
		section,
		L"ClearTypeLevel",
		(int)(params.ClearTypeLevel * 100),
		path) / 100.0f;
	params.PixelGeometry = (DWRITE_PIXEL_GEOMETRY)GetPrivateProfileIntW(
		section,
		L"PixelGeometry",
		params.PixelGeometry,
		path);
	params.RenderingMode = (DWRITE_RENDERING_MODE)GetPrivateProfileIntW(
		section,
		L"RenderingMode",
		params.RenderingMode,
		path);
	params.GrayscaleEnhancedContrast = GetPrivateProfileIntW(
		section,
		L"GrayscaleEnhancedContrast",
		(int)(params.GrayscaleEnhancedContrast * 100),
		path) / 100.0f;
	params.GridFitMode = (DWRITE_GRID_FIT_MODE)GetPrivateProfileIntW(
		section,
		L"GridFitMode",
		params.GridFitMode,
		path);
	params.RenderingMode1 = (DWRITE_RENDERING_MODE1)GetPrivateProfileIntW(
		section,
		L"RenderingMode1",
		params.RenderingMode1,
		path);
}

static void initDWriteRenderingParams(RenderingParams& params, IDWriteFactory3* pDWriteFactory3) {
	IDWriteRenderingParams3* pDWriteRenderingParams3;
	HRESULT hr = pDWriteFactory3->CreateCustomRenderingParams(
		params.Gamma,
		params.EnhancedContrast,
		params.GrayscaleEnhancedContrast,
		params.ClearTypeLevel,
		params.PixelGeometry,
		params.RenderingMode1,
		params.GridFitMode,
		&pDWriteRenderingParams3
	);
	if (SUCCEEDED(hr)) {
		params.pDWriteRenderingParams = pDWriteRenderingParams3;
	}
}

static void initDWriteRenderingParams(RenderingParams& params, IDWriteFactory2* pDWriteFactory2) {
	IDWriteRenderingParams2* pDWriteRenderingParams2;
	HRESULT hr = pDWriteFactory2->CreateCustomRenderingParams(
		params.Gamma,
		params.EnhancedContrast,
		params.GrayscaleEnhancedContrast,
		params.ClearTypeLevel,
		params.PixelGeometry,
		params.RenderingMode,
		params.GridFitMode,
		&pDWriteRenderingParams2
	);
	if (SUCCEEDED(hr)) {
		params.pDWriteRenderingParams = pDWriteRenderingParams2;
	}
}

static void initDWriteRenderingParams(RenderingParams& params, IDWriteFactory1* pDWriteFactory1) {
	IDWriteRenderingParams1* pDWriteRenderingParams1;
	HRESULT hr = pDWriteFactory1->CreateCustomRenderingParams(
		params.Gamma,
		params.EnhancedContrast,
		params.GrayscaleEnhancedContrast,
		params.ClearTypeLevel,
		params.PixelGeometry,
		params.RenderingMode,
		&pDWriteRenderingParams1
	);
	if (SUCCEEDED(hr)) {
		params.pDWriteRenderingParams = pDWriteRenderingParams1;
	}
}

static void initDWriteRenderingParams(RenderingParams& params, IDWriteFactory* pDWriteFactory) {
	IDWriteRenderingParams* pDWriteRenderingParams;
	HRESULT hr = pDWriteFactory->CreateCustomRenderingParams(
		params.Gamma,
		params.EnhancedContrast,
		params.ClearTypeLevel,
		params.PixelGeometry,
		params.RenderingMode,
		&pDWriteRenderingParams
	);
	if (SUCCEEDED(hr)) {
		params.pDWriteRenderingParams = pDWriteRenderingParams;
	}
}

template<typename IDWriteFactoryN>
static HRESULT initDWriteRenderingParamsHelper() {
	IDWriteFactoryN* pDWriteFactoryN;
	HRESULT hr = DWriteCreateFactory(
		DWRITE_FACTORY_TYPE_ISOLATED,
		__uuidof(IDWriteFactoryN),
		reinterpret_cast<IUnknown**>(&pDWriteFactoryN)
	);
	if (SUCCEEDED(hr)) {
		initDWriteRenderingParams(Direct2DParams, pDWriteFactoryN);
		initDWriteRenderingParams(DirectWriteParams, pDWriteFactoryN);
		pDWriteFactoryN->Release();
	}
	return hr;
}

static void loadRenderingParams(LPCWSTR path) {
	loadRenderingParams(Direct2DParams, L"Direct2D", path);
	loadRenderingParams(DirectWriteParams, L"DirectWrite", path);
	HRESULT hr;
	hr = initDWriteRenderingParamsHelper<IDWriteFactory3>();
	if (SUCCEEDED(hr)) return;
	hr = initDWriteRenderingParamsHelper<IDWriteFactory2>();
	if (SUCCEEDED(hr)) return;
	hr = initDWriteRenderingParamsHelper<IDWriteFactory1>();
	if (SUCCEEDED(hr)) return;
	hr = initDWriteRenderingParamsHelper<IDWriteFactory>();
	if (SUCCEEDED(hr)) return;
}

extern void loadUserParams(HMODULE hDllModule) {
	wchar_t path[_MAX_PATH];
	GetModuleFileNameW(hDllModule, path, _MAX_PATH);
	PathRemoveFileSpecW(path);
	PathAppendW(path, L"UserParams.ini");
	
	loadGeneralParams(path);
	if (GeneralParams.isHookTarget) {
		loadRenderingParams(path);
	}
}
