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
		auto lock = globalMutex.getLock();
		return Orig::GetTextFaceAliasW(hdc, cChar, pszOut);
	}
	
	static int WINAPI GetTextFaceW(
		HDC hdc,
		int nCount,
		LPWSTR lpFaceName
	) {
		auto lock = globalMutex.getLock();
		return Orig::GetTextFaceW(hdc, nCount, lpFaceName);
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
	static BOOL WINAPI ExtTextOutW_BugFix3(
		HDC hdc,
		int X,
		int Y,
		UINT fuOptions,
		CONST RECT *lprc,
		LPCWSTR lpString,
		UINT cbCount,
		CONST INT *lpDx
	) {
		// bug fix of vertical text
		if (fuOptions & ETO_PDY) {
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
		return Dummy::ExtTextOutW(hdc, X, Y, fuOptions, lprc, lpString, cbCount, lpDx);
	}


	static BOOL WINAPI ExtTextOutW_BugFix2(
		HDC hdc,
		int X,
		int Y,
		UINT fuOptions,
		CONST RECT *lprc,
		LPCWSTR lpString,
		UINT cbCount,
		CONST INT *lpDx
	) {
		// space bug fix
		if (cbCount > 1 && (GetTextAlign(hdc) & 0x7) == (TA_NOUPDATECP & TA_LEFT)) {
			bool isSucceeded = true;
			UINT16 space = L' ';
			if (fuOptions & ETO_GLYPH_INDEX) {
				isSucceeded = GetGlyphIndicesW(hdc, L" ", 1, &space, GGI_MARK_NONEXISTING_GLYPHS) == 1;
			}
			if (isSucceeded) {
				WCHAR* textStr = new WCHAR[cbCount];
				WCHAR* spaceStr = new WCHAR[cbCount];
				UINT textCount = 0;
				UINT spaceCount = 0;
				INT* textDx = new INT[cbCount * 2 + 2];
				INT* spaceDx = new INT[cbCount * 2 + 2];
				int textX = 0, textY = 0, spaceX = 0, spaceY = 0;
				if (textStr && spaceStr && textDx && spaceDx) {
					if (fuOptions & ETO_PDY) {
						for (size_t i = 0; i < cbCount; i++) {
							if (lpString[i] == space) {
								spaceStr[spaceCount] = lpString[i];
								spaceDx[2 * spaceCount] = spaceX;
								spaceDx[2 * spaceCount + 1] = spaceY;
								spaceX = 0;
								spaceY = 0;
								spaceCount++;
							} else {
								textStr[textCount] = lpString[i];
								textDx[2 * textCount] = textX;
								textDx[2 * textCount + 1] = textY;
								textX = 0;
								textY = 0;
								textCount++;
							}
							textX += lpDx[2 * i];
							textY += lpDx[2 * i + 1];
							spaceX += lpDx[2 * i];
							spaceY += lpDx[2 * i + 1];
						}
						spaceDx[2 * spaceCount] = spaceX;
						spaceDx[2 * spaceCount + 1] = spaceY;
						textDx[2 * textCount] = textX;
						textDx[2 * textCount + 1] = textY;
						if (spaceCount) {
							ExtTextOutW_BugFix3(hdc, X + spaceDx[0], Y - spaceDx[1], fuOptions, lprc, spaceStr, spaceCount, spaceDx + 2);
						}
						if (textCount) {
							ExtTextOutW_BugFix3(hdc, X + textDx[0], Y - textDx[1], fuOptions, lprc, textStr, textCount, textDx + 2);
						}
					} else {
						for (size_t i = 0; i < cbCount; i++) {
							if (lpString[i] == space) {
								spaceStr[spaceCount] = lpString[i];
								spaceDx[spaceCount] = spaceX;
								spaceX = 0;
								spaceCount++;
							} else {
								textStr[textCount] = lpString[i];
								textDx[textCount] = textX;
								textX = 0;
								textCount++;
							}
							textX += lpDx[i];
							spaceX += lpDx[i];
						}
						spaceDx[spaceCount] = spaceX;
						textDx[textCount] = textX;
						if (spaceCount) {
							ExtTextOutW_BugFix3(hdc, X + spaceDx[0], Y, fuOptions, lprc, spaceStr, spaceCount, spaceDx + 1);
						}
						if (textCount) {
							ExtTextOutW_BugFix3(hdc, X + textDx[0], Y, fuOptions, lprc, textStr, textCount, textDx + 1);
						}
					}
				}

				if (textStr) delete[] textStr;
				if (spaceStr) delete[] spaceStr;
				if (textDx) delete[] textDx;
				if (spaceDx) delete[] spaceDx;

				if (textStr && spaceStr && textDx && spaceDx) {
					return TRUE;
				}
			}
		}
		return ExtTextOutW_BugFix3(hdc, X, Y, fuOptions, lprc, lpString, cbCount, lpDx);
	}

	static BOOL WINAPI ExtTextOutW_BugFix1(
		HDC hdc,
		int X,
		int Y,
		UINT fuOptions,
		CONST RECT *lprc,
		LPCWSTR lpString,
		UINT cbCount,
		CONST INT *lpDx
	) {
		// use lpDx in ExtTextOutW_BugFix2
		if (!lpDx && !(fuOptions & ETO_PDY)) {
			INT* dx = new INT[cbCount]{};
			if (dx) {
				SIZE size = { };
				BOOL isSucceeded = fuOptions & ETO_GLYPH_INDEX ?
					GetTextExtentExPointI(hdc, (LPWORD)lpString, cbCount, 0, nullptr, dx, &size) :
					GetTextExtentExPointW(hdc, lpString, cbCount, 0, nullptr, dx, &size);
				if (isSucceeded) {
					for (size_t i = cbCount - 1; i > 0; i--) {
						dx[i] -= dx[i - 1];
					}
					BOOL ret = ExtTextOutW_BugFix2(hdc, X, Y, fuOptions, lprc, lpString, cbCount, dx);
					delete[] dx;
					return ret;
				}
			}

		}
		return ExtTextOutW_BugFix2(hdc, X, Y, fuOptions, lprc, lpString, cbCount, lpDx);
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
		auto lock = globalMutex.getLock();

		// invalid arguments
		bool invalidArguments = !hdc || !lpString || !cbCount;
		
		// no support printer
		bool noDisplay = GetDeviceCaps(hdc, TECHNOLOGY) != DT_RASDISPLAY;

		// need recurse
		bool needRecurse = !(fuOptions & ETO_GLYPH_INDEX) && !(fuOptions & ETO_IGNORELANGUAGE);

		if (invalidArguments || noDisplay || needRecurse) {
			return Orig::ExtTextOutW(hdc, X, Y, fuOptions, lprc, lpString, cbCount, lpDx);
		}

		// draw opaque background
		if (fuOptions & ETO_OPAQUE) {
			Orig::ExtTextOutW(hdc, X, Y, fuOptions, lprc, NULL, 0, NULL);
			fuOptions &= ~ETO_OPAQUE;
		}

		return ExtTextOutW_BugFix1(hdc, X, Y, fuOptions, lprc, lpString, cbCount, lpDx);
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
		changeHookTargetWithMhookForcibly(
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
