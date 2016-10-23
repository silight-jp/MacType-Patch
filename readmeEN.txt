This software is patch of MacType.
You can change DirectWrite parameters.

How to use

1. Open MacType directory.
2. Rename or delete EasyHK32.dll and EasyHK64.dll(64bit only)
3. Copy our DLLs and UserParams.ini to MacType directory.
4. Also Copy our DLLs to System directory as below.
32bit
	C:\Windows\System32\EasyHK32.dll
64bit
	C:\Windows\System32\EasyHK64.dll
	C:\Windows\SysWOW64\EasyHK32.dll

Good Luck!

Version History
2016/10/15	1.18	Improve compatibility with MacTray.
			Support DynamicCodePolicy.
2016/10/11	1.17	Bug fix for High DPI.
			Add ForceNoHinting for Windows Vista or later.
2016/10/08	1.16	Bug fix.
2016/10/07	1.15	Bug fix and speed up.
2016/10/05	1.14	Bug fix for High DPI.
			Bug fix for text including space.
			Add FontSubstitutes experimentally.
2016/08/28	1.13	Hook New Version of Direct2D.
			Fixed printer bug.
			Do not Initialize DirectWrite if software don't use.
2016/07/16	1.12	EasyHook updated.
2016/06/27	1.11	ReplaceDirectWriteWithGDI was deleted.
2016/06/22	1.10	Parameters were reduced.
2016/06/20	1.9	IsPixelSnappingDisabled was deleted.
2016/06/19	1.8	Store apps were supported.
2016/06/18	1.7	MacType bug fix.
2016/06/14	1.6	maybe more stable.
2016/06/11	1.5	UserParams.ini was changed.
			Firefox was more stable.
2016/06/09	1.4	bug fix.
2016/06/08	1.3	bug fix.
2016/06/05	1.2	D2D1AntialiasMode was deleted.
			Hook was more safe;
			Vertical text bug on MacType was partially fixed.
2016/06/04	1.1	UserParams.ini updated.
			DWriteTextAntialiasMode was deleted.
			HookTarget was added.
2016/06/01	1.0	Published.
