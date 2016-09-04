#pragma once

#include <windows.h>


class RecursiveLock {
	CRITICAL_SECTION* pCS;
	RecursiveLock(RecursiveLock const&) = delete;
	RecursiveLock& operator=(RecursiveLock const&) = delete;
	RecursiveLock& operator=(RecursiveLock&&) = delete;
	static void* operator new(size_t count) = delete;
	static void* operator new[](size_t count) = delete;
	RecursiveLock* operator&() = delete;
public:
	RecursiveLock(CRITICAL_SECTION* pCS) : pCS(pCS) {
		EnterCriticalSection(pCS);
	}
	RecursiveLock(RecursiveLock&& old) noexcept : pCS(old.pCS) {
		old.pCS = nullptr;
	}
	~RecursiveLock() {
		if (pCS) {
			LeaveCriticalSection(pCS);
		}
	}
};
