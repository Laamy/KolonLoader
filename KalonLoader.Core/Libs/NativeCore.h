#ifndef NATIVE_CORE_H
#define NATIVE_CORE_H

#include <map>
#include <string>
#include <Psapi.h>

#ifdef KOLAN_EXPORTS
#define NATIVE_API __declspec(dllexport)
#else
#define NATIVE_API __declspec(dllimport)
#endif

class NATIVE_API NativeOffset {
public:
	uintptr_t detourPtr = 0;
	uintptr_t original = 0;
	bool hooked = false;
};

class NATIVE_API NativeCore {
	// Class::Func+Offset(If any)|(overload number)
	// Class::Func (no offset or overload)
	// Class::Func+1|1 (1 byte into the function & second overload)
    NativeOffset FetchOffset(const char* sidId, const char* sig);
	uintptr_t findSig_MS(const char* sig);
	bool HookFunction(void* pTarget, void* pDetour, void* pOriginal);
}

#endif // !NATIVE_CORE_H
