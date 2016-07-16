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

static void* hookAndGetOrig(void* orig, void* impl) {
	auto lock = globalMutex.getLock();
	
	if (fpIndex >= MAX_HOOK_COUNT) return orig;
	if (fpSet.count(orig)) return orig;
	
	TRACED_HOOK_HANDLE thh = &htiArray[fpIndex];
	thh->Link = NULL;
	
	NTSTATUS isFailed = _LhInstallHook(orig, impl, NULL, thh);
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
		*newOrig = target;
		BOOL isSucceeded = Mhook_SetHook(newOrig, impl);
		if (!isSucceeded) return;
		mhookSet.insert(newOrig);
	}
	changeMap[target] = dummy;
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
}
