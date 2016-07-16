#pragma once

#include "easyhook.h"

#define STATUS_SUCCESS 0;

typedef struct _HOOK_ACL_ {
	ULONG					Count;
	BOOL					IsExclusive;
	ULONG					Entries[MAX_ACE_COUNT];
} HOOK_ACL;

typedef struct _LOCAL_HOOK_INFO_ {
	PLOCAL_HOOK_INFO		Next;
	ULONG					NativeSize;
	void*					TargetProc;
	ULONGLONG				TargetBackup;
	ULONGLONG				TargetBackup_x64;
	ULONGLONG				HookCopy;
	ULONG					EntrySize;
	void*					Trampoline;
	ULONG					HLSIndex;
	ULONG					HLSIdent;
	void*					Callback;
	HOOK_ACL				LocalACL;
	ULONG					Signature;
	TRACED_HOOK_HANDLE		Tracking;

	void*					RandomValue;
	void*					HookIntro;
	void*					OldProc;
	void*					HookProc;
	void*					HookOutro;
	int*					IsExecutedPtr;
} LOCAL_HOOK_INFO, *PLOCAL_HOOK_INFO;

