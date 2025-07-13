#ifndef SYSTEM_CONFIG_H
#define SYSTEM_CONFIG_H

#include <map>
#include <string>
#include <windows.h>

#ifdef KOLAN_EXPORTS
#define NATIVECORE_API __declspec(dllexport)
#else
#define NATIVECORE_API __declspec(dllimport)
#endif

class NATIVECORE_API SystemConfig {
public: // some misc things
	static std::string GetOS();
};

#endif // !SYSTEM_CONFIG_H
