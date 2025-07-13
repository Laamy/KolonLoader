#ifndef GAME_CONFIG_H
#define GAME_CONFIG_H

#include <map>
#include <string>
#include <windows.h>
#include <vector>
#include <filesystem>
#include <shlobj.h>
#include <shlwapi.h>
#include <appmodel.h>
#include <iostream>

#ifdef KOLAN_EXPORTS
#define NATIVECORE_API __declspec(dllexport)
#else
#define NATIVECORE_API __declspec(dllimport)
#endif

class NATIVECORE_API GameConfig {
public: // some misc things
	static HWND GetCurrentFrameWindow();
	static HWND GetCoreWindow();

	static std::string GetExecutablePath();
	static std::string GetGameVersion();
	static std::wstring GetFamilyName();
	static std::string GetRoamingState();
};

#endif // !GAME_CONFIG_H
