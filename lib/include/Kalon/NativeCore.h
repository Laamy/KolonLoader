#include <map>
#include <string>

#ifndef NATIVE_CORE_H
#define NATIVE_CORE_H

#ifdef KOLAN_EXPORTS
#define NATIVECORE_API __declspec(dllexport)
#else
#define NATIVECORE_API __declspec(dllimport)
#endif

class NATIVECORE_API NativeCore {
private:
	static std::map<std::string, uintptr_t> g_Offsets;

public:
	static uintptr_t FetchOffset(const char* sidId, const char* sig);

	// HookFunction so minhook isnt essential to install into mods (also i want to multiplex then later!)
	static bool HookFunction(uintptr_t address, void* hook, void** original);
	static bool UnhookFunction(uintptr_t address);
};

#endif // !NATIVE_CORE_H
