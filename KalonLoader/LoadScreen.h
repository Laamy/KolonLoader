#include <string>
#include <mutex>
#include <queue>
#include <functional>

#define STB_IMAGE_IMPLEMENTATION
#include "Libs/stb_image.h"

#ifndef LOAD_SCREEN_H
#define LOAD_SCREEN_H

HBITMAP LoadPngAsBitmap(const char* path, int& outWidth, int& outHeight) {
    int width, height, channels;
    unsigned char* data = stbi_load(path, &width, &height, &channels, 4);
    if (!data)
        return NULL;

    for (int i = 0; i < width * height; ++i)
        std::swap(data[i * 4 + 0], data[i * 4 + 2]);

    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = width;
    bmi.bmiHeader.biHeight = -height;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    void* bits = nullptr;
    HDC hdc = GetDC(NULL);
    HBITMAP hBitmap = CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, &bits, NULL, 0);
    ReleaseDC(NULL, hdc);

    if (!hBitmap) {
        stbi_image_free(data);
        return NULL;
    }

    memcpy(bits, data, width * height * 4);
    stbi_image_free(data);

    outWidth = width;
    outHeight = height;
    return hBitmap;
}

class LoadScreen {
private:
    static HWND g_hwnd;
    static std::queue<std::function<void()>> g_tasks;
    static bool g_running;
    static std::string g_status;

    static HBITMAP hBitmap;
    static int width, height;

    static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
        static HBRUSH bgBrush = nullptr;
        if (msg == WM_CREATE)
            bgBrush = CreateSolidBrush(RGB(0xEF, 0x32, 0x3D)); // might read the appx manifest 4 dis

        if (msg == WM_PAINT) {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            RECT rect;
            GetClientRect(hwnd, &rect);
            FillRect(hdc, &rect, bgBrush);

            if (hBitmap) {
                HDC memDC = CreateCompatibleDC(hdc);
                if (!memDC)
                    return 0;

                HBITMAP oldBmp = (HBITMAP)SelectObject(memDC, hBitmap);
                if (!oldBmp) {
                    DeleteDC(memDC);
                    return 0;
                }

                BitBlt(hdc, (rect.right - rect.left) / 2 - (width / 2), 15, width, height, memDC, 0, 0, SRCCOPY);

                SelectObject(memDC, oldBmp);
                DeleteDC(memDC);
            }

            if (!g_status.empty()) {
                std::wstring wstatus(g_status.begin(), g_status.end());
                rect.top += 25;
                rect.bottom += 25;
                SetTextColor(hdc, RGB(255, 255, 255));
                SetBkMode(hdc, TRANSPARENT);
                DrawTextW(hdc, wstatus.c_str(), -1, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
            }

            EndPaint(hwnd, &ps);

            if (!g_tasks.empty()) {
                auto task = g_tasks.front();
                g_tasks.pop();
                task();
                InvalidateRect(hwnd, nullptr, FALSE);
            }
            else {
                g_running = false;
                PostMessage(hwnd, WM_CLOSE, 0, 0);
            }
            return 0;
        }
        if (msg == WM_DESTROY) {
            PostQuitMessage(0);
            return 0;
        }
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }

public:
    static void SetupTask(std::function<void()> task) {
        g_tasks.push(task);
    }

    static void Launch() {
        g_running = true;
        g_status.clear();

        //ShowWindow(GetConsoleWindow(), SW_HIDE);

        char path[MAX_PATH];
        HMODULE hModule = nullptr;
        GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
            reinterpret_cast<LPCSTR>(&Launch),
            &hModule);
        GetModuleFileNameA(hModule, path, MAX_PATH);
        std::string fullPath(path);
        size_t pos = fullPath.find_last_of("\\/");
        std::string imgPath = fullPath.substr(0, pos) + "\\UAP.Assets\\minecraft\\icons\\Logo.scale-100.png";

        if (hBitmap == NULL)
            hBitmap = LoadPngAsBitmap(imgPath.c_str(), width, height);

        WNDCLASS wc{};
        wc.lpfnWndProc = WndProc;
        wc.hInstance = GetModuleHandle(nullptr);
        wc.lpszClassName = L"LoadScreenClass";

        RegisterClass(&wc);

        const int width = 300;
        const int height = 150;
        int screenX = GetSystemMetrics(SM_CXSCREEN);
        int screenY = GetSystemMetrics(SM_CYSCREEN);
        int x = (screenX - width) / 2;
        int y = (screenY - height) / 2;

        g_hwnd = CreateWindowEx(
            0,
            wc.lpszClassName,
            nullptr,
            WS_POPUP,
            x, y, width, height,
            nullptr, nullptr, wc.hInstance, nullptr);

        ShowWindow(g_hwnd, SW_SHOW);

        MSG msg{};
        while (g_running && GetMessage(&msg, nullptr, 0, 0) > 0) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        Sleep(250);
        DestroyWindow(g_hwnd);
        DeleteObject(hBitmap);
    }

    static void SetStatusText(const std::string& status) {
        g_status = status;
        InvalidateRect(g_hwnd, nullptr, FALSE);
    }
};

HWND LoadScreen::g_hwnd = nullptr;
std::queue<std::function<void()>> LoadScreen::g_tasks;
bool LoadScreen::g_running = false;
std::string LoadScreen::g_status;

HBITMAP LoadScreen::hBitmap = NULL;
int LoadScreen::width = NULL;
int LoadScreen::height = NULL;

#endif // !LOAD_SCREEN_H
