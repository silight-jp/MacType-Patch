#include <windows.h>
#include "common.h"


namespace Orig
{
	static decltype(&::ExtTextOutW) ExtTextOutW;
	static decltype(&::GetGlyphOutlineA) GetGlyphOutlineA;
	static decltype(&::GetGlyphOutlineW) GetGlyphOutlineW;
	static int(WINAPI *GetTextFaceAliasW)(HDC hdc, int cChar, LPWSTR pszOut);
	static decltype(&::GetTextFaceW) GetTextFaceW;
}



#pragma auto_inline(off)
namespace Dummy
{
	static BOOL WINAPI ExtTextOutA(
		HDC hdc,
		int X,
		int Y,
		UINT fuOptions,
		CONST RECT *lprc,
		LPCSTR lpString,
		UINT cbCount,
		CONST INT *lpDx
	) {
		return ::ExtTextOutA(hdc, X, Y, fuOptions, lprc, lpString, cbCount, lpDx);
	}
	
	static BOOL WINAPI ExtTextOutW(
		HDC hdc,
		int X,
		int Y,
		UINT fuOptions,
		CONST RECT *lprc,
		LPCWSTR lpString,
		UINT cbCount,
		CONST INT *lpDx
	) {
		auto orig = getWithLock(&Orig::ExtTextOutW);
		return orig(hdc, X, Y, fuOptions, lprc, lpString, cbCount, lpDx);
	}
	
	static BOOL WINAPI GetGlyphOutlineA(
		HDC hdc,
		UINT uChar,
		UINT uFormat,
		LPGLYPHMETRICS lpgm,
		DWORD cbBuffer,
		LPVOID lpvBuffer,
		CONST MAT2 *lpmat2
	) {
		auto orig = getWithLock(&Orig::GetGlyphOutlineA);
		return orig(hdc, uChar, uFormat, lpgm, cbBuffer, lpvBuffer, lpmat2);
	}
	
	static BOOL WINAPI GetGlyphOutlineW(
		HDC hdc,
		UINT uChar,
		UINT uFormat,
		LPGLYPHMETRICS lpgm,
		DWORD cbBuffer,
		LPVOID lpvBuffer,
		CONST MAT2 *lpmat2
	) {
		auto orig = getWithLock(&Orig::GetGlyphOutlineW);
		return orig(hdc, uChar, uFormat, lpgm, cbBuffer, lpvBuffer, lpmat2);
	}
	
	static int WINAPI GetTextFaceAliasW(
		HDC hdc,
		int cChar,
		LPWSTR pszOut
	) {
		auto orig = getWithLock(&Orig::GetTextFaceAliasW);
		return orig(hdc, cChar, pszOut);
	}
	
	static int WINAPI GetTextFaceW(
		HDC hdc,
		int nCount,
		LPWSTR lpFaceName
	) {
		auto orig = getWithLock(&Orig::GetTextFaceW);
		return orig(hdc, nCount, lpFaceName);
	}
	
	static BOOL WINAPI TextOutA(
		HDC hdc,
		int nXStart,
		int nYStart,
		LPCSTR lpString,
		int cbString
	) {
		return ::TextOutA(hdc, nXStart, nYStart, lpString, cbString);
	}
	
	static BOOL WINAPI TextOutW(
		HDC hdc,
		int nXStart,
		int nYStart,
		LPCWSTR lpString,
		int cbString
	) {
		return ::TextOutW(hdc, nXStart, nYStart, lpString, cbString);
	}
}
#pragma auto_inline(on)



namespace Impl
{
	static BOOL WINAPI ExtTextOutW(
		HDC hdc,
		int X,
		int Y,
		UINT fuOptions,
		CONST RECT *lprc,
		LPCWSTR lpString,
		UINT cbCount,
		CONST INT *lpDx
	) {
		if (lpDx && fuOptions & ETO_PDY) {
			int tx = X, ty = Y, dx = 0, dy = 0;
			LPCWSTR tStr = lpString;
			UINT tCount = 0;
			CONST INT *tDx = lpDx;
			for (size_t i = 0; i < cbCount; i++) {
				tCount++;
				if (i < cbCount - 1) {
					dx += lpDx[2 * i];
					dy += lpDx[2 * i + 1];
					if (dy == 0) continue;
				}
				Dummy::ExtTextOutW(hdc, tx, ty, fuOptions, lprc, tStr, tCount, tDx);
				tx += dx;
				ty -= dy;
				dx = 0;
				dy = 0;
				tStr = &lpString[i + 1];
				tCount = 0;
				tDx = &lpDx[2 * i + 2];
			}
			return TRUE;
		}
		
		if (lprc && (fuOptions & ETO_CLIPPED) && !(fuOptions & ETO_OPAQUE)) {
			RECT expand = {
				lprc->left - 2,
				lprc->top - 1,
				lprc->right + 1,
				lprc->bottom + 1
			};
			return Dummy::ExtTextOutW(hdc, X, Y, fuOptions, &expand, lpString, cbCount, lpDx);
		} else {
			return Dummy::ExtTextOutW(hdc, X, Y, fuOptions, lprc, lpString, cbCount, lpDx);
		}
	}
	
	static DWORD WINAPI GetGlyphOutlineA(
		HDC hdc,
		UINT uChar,
		UINT uFormat,
		LPGLYPHMETRICS lpgm,
		DWORD cbBuffer,
		LPVOID lpvBuffer,
		CONST MAT2 *lpmat2
	) {
		DWORD ret = ::GetGlyphOutlineA(hdc, uChar, uFormat, lpgm, cbBuffer, lpvBuffer, lpmat2);
		if (!cbBuffer || !lpvBuffer) {
			if (!(uFormat & (GGO_BITMAP | GGO_GRAY2_BITMAP | GGO_GRAY4_BITMAP | GGO_GRAY8_BITMAP | GGO_NATIVE))) {
				lpgm->gmptGlyphOrigin.x -= 1;
				lpgm->gmptGlyphOrigin.y += 1;
				lpgm->gmBlackBoxX += 3;
				lpgm->gmBlackBoxY += 2;
			}
		}
		return ret;
	}
	
	static DWORD WINAPI GetGlyphOutlineW(
		HDC hdc,
		UINT uChar,
		UINT uFormat,
		LPGLYPHMETRICS lpgm,
		DWORD cbBuffer,
		LPVOID lpvBuffer,
		CONST MAT2 *lpmat2
	) {
		DWORD ret = ::GetGlyphOutlineW(hdc, uChar, uFormat, lpgm, cbBuffer, lpvBuffer, lpmat2);
		if (!cbBuffer || !lpvBuffer) {
			if (!(uFormat & (GGO_BITMAP | GGO_GRAY2_BITMAP | GGO_GRAY4_BITMAP | GGO_GRAY8_BITMAP | GGO_NATIVE))) {
				lpgm->gmptGlyphOrigin.x -= 1;
				lpgm->gmptGlyphOrigin.y += 1;
				lpgm->gmBlackBoxX += 3;
				lpgm->gmBlackBoxY += 2;
			}
		}
		return ret;
	}
	
	static int WINAPI GetTextFaceAliasW(
		HDC hdc,
		int cChar,
		LPWSTR pszOut
	) {
		auto lock = globalMutex.getLock();
		return Dummy::GetTextFaceAliasW(hdc, cChar, pszOut);
	}
	
	static int WINAPI GetTextFaceW(
		HDC hdc,
		int nCount,
		LPWSTR lpFaceName
	) {
		auto lock = globalMutex.getLock();
		return Dummy::GetTextFaceW(hdc, nCount, lpFaceName);
	}
}



extern void initMacTypeIntercept() {
	auto lock = globalMutex.getLock();
	
	HMODULE hModuleGdi32 = GetModuleHandle(L"gdi32.dll");
	if (!hModuleGdi32) return;
	
	FARPROC ExtTextOutAProc = GetProcAddress(hModuleGdi32, "ExtTextOutA");
	if (ExtTextOutAProc) {
		changeHookTarget(ExtTextOutAProc, Dummy::ExtTextOutA);
	}
	
	FARPROC ExtTextOutWProc = GetProcAddress(hModuleGdi32, "ExtTextOutW");
	if (ExtTextOutWProc) {
		changeHookTarget(
			ExtTextOutWProc,
			Dummy::ExtTextOutW,
			Impl::ExtTextOutW,
			(void**)&Orig::ExtTextOutW
		);
	}
	
	FARPROC GetGlyphOutlineAProc = GetProcAddress(hModuleGdi32, "GetGlyphOutlineA");
	if (GetGlyphOutlineAProc) {
		changeHookTarget(
			GetGlyphOutlineAProc,
			Dummy::GetGlyphOutlineA,
			Impl::GetGlyphOutlineA,
			(void**)&Orig::GetGlyphOutlineA
		);
	}
	
	FARPROC GetGlyphOutlineWProc = GetProcAddress(hModuleGdi32, "GetGlyphOutlineW");
	if (GetGlyphOutlineWProc) {
		changeHookTarget(
			GetGlyphOutlineWProc,
			Dummy::GetGlyphOutlineW,
			Impl::GetGlyphOutlineW,
			(void**)&Orig::GetGlyphOutlineW
		);
	}
	
	FARPROC GetTextFaceAliasWProc = GetProcAddress(hModuleGdi32, "GetTextFaceAliasW");
	if (GetTextFaceAliasWProc) {
		changeHookTargetWithMhook(
			GetTextFaceAliasWProc,
			Dummy::GetTextFaceAliasW,
			Impl::GetTextFaceAliasW,
			(void**)&Orig::GetTextFaceAliasW
		);
	}
	
	FARPROC GetTextFaceWProc = GetProcAddress(hModuleGdi32, "GetTextFaceW");
	if (GetTextFaceWProc) {
		changeHookTargetWithMhook(
			GetTextFaceWProc,
			Dummy::GetTextFaceW,
			Impl::GetTextFaceW,
			(void**)&Orig::GetTextFaceW
		);
	}
	
	FARPROC TextOutAProc = GetProcAddress(hModuleGdi32, "TextOutA");
	if (TextOutAProc) {
		changeHookTarget(TextOutAProc, Dummy::TextOutA);
	}
	
	FARPROC TextOutWProc = GetProcAddress(hModuleGdi32, "TextOutW");
	if (TextOutWProc) {
		changeHookTarget(TextOutWProc, Dummy::TextOutW);
	}
}
