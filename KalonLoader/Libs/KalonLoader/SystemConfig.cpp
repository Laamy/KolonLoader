#include "SystemConfig.h"

// some misc things
inline std::string NATIVECORE_API SystemConfig::GetOS() {
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
