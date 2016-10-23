#include "easyhook2.h"
#include "common.h"


static LOCAL_HOOK_INFO prev = {};

extern "C" __declspec(dllexport)
NTSTATUS __stdcall LhInstallHook(
	void* InEntryPoint,
	void* InHookProc,
	void* InCallback,
	TRACED_HOOK_HANDLE OutHandle
) {
	initIfStill();
	
	InEntryPoint = changeHookTargetIfNeed(InEntryPoint);
	
	NTSTATUS isFailed = LhInstallHook2(
		InEntryPoint,
		InHookProc,
		InCallback,
		OutHandle
	);
	
	if (isFailed) {
		prev.TargetProc = InEntryPoint;
		prev.OldProc = InEntryPoint;
		prev.HookProc = InHookProc;
		OutHandle->Link = &prev;
	}
	
	return STATUS_SUCCESS;
}

extern "C" __declspec(dllexport)
NTSTATUS __stdcall LhSetExclusiveACL(
	ULONG* InProcessIdList,
	ULONG InProcessCount,
	TRACED_HOOK_HANDLE InHandle
) {
	if (InHandle->Link == &prev) {
		return STATUS_SUCCESS;
	}
	return _LhSetExclusiveACL(InProcessIdList, InProcessCount, InHandle);
}

extern "C" __declspec(dllexport)
NTSTATUS __stdcall LhSetGlobalExclusiveACL(
	ULONG* InProcessIdList,
	ULONG InProcessCount
) {
	return _LhSetGlobalExclusiveACL(InProcessIdList, InProcessCount);
}

extern "C" __declspec(dllexport)
NTSTATUS __stdcall LhUninstallAllHooks() {
	unhookAll();
	return _LhUninstallAllHooks();
}

extern "C" __declspec(dllexport)
NTSTATUS __stdcall LhWaitForPendingRemovals() {
	return _LhWaitForPendingRemovals();
}
