#include <string>
#include <mutex>
#include <queue>
#include <functional>
#include <wtypes.h>

#ifndef LOAD_SCREEN_H
#define LOAD_SCREEN_H

HBITMAP LoadPngAsBitmap(const char* path, int& outWidth, int& outHeight);

// thrown together messily as a concept
// i might hide the console window & MC window until this is done
class LoadScreen {
private:
    static HWND g_hwnd;
    static std::queue<std::function<void()>> g_tasks;
    static bool g_running;
    static std::string g_status;

    static HBITMAP hBitmap;
    static int width, height;

    static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

public:
    static void SetupTask(std::function<void()> task);

    static void Launch();

    static void SetStatusText(const std::string& status);
};

#endif // !LOAD_SCREEN_H
