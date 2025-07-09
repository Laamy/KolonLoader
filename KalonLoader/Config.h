#include <string>

#ifndef CONFIG_H
#define CONFIG_H

std::string GetOS() {
	auto CheckVersion = [](DWORD major, DWORD minor, DWORD build = 0, bool checkBuild = false) {
		OSVERSIONINFOEXW info{};
		info.dwOSVersionInfoSize = sizeof(info);
		info.dwMajorVersion = major;
		info.dwMinorVersion = minor;
		info.dwBuildNumber = build;

		ULONGLONG mask = VerSetConditionMask(
			VerSetConditionMask(0, VER_MAJORVERSION, VER_EQUAL),
			VER_MINORVERSION, VER_EQUAL);

		DWORD typeMask = VER_MAJORVERSION | VER_MINORVERSION;

		if (checkBuild) {
			mask = VerSetConditionMask(mask, VER_BUILDNUMBER, VER_GREATER_EQUAL);
			typeMask |= VER_BUILDNUMBER;
		}

		return VerifyVersionInfoW(&info, typeMask, mask) != FALSE;
	};

	if (CheckVersion(10, 0, 22000, true)) return "Windows 11";
	if (CheckVersion(10, 0)) return "Windows 10";

	// anything under here has no support from kolan (red flags if logs contain these)
	if (CheckVersion(6, 3)) return "Windows 8.1";
	if (CheckVersion(6, 2)) return "Windows 8";
	if (CheckVersion(6, 1)) return "Windows 7";
	if (CheckVersion(6, 0)) return "Windows Vista";
	if (CheckVersion(5, 1)) return "Windows XP";

	return "Unknown";
}

std::string GetGameVersion() {
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

// FOR PEOPLE WHO ARE CURIOUS:
// found the path in cheat engine in a live mc grabbed the static address (cuz there was only 1) threw it into ida and grabbed the sig to the first xref
#include "Utils.h"
std::string GetRoamingState() {
	static const auto pattern = findSig_MS("48 8D 05 ? ? ? ? 48 89 5C 24 ? 48 89 44 24 ? 48 8B 05");
	static std::string cache;

	if (!cache.empty())
		return cache;

	auto offset = *reinterpret_cast<const int32_t*>(pattern + 3);
	auto base = pattern + offset + 7;

	cache = std::string(reinterpret_cast<const char*>(base));
	cache = cache.substr(0, cache.size() - strlen("LocalState/")) + "RoamingState/";

	return cache;
}

namespace Config {
	std::string Name = "KolanLoader";
	std::string Version = "v0.1.2 Open-Client";

	inline void LogInfo() {
		Console::Log(Name.c_str(), "------------------------------");
		Console::Log(Name.c_str(), Name + " " + Version);
		Console::Log(Name.c_str(), "OS: " + GetOS());
		Console::Log(Name.c_str(), "------------------------------");
		Console::Log(Name.c_str(), "Game Arch: x" + std::to_string(sizeof(void*) * 8)); // probably wont ever use
		Console::Log(Name.c_str(), "Game Version: v" + GetGameVersion());
		Console::Log(Name.c_str(), "Roaming State: " + GetRoamingState());
		Console::Log(Name.c_str(), "------------------------------");
	}
}

#endif // !CONFIG_H
