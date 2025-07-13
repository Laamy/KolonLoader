#ifndef GAME_CONFIG_H
#define GAME_CONFIG_H

#include <map>
#include <string>
#include <windows.h>
#include <vector>

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
};

#endif // !GAME_CONFIG_H
