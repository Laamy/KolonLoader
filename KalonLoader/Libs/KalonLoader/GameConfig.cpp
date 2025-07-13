#include "GameConfig.h"

// some misc things
inline HWND NATIVECORE_API GameConfig::GetCurrentFrameWindow() {
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

inline HWND NATIVECORE_API GameConfig::GetCoreWindow()
{
	return _GetCoreWindow(GetCurrentFrameWindow());
}

std::string GameConfig::GetExecutablePath()
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

inline std::string GameConfig::GetGameVersion() {
	char path[MAX_PATH];
	GetModuleFileNameA(0, path, MAX_PATH);

	DWORD handle = 0;
	DWORD size = GetFileVersionInfoSizeA(path, &handle);
	if (!size) return {};

	std::vector<char> buffer(size);
	if (!GetFileVersionInfoA(path, handle, size, buffer.data())) return {};

	VS_FIXEDFILEINFO* info = nullptr;
	UINT len = 0;
	if (!VerQueryValueA(buffer.data(), "\\", reinterpret_cast<void**>(&info), &len)) return {};
	if (!info) return {};

	WORD major = HIWORD(info->dwFileVersionMS);
	WORD minor = LOWORD(info->dwFileVersionMS);
	WORD patch = HIWORD(info->dwFileVersionLS);

	return std::to_string(major) + "." + std::to_string(minor) + "." + std::to_string(patch);
}
