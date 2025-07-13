#include <Windows.h>

#include <Kalon/Console.h>
#include <Kalon/FileIO.h>
#include <Kalon/NativeCore.h>

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

#include "Engine/GameCore.h"
#include "Hook/FuncHook.h"
#include "InputConversion.h"

FileIO file;

void Init()
{
	Console::Log("MoreInputs", "Initializing MoreInputs...");

	file = FileIO();
	file.setupClientPath("MoreInputs");
	file.createPath("sex");

	InitHooks();
	InputConversion::Init();
}

void Uninit()
{
	Console::Log("MoreInputs", "Uninitializing MoreInputs...");
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID)
{
	if (reason == DLL_PROCESS_ATTACH)
		Init();

	if (reason == DLL_PROCESS_DETACH)
		Uninit();

	return TRUE;
}
