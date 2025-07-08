#ifndef CONSOLE_H
#define CONSOLE_H

#include <Windows.h>
#include <cstdio>
#include <string>

#ifdef KOLAN_EXPORTS
#define CONSOLE_API __declspec(dllexport)
#else
#define CONSOLE_API __declspec(dllimport)
#endif

class CONSOLE_API Console {
public:
	static void CreateConsole(const char* title);

	//static void Log(std::string format, ...)
	//{
	//	va_list args;
	//	va_start(args, format);
	//	printf("[KolanLoader] ");
	//	vprintf(format.c_str(), args);
	//	va_end(args);
	//	printf("\n");
	//}

	static void Log(std::string modName, std::string format, ...);
	static void Log(const char* modName, const char* format, ...);
	static void CloseConsole();
	static void ClearConsole();

private:
	static FILE* f;
};

#endif // !CONSOLE_H
