#include "common.h"

#include <unordered_set>
#include <unordered_map>
#include "lock/RecursiveMutex.h"
#include "easyhook2.h"
#include "mhook.h"


RecursiveMutex globalMutex;

static bool isInited = false;

static std::unordered_map<void*, void*> changeMap;
static std::unordered_set<void**> mhookSet;
static std::unordered_set<void*> vtblSet;

static size_t fpIndex = 0;
static HOOK_TRACE_INFO htiArray[MAX_HOOK_COUNT];
static std::unordered_set<void*> fpSet;

extern NTSTATUS LhInstallHook2(
	void* InEntryPoint,
	void* InHookProc,
	void* InCallback,
	TRACED_HOOK_HANDLE OutHandle
) {
	static auto hModule = GetModuleHandleW(L"Kernel32.dll");
	static auto GetProcessMitigationPolicyProc = GetProcAddress(hModule, "GetProcessMitigationPolicy");
	static auto pGetProcessMitigationPolicy = (decltype(&GetProcessMitigationPolicy))GetProcessMitigationPolicyProc;
	static auto GetThreadInformationProc = GetProcAddress(hModule, "GetThreadInformation");
	static auto pGetThreadInformation = (decltype(&GetThreadInformation))GetThreadInformationProc;
	static auto SetThreadInformationProc = GetProcAddress(hModule, "SetThreadInformation");
	static auto pSetThreadInformation = (decltype(&SetThreadInformation))SetThreadInformationProc;
	
	if (pGetProcessMitigationPolicy && pGetThreadInformation && pSetThreadInformation) {
		PROCESS_MITIGATION_DYNAMIC_CODE_POLICY policy;
		BOOL isSucceeded = pGetProcessMitigationPolicy(
			GetCurrentProcess(),
			ProcessDynamicCodePolicy,
			&policy,
			sizeof(PROCESS_MITIGATION_DYNAMIC_CODE_POLICY)
		);
		
		if (!isSucceeded) {
			return -1;
		}
		
		if (policy.ProhibitDynamicCode) {
			if (!policy.AllowThreadOptOut) {
				return -1;
			}
			
			HANDLE thread = GetCurrentThread();
			DWORD prev;
			pGetThreadInformation(
				thread,
				ThreadDynamicCodePolicy,
				&prev,
				sizeof(DWORD)
			);
			DWORD policy = THREAD_DYNAMIC_CODE_ALLOW;
			pSetThreadInformation(
				thread,
				ThreadDynamicCodePolicy,
				&policy,
				sizeof(DWORD)
			);
			NTSTATUS ret = _LhInstallHook(
				InEntryPoint, InHookProc, InCallback, OutHandle
			);
			pSetThreadInformation(
				thread,
				ThreadDynamicCodePolicy,
				&prev,
				sizeof(DWORD)
			);
			return ret;
		}
	}
	return _LhInstallHook(
		InEntryPoint, InHookProc, InCallback, OutHandle
	);
}

static void* hookAndGetOrig(void* orig, void* impl) {
	auto lock = globalMutex.getLock();
	
	if (fpIndex >= MAX_HOOK_COUNT) return orig;
	if (fpSet.count(orig)) return orig;
	
	TRACED_HOOK_HANDLE thh = &htiArray[fpIndex];
	thh->Link = NULL;
	
	NTSTATUS isFailed = LhInstallHook2(orig, impl, NULL, thh);
	if (isFailed) return orig;
	
	ULONG ZERO = 0;
	_LhSetExclusiveACL(&ZERO, 0, thh);
	
	fpSet.insert(orig);
	fpIndex++;
	return thh->Link->OldProc;
}

/* dllmain.cpp */
extern void init();

extern void initIfStill() {
	auto lock = globalMutex.getLock();
	if (!isInited) {
		init();
		isInited = true;
	}
}

extern bool insertVtbl(void** vtbl) {
	auto lock = globalMutex.getLock();
	return vtblSet.insert(vtbl).second;
}

extern void hook(void* orig, void* impl) {
	hookAndGetOrig(orig, impl);
}

extern void changeHookTarget(void* target, void* dummy) {
	auto lock = globalMutex.getLock();
	changeMap[target] = dummy;
}

extern void changeHookTarget(void* target, void* dummy, void* impl, void** newOrig) {
	auto lock = globalMutex.getLock();
	*newOrig = hookAndGetOrig(target, impl);
	if (*newOrig != target) {
		changeMap[target] = dummy;
	}
}

extern void changeHookTargetWithMhook(void* target, void* dummy, void* impl, void** newOrig) {
	auto lock = globalMutex.getLock();
	*newOrig = hookAndGetOrig(target, impl);
	if (*newOrig == target) {
		BOOL isSucceeded = Mhook_SetHook(newOrig, impl);
		if (!isSucceeded) return;
		mhookSet.insert(newOrig);
	}
	changeMap[target] = dummy;
}

extern void changeHookTargetWithMhookForcibly(void* target, void* dummy, void* impl, void** newOrig) {
	auto lock = globalMutex.getLock();
	*newOrig = target;
	BOOL isSucceeded = Mhook_SetHook(newOrig, impl);
	if (isSucceeded) {
		mhookSet.insert(newOrig);
		changeMap[target] = dummy;
	}
}

extern void* changeHookTargetIfNeed(void* hookTarget) {
	auto lock = globalMutex.getLock();
	return changeMap.count(hookTarget) ? changeMap[hookTarget] : hookTarget;
}

extern void unhookAll() {
	{
		auto lock = globalMutex.getLock();
		_LhUninstallAllHooks();
		fpIndex = 0;
		fpSet.clear();
		vtblSet.clear();
		for (auto e : mhookSet) {
			Mhook_Unhook(e);
		}
		mhookSet.clear();
	}
	_LhWaitForPendingRemovals();
	{
		auto lock = globalMutex.getLock();
		changeMap.clear();
		isInited = false;
	}
}
