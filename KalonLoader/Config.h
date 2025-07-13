#ifndef CONFIG_H
#define CONFIG_H

#include <string>

#include "Libs/KalonLoader/GameConfig.h"
#include "Libs/KalonLoader/SystemConfig.h"

// FOR PEOPLE WHO ARE CURIOUS:
// found the path in cheat engine in a live mc grabbed the static address (cuz there was only 1) threw it into ida and grabbed the sig to the first xref
// you could also parse near the executable for the file that contains the name but thats alot of code for something that takes 2 seconds to make a permanment sig
std::string GetRoamingState() {
	static const auto pattern = NativeCore::FetchOffset("unk::GetLocalState", "48 8D 05 ? ? ? ? 48 89 5C 24 ? 48 89 44 24 ? 48 8B 05");
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
	std::string Version = "v0.1.6 Open-Client";

	inline void LogInfo() {
		Console::Log(Name.c_str(), "------------------------------");
		Console::Log(Name.c_str(), Name + " " + Version);
		Console::Log(Name.c_str(), "OS: " + SystemConfig::GetOS());
		Console::Log(Name.c_str(), "------------------------------");
		Console::Log(Name.c_str(), "Game Arch: x" + std::to_string(sizeof(void*) * 8)); // probably wont ever use
		Console::Log(Name.c_str(), "Game Version: v" + GameConfig::GetGameVersion());
		Console::Log(Name.c_str(), "Roaming State: " + GetRoamingState());
		Console::Log(Name.c_str(), "------------------------------");
	}
}

#endif // !CONFIG_H
