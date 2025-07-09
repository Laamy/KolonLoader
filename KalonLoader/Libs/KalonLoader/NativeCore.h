#include <map>
#include <string>

#ifndef NATIVE_CORE_H
#define NATIVE_CORE_H

#ifdef KOLAN_EXPORTS
#define NATIVECORE_API __declspec(dllexport)
#else
#define NATIVECORE_API __declspec(dllimport)
#endif

namespace NativeCore {
	std::map<std::string, uintptr_t> g_Offsets;

	uintptr_t NATIVECORE_API FetchOffset(const char* sidId, const char* sig) {
		auto it = g_Offsets.find(sidId);

		if (it != g_Offsets.end())
			return it->second;

		uintptr_t sigResult = findSig_MS(sig);
		if (sigResult == NULL)
		{
			Console::Log("NativeCore", "Failed to find signature for %s (It's possible someone didnt name their sig properly so kolan cant find it)", sidId);
			return {};
		}

		g_Offsets[sidId] = sigResult;
		return sigResult;
	}
}

#endif // !NATIVE_CORE_H
