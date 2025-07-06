#include <Windows.h>
#include <thread>
#include <iostream>
#include <fstream>
#include <filesystem>

#include "Libs/Console.h"
#include "Libs/FileIO.h"

#include "Config.h"

#pragma region Important Exports

using _D3D11CreateDevice = HRESULT(WINAPI*)(void*, void*, UINT, const void*, UINT, UINT, void**);
using _D3D11CreateDeviceAndSwapChain = HRESULT(WINAPI*)(void*, void*, UINT, const void*, UINT, UINT, void*, void**, void**, void**);
using _D3D11On12CreateDevice = HRESULT(WINAPI*)(void*, UINT, const void*, UINT, void*, UINT, void**, void**, void**);

HMODULE d3d11Mod = nullptr;

extern "C" {
	__declspec(dllexport) _D3D11CreateDevice* D3D11CreateDevice = nullptr;
	__declspec(dllexport) _D3D11CreateDeviceAndSwapChain* D3D11CreateDeviceAndSwapChain = nullptr;
	__declspec(dllexport) _D3D11On12CreateDevice* D3D11On12CreateDevice = nullptr;
}

#pragma endregion

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

	// setup the real module cuz thats important i guess
	d3d11Mod = LoadLibraryW(L"C:\\Windows\\System32\\d3d11.dll");
	D3D11CreateDevice = (_D3D11CreateDevice*)GetProcAddress(d3d11Mod, "D3D11CreateDevice");
	D3D11CreateDeviceAndSwapChain = (_D3D11CreateDeviceAndSwapChain*)GetProcAddress(d3d11Mod, "D3D11CreateDeviceAndSwapChain");
	D3D11On12CreateDevice = (_D3D11On12CreateDevice*)GetProcAddress(d3d11Mod, "D3D11On12CreateDevice");
	Console::Log(Config::Name.c_str(), "Restored D3D11 changes");

	Config::LogInfo();

	// mod loadin crap
	{
		Console::Log(Config::Name.c_str(), "Searching for mods folder");

		FileIO::createPath("mods");
		auto modsPath = FileIO::getClientPath() + "mods";
		Console::Log(Config::Name.c_str(), "Loading mods from: %s", modsPath.c_str());

		auto modLibs = std::vector<std::string>();
		for (const auto& entry : std::filesystem::directory_iterator(modsPath)) {
			if (entry.is_regular_file() && entry.path().extension() == ".dll") {
				modLibs.push_back(entry.path().string());
			}
		}

		Console::Log(Config::Name.c_str(), "Found %d mods", modLibs.size());
		for (const auto& mod : modLibs) {
			Console::Log(Config::Name.c_str(), "Loading mod: %ls", mod.c_str());
			HMODULE modHandle = LoadLibraryA(mod.c_str());
			if (!modHandle)
				Console::Log(Config::Name.c_str(), "ERROR: Failed to load mod: %ls", mod.c_str());
		}
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
