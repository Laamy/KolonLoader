
#ifndef CONFIG_H
#define CONFIG_H

#include <string>

#include "Libs/KalonLoader/GameConfig.h"
#include "Libs/KalonLoader/SystemConfig.h"

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
