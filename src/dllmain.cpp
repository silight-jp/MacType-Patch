#include <windows.h>

#include "common.h"
#include "userparams.h"


#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "dwrite.lib")
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "Shlwapi.lib")

#ifndef _WIN64
	#pragma comment(lib, "mhook32.lib")
	LPCWSTR MacTypeDllName = L"MacType.dll";
#else
	#pragma comment(lib, "mhook64.lib")
	LPCWSTR MacTypeDllName = L"MacType64.dll";
#endif



extern void init() {
	HMODULE hModule = GetModuleHandleW(MacTypeDllName);
	if (hModule) {
		loadUserParams(hModule);
		if (GeneralParams.isHookTarget) {
			initMacTypeIntercept();
			hookDirectWrite();
			hookDirect2D();
		}
	}
}

extern "C" BOOL EasyHook_DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved);

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		break;
	case DLL_PROCESS_DETACH:
		unhookAll();
		break;
	default:
		break;
	}
	return EasyHook_DllMain(hModule, ul_reason_for_call, lpReserved);
}
