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

	std::vector<char> data(size);
	if (!GetFileVersionInfoA(path, handle, size, data.data())) return {};

	VS_FIXEDFILEINFO* fileInfo = nullptr;
	UINT len = 0;
	if (!VerQueryValueA(data.data(), "\\", reinterpret_cast<void**>(&fileInfo), &len)) return {};

	if (!fileInfo) return {};

	WORD major = HIWORD(fileInfo->dwFileVersionMS);
	WORD minor = LOWORD(fileInfo->dwFileVersionMS);	
	WORD patch = HIWORD(fileInfo->dwFileVersionLS);
	return std::to_string(major) + "." + std::to_string(minor) + "." + std::to_string(patch);
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
		Console::Log(Name.c_str(), "------------------------------");
	}
}

#endif // !CONFIG_H
