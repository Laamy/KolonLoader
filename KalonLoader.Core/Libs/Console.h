#pragma once

#include <Windows.h>
#include <cstdio>
#include <string>

class __declspec(dllexport) Console {
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