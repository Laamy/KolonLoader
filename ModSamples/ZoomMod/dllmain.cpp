#include <Windows.h>

#include <Kalon/Console.h>
#include <Kalon/FileIO.h>
#include <Kalon/NativeCore.h>
#include <Kalon/GameConfig.h>

#pragma region MemoryUtils

// these are normally in their own file
template <typename R, typename... Args>
R CallFunc(void* func, Args... args) { return ((R(*)(Args...))func)(args...); }

#define OFFSET_FIELD(type, name, offset) \
    type name##() { \
        return *reinterpret_cast<type*>(reinterpret_cast<char*>(this) + offset); \
    } \
    void name##(type new_##name) { \
        *reinterpret_cast<type*>(reinterpret_cast<char*>(this) + offset) = new_##name; \
    }

#pragma endregion

FileIO file;

#include "IniBuilder.h"
IniBuilder config;

#include "Engine/GameCore.h"
#include "Hook/FuncHook.h"

void Init()
{
	Console::Log("ZoomMod", "Initializing ZoomMod...");

	file.setupClientPath("ZoomMod");
	config = IniBuilder("settings.ini");

	InitHooks();
}

void Uninit()
{
	Console::Log("ZoomMod", "Uninitializing ZoomMod...");
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID)
{
	if (reason == DLL_PROCESS_ATTACH)
		Init();

	if (reason == DLL_PROCESS_DETACH)
		Uninit();

	return TRUE;
}
