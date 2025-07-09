# Screenshots

Your mods load with minecraft (AND minecraft will wait until your done signature scanning and hooking)</br>
allows quick & safe modding built into minecraft
![image](https://github.com/user-attachments/assets/9728d750-ca40-44bd-b9ab-5e3aba2b761e)
![image](https://github.com/user-attachments/assets/2fcc8d24-8e46-4b87-83b6-c02117faaff1)

# How to install

go to releases and download the winhttp.dll & drop it next to your minecraft .exe and your done

TUTORIAL: https://youtu.be/FrdvPxKTYr8

# Features

Note that you don't actually have to use *any* of these and can just use your own (but it is recommended for compatibility with other mods).

Currently it's mostly the bare minimum:

## KalonLoader misc features

- Minecraft will freeze up on 33% loaded until all mods have fully loaded (unless you start a new thread)
- Ensures your mod will **always** load with Minecraft, similar to BepInEx
- A developer KalonLoader that will freeze the game until you attach a debugger

## ConsoleApi (STATIC)

- `static void CreateConsole(const char* title)`
- `static void Log(std::string modName, std::string format, ...)`
- `static void Log(const char* modName, const char* format, ...)`
- `static void CloseConsole()`
- `static void ClearConsole()`

## FileIOApi (INSTANCE)

*Even if it's a custom or preview install of Minecraft, Kalon will automatically figure out the correct roaming state folder for you.*

- `bool setupClientPath(std::string clientName)`
- `bool doesClientPathExist(const std::string& path)`
- `void createPath(const std::string& path)`
- `void deletePath(const std::string& path)`
- `void writeFile(const std::string& filePath, const std::string& content)`
- `std::string getRoamingStatePath()`
- `std::string getClientPath()`
- `std::string readFile(const std::string& filePath, bool noError = false)`

## NativeCore (STATIC)

*Just compatibility stuff for mods that use the same functions.*

- `static uintptr_t FetchOffset(const char* sidId, const char* sig)`
- `static bool HookFunction(uintptr_t address, void* hook, void** original)`
- `static bool UnhookFunction(uintptr_t address)`
