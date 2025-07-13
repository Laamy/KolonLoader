#include "NativeCore.h"

#include "../minhook/minhook.h"

#include "Console.h"
#include "../../Utils.h"

std::map<std::string, uintptr_t> NativeCore::g_Offsets = {};

inline uintptr_t NATIVECORE_API NativeCore::FetchOffset(const char* sidId, const char* sig) {
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

// HookFunction so minhook isnt essential to install into mods (also i want to multiplex them later)
inline bool NATIVECORE_API NativeCore::HookFunction(uintptr_t address, void* hook, void** original) {
	static bool initialized = false;
	if (!initialized) {
		MH_Initialize(); // wont confirm this because kiero and other crap can initialize it before us (doubtful though)
		initialized = true;
	}

	if (MH_CreateHook(reinterpret_cast<LPVOID>(address), hook, original) != MH_OK) {
		Console::Log("NativeCore", "Failed to create hook for address: 0x%p", address);
		return false;
	}

	if (MH_EnableHook(reinterpret_cast<LPVOID>(address)) != MH_OK) {
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

// some misc things
inline HWND NATIVECORE_API NativeCore::GetCurrentFrameWindow() {
	DWORD currentPid = GetCurrentProcessId();
	HWND hwnd = nullptr;
	HWND result = nullptr;

	while ((hwnd = FindWindowEx(nullptr, hwnd, L"ApplicationFrameWindow", nullptr))) {
		DWORD pid = 0;
		GetWindowThreadProcessId(hwnd, &pid);
		if (pid == 0)
			continue;

		HWND child = FindWindowEx(hwnd, nullptr, nullptr, nullptr);
		while (child) {
			DWORD childPid = 0;
			GetWindowThreadProcessId(child, &childPid);
			if (childPid == currentPid) {
				result = hwnd;
				break;
			}
			child = FindWindowEx(hwnd, child, nullptr, nullptr);
		}
		if (result)
			break;
	}

	return result;
}

inline HWND _GetCoreWindow(HWND root) {
	HWND child = nullptr;
	while ((child = FindWindowEx(root, child, nullptr, nullptr))) {
		wchar_t cls[256];
		GetClassNameW(child, cls, 256);
		if (wcsstr(cls, L"Windows.UI.Core.CoreWindow") || wcsstr(cls, L"ApplicationViewWindow")) {
			return child;
		}
		HWND deeper = _GetCoreWindow(child);
		if (deeper) return deeper;
	}
	return nullptr;
}

inline HWND NATIVECORE_API NativeCore::GetCoreWindow()
{
	return _GetCoreWindow(GetCurrentFrameWindow());
}

std::string NativeCore::GetExecutablePath()
{
	char path[MAX_PATH];
	HMODULE hModule = nullptr;
	GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
		reinterpret_cast<LPCSTR>(&GetExecutablePath),
		&hModule);
	GetModuleFileNameA(hModule, path, MAX_PATH);
	std::string fullPath(path);
	size_t pos = fullPath.find_last_of("\\/");
	return fullPath.substr(0, pos);
}
