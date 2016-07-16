#pragma once

#include <windows.h>

#include "RecursiveLock.h"


class RecursiveMutex {
	CRITICAL_SECTION cs;
	RecursiveMutex(RecursiveMutex const&) = delete;
	RecursiveMutex(RecursiveMutex&&) = delete;
	RecursiveMutex& operator=(RecursiveMutex const&) = delete;
	RecursiveMutex& operator=(RecursiveMutex&&) = delete;
	static void* operator new(size_t count);
	static void* operator new[](size_t count);
public:
	RecursiveMutex() {
		InitializeCriticalSection(&cs);
	}
	~RecursiveMutex() {
		// DeleteCriticalSection(&cs);
	}
	RecursiveLock getLock() {
		return RecursiveLock(&cs);
	}
};
