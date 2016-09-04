#pragma once

#include <unknwn.h>
#include "lock/RecursiveMutex.h"



extern RecursiveMutex globalMutex;



extern bool insertVtbl(void** vtbl);

extern void hook(void* orig, void* impl);

extern void* changeHookTargetIfNeed(void* hookTarget);

extern void changeHookTarget(void* original, void* dummy);

extern void changeHookTarget(
	void* original,
	void* dummy,
	void* impl,
	void** newOriginal
);

extern void changeHookTargetWithMhook(
	void* original,
	void* dummy,
	void* impl,
	void** newOriginal
);

extern void changeHookTargetWithMhookForcibly(
	void* original,
	void* dummy,
	void* impl,
	void** newOriginal
);

extern void unhookAll();

extern void initIfStill();



/* direct2d.cpp */
extern void hookDirect2D();

/* directwrite.cpp */
extern void hookDirectWrite();

/* mactype.cpp */
extern void initMacTypeIntercept();



inline void** getVtbl(void* obj) {
	return *reinterpret_cast<void***>(obj);
}

template<typename COM>
inline HRESULT hookIfImplemented(IUnknown* pUnknown, void(*hookFunc)(COM*)) {
	COM* comObject;
	HRESULT hr = pUnknown->QueryInterface(&comObject);
	if (SUCCEEDED(hr)) {
		hookFunc(comObject);
		comObject->Release();
	}
	return hr;
}

template<typename T>
inline T getWithLock(T* p) {
	auto lock = globalMutex.getLock();
	return *p;
}
