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

- `static uintptr_t FetchOffset(const char* sidId, const char* sig)` - if the same sigid is used by 2 seperate mods then the second mod will use the cache
- `static bool HookFunction(uintptr_t address, void* hook, void** original)`
- `static bool UnhookFunction(uintptr_t address)`

## GameConfig (STATIC)

*Useful tools*

- `static HWND GetCurrentFrameWindow()` - The ApplicationFrameWindow/host assigned to the current mc process
- `static HWND GetCoreWindow()` - extract the renderwindow from the apphost
- `static std::string GetExecutablePath()`
- `static std::string GetGameVersion()` - 1.x.x (no preview numbers)
- `static std::wstring GetFamilyName()` - gets the current games family name
- `static std::string GetRoamingState()`

## SystemConfig (STATIC)

- `static std::string GetOS()`
