#include <Windows.h>
#include <thread>
#include <iostream>
#include <fstream>
#include <filesystem>

#pragma region QuickDLLProxy

#include "Libs/QuickDllProxy/DllProxy.h"

void* ResolveModule()
{
	return LoadLibraryA("C:\\windows\\system32\\winhttp.dll");
}

void ExampleHookedFunction()
{
}

bool ResolveFunction(void* Module, uint32_t Ordinal, const char* Name, void** Pointer)
{
	// Intercept a specific ordinal
	if (Ordinal == 23)
	{
		*Pointer = &ExampleHookedFunction;
		return true;
	}

	// Otherwise resolve it as normal through GetProcAddress
	auto ptr = GetProcAddress((HMODULE)Module, Name);

	if (!ptr)
		return false;

	*Pointer = ptr;
	return true;
}

void HandleException(DllProxy::ErrorCode Code)
{
	char reason[32];
	sprintf_s(reason, "Error code: %u", Code);

	MessageBoxA(nullptr, reason, "PROXY ERROR", MB_ICONERROR);
	__debugbreak();
}

#define DLL_PROXY_EXPORT_LISTING_FILE "winhttp_exports.inc"
#define DLL_PROXY_CHECK_MISSING_EXPORTS
#define DLL_PROXY_LIBRARY_RESOLVER_CALLBACK ResolveModule
#define DLL_PROXY_EXPORT_RESOLVER_CALLBACK ResolveFunction
#define DLL_PROXY_EXCEPTION_CALLBACK HandleException
#define DLL_PROXY_DECLARE_IMPLEMENTATION

#pragma endregion

#include "Libs/QuickDllProxy/DllProxy.h"

#include "Libs/KalonLoader/Console.h"

#include "Libs/KalonLoader/NativeCore.h"
#include "Config.h"

#include "Libs/KalonLoader/FileIO.h"

//#define DEBUG

void Init() {
	Console::CreateConsole(Config::Name.c_str());

#ifdef DEBUG
	Console::Log(Config::Name.c_str(), "Attach debugger to continue..");
	while (!IsDebuggerPresent()) {
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
#elif !DEBUG
	Console::Log(Config::Name.c_str(), "Running without debugger attached, this is not recommended if you are a developer.");
#endif

	Console::Log(Config::Name.c_str(), "Initializing..");
	
	DllProxy::Initialize();
	Config::LogInfo();

	auto files = FileIO();
	auto modsPath = files.getRoamingStatePath() + "mods";
	std::filesystem::create_directory(modsPath);

	Console::Log(Config::Name.c_str(), "Mods folder found");
	Console::Log(Config::Name.c_str(), "Loading mods from: %s", modsPath.c_str());

	std::vector<std::string> modLibs;
	for (const auto& entry : std::filesystem::directory_iterator(modsPath)) {
		if (entry.is_regular_file() && entry.path().extension() == ".dll")
			modLibs.push_back(entry.path().string());
	}

	Console::Log(Config::Name.c_str(), "Found %d mods", modLibs.size());

	// NOTE: I might check for a registermod export so i can tell mods apart from unsupported mods so i can alert the user that the game will probably break or crash
	for (const auto& mod : modLibs) {
		Console::Log(Config::Name.c_str(), "Loading mod: %ls", mod.c_str());
		if (!LoadLibraryA(mod.c_str()))
			Console::Log(Config::Name.c_str(), "ERROR: Failed to load mod: %ls", mod.c_str());
	}
}

void Uninit() {

}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID)
{
	if (reason == DLL_PROCESS_ATTACH)
		Init();

	if (reason == DLL_PROCESS_DETACH)
		Uninit();

    return TRUE;
}
