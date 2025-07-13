#ifndef NATIVE_CORE_H
#define NATIVE_CORE_H

#include <map>
#include <string>
#include <windows.h>
#include <vector>
#include <unordered_map>

#ifdef KOLAN_EXPORTS
#define NATIVECORE_API __declspec(dllexport)
#else
#define NATIVECORE_API __declspec(dllimport)
#endif

using GenericHook = bool(*)(void* retPtr, std::vector<void*> args);

class NATIVECORE_API NativeCore {
private:
	static std::map<std::string, uintptr_t> m_Offsets;
	
public: // essential
	static uintptr_t FetchOffset(const char* sidId, const char* sig);

	// HookFunction so minhook isnt essential to install into mods (also i want to multiplex then later!)
	static bool HookFunction(uintptr_t address, void* hook, void** original);
	static bool UnhookFunction(uintptr_t address);
};

#endif // !NATIVE_CORE_H
