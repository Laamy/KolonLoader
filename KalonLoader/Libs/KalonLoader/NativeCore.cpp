#include "NativeCore.h"

#include "../minhook/minhook.h"

#include "Console.h"
#include "../../Utils.h"

std::map<std::string, uintptr_t> NativeCore::m_Offsets = {};

inline uintptr_t NATIVECORE_API NativeCore::FetchOffset(const char* sidId, const char* sig) {
	auto it = m_Offsets.find(sidId);

	if (it != m_Offsets.end())
		return it->second;

	uintptr_t sigResult = findSig_MS(sig);
	if (sigResult == NULL)
	{
		Console::Log("NativeCore", "Failed to find signature for %s (It's possible someone didnt name their sig properly so kolan cant find it)", sidId);
		return {};
	}

	m_Offsets[sidId] = sigResult;
	return sigResult;
}

std::unordered_map<uintptr_t, int> hookIds{};
// HookFunction so minhook isnt essential to install into mods (also i want to multiplex them later)
inline bool NATIVECORE_API NativeCore::HookFunction(uintptr_t address, void* hook, void** original) {
	static bool initialized = false;
	if (!initialized) {
		MH_Initialize(); // wont confirm this because kiero and other crap can initialize it before us (doubtful though)
		initialized = true;
	}

	if (hookIds.find(address) == hookIds.end())
		hookIds[address] = 0;

	auto hookID = hookIds[address]++;

	if (MH_CreateHookEx(hookID, reinterpret_cast<LPVOID>(address), hook, original) != MH_OK) {
		Console::Log("NativeCore", "Failed to create hook for address: 0x%p", address);
		return false;
	}

	if (MH_EnableHookEx(hookID, reinterpret_cast<LPVOID>(address)) != MH_OK) {
		Console::Log("NativeCore", "Failed to enable hook for address: 0x%p", address);
		return false;
	}

	return true;
}

inline bool NATIVECORE_API NativeCore::UnhookFunction(uintptr_t address) {
	if (MH_DisableHook(reinterpret_cast<LPVOID>(address)) != MH_OK) {
		Console::Log("NativeCore", "Failed to disable hook for address: 0x%p", address);
		return false;
	}

	if (MH_RemoveHook(reinterpret_cast<LPVOID>(address)) != MH_OK) {
		Console::Log("NativeCore", "Failed to remove hook for address: 0x%p", address);
		return false;
	}

	return true;
}
