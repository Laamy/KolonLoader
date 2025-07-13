#include "LoadScreen.h"

#define STB_IMAGE_IMPLEMENTATION
#include <iostream>
#include <TlHelp32.h>
#include <chrono>
#include <dwmapi.h>

#pragma comment(lib, "dwmapi.lib")

#include "Libs/KalonLoader/GameConfig.h"
#include "Utils/BitmapUtils.h"

void ApplyDwmStyle(HWND hwnd) {
    DWORD pref = DWMWCP_ROUND;
	DwmSetWindowAttribute(hwnd, DWMWA_WINDOW_CORNER_PREFERENCE, &pref, sizeof(pref));

    MARGINS shadow = { -1 };
	DwmExtendFrameIntoClientArea(hwnd, &shadow);
}

HWND LoadScreen::g_hwnd = nullptr;
bool LoadScreen::g_running = false;
std::queue<std::function<void()>> LoadScreen::g_tasks;
std::string LoadScreen::g_status;

HBITMAP LoadScreen::hBitmap = NULL;
int LoadScreen::width = NULL;
int LoadScreen::height = NULL;

float GetDeltaTime() {
    static auto last = std::chrono::high_resolution_clock::now();
	auto now = std::chrono::high_resolution_clock::now();
	std::chrono::duration<float> delta = now - last;
	last = now;
	return delta.count();
}

static float pos = -24.f;
static float dir = 1.f;

inline LRESULT LoadScreen::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    static HBRUSH bgBrush = nullptr;
    if (msg == WM_CREATE)
        bgBrush = CreateSolidBrush(RGB(0xEF, 0x32, 0x3D)); // might read the appx manifest 4 dis

    if (msg == WM_PAINT) {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        RECT rect;
        GetClientRect(hwnd, &rect);

        HDC backDC = CreateCompatibleDC(hdc);
        HBITMAP backBmp = CreateCompatibleBitmap(hdc, rect.right - rect.left, rect.bottom - rect.top);
        HBITMAP oldBack = (HBITMAP)SelectObject(backDC, backBmp);

        FillRect(backDC, &rect, bgBrush);

        float delta = GetDeltaTime();
        pos += delta * 12.f * dir;
        if (pos >= 24.f) {
            pos = 24.f;
            dir = -1.f;
        }
        if (pos <= -24.f) {
            pos = -24.f;
            dir = 1.f;
        }
        std::cout << pos << std::endl;

        if (hBitmap) {
            HDC memDC = CreateCompatibleDC(backDC);
            if (memDC) {
                HBITMAP oldBmp = (HBITMAP)SelectObject(memDC, hBitmap);
                if (oldBmp) {
                    BitBlt(backDC,
                        (rect.right - rect.left) / 2 - (width / 2) + pos,
                        (rect.bottom - rect.top) / 2 - (height / 2),
                        width, height, memDC, 0, 0, SRCCOPY);
                    SelectObject(memDC, oldBmp);
                }
                DeleteDC(memDC);
            }
        }

        if (!g_status.empty()) {
            std::wstring wstatus(g_status.begin(), g_status.end());
            RECT textRect = rect;
            textRect.top += 25;
            textRect.bottom += 25;
            SetTextColor(backDC, RGB(255, 255, 255));
            SetBkMode(backDC, TRANSPARENT);
            DrawTextW(backDC, wstatus.c_str(), -1, &textRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        }

        BitBlt(hdc, 0, 0, rect.right - rect.left, rect.bottom - rect.top, backDC, 0, 0, SRCCOPY);
        SelectObject(backDC, oldBack);
        DeleteObject(backBmp);
        DeleteDC(backDC);

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

void LoadScreen::SetupTask(std::function<void()> task) {
    g_tasks.push(task);
}

static void RenderLoop(HWND hwnd) {
    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(16));

        InvalidateRect(hwnd, nullptr, FALSE);
    }
}

std::vector<std::string> backgrounds = {
    "FirstPoweredBeacon-886be5f1b4fb0ab7966a.png",
    "FirstAncientCityFound-8d6da611faef146c050d.png",
    "FirstWoodlandMansionFound-a68128deae6a4b05e5d0.png",
    "FirstNetherFortressFound-5990a1bfc6a2b0d593dd.png",
    "FirstPeakMountainFound-77bba36b907a808917fb.png",
};
std::string selected = "";

void LoadScreen::Launch() {
    g_running = true;
    g_status.clear();

    srand(static_cast<unsigned int>(time(nullptr)));
	selected = backgrounds[rand() % backgrounds.size()];

    std::string imgPath = GameConfig::GetExecutablePath() + "\\data\\gui\\dist\\hbui\\assets\\" + selected;

    if (hBitmap == NULL)
        hBitmap = LoadPngAsBitmap(imgPath.c_str(), width, height);

    if (hBitmap) {
		HDC screenDC = GetDC(NULL);
		hBitmap = ScaleBitmap(hBitmap, width, height, 12, screenDC);
		ReleaseDC(NULL, screenDC);

        width *= 12;
		height *= 12;
    }

    WNDCLASS wc{};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = GetModuleHandle(nullptr);
    wc.lpszClassName = L"LoadScreenClass";

    RegisterClass(&wc);

    const int width = 300*2;
    const int height = 150*2;
    int screenX = GetSystemMetrics(SM_CXSCREEN);
    int screenY = GetSystemMetrics(SM_CYSCREEN);
    int x = (screenX - width) / 2;
    int y = (screenY - height) / 2;

    g_hwnd = CreateWindowEx(
        WS_EX_TOPMOST,
        wc.lpszClassName,
        nullptr,
        WS_POPUP,
        x, y, width, height,
        nullptr, nullptr, wc.hInstance, nullptr);
    ApplyDwmStyle(g_hwnd);

    ShowWindow(g_hwnd, SW_SHOW);

	std::thread renderThread(RenderLoop, g_hwnd);
	renderThread.detach();

    MSG msg{};
    while (g_running && GetMessage(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    Sleep(250);
    DestroyWindow(g_hwnd);
    DeleteObject(hBitmap);
}

void LoadScreen::SetStatusText(const std::string& status) {
    g_status = status;
    InvalidateRect(g_hwnd, nullptr, FALSE);
}
