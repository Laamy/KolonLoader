#ifndef INPUT_CONVERSION_H
#define INPUT_CONVERSION_H

#include <thread>
#include <GL/gl.h>

#pragma comment(lib, "opengl32.lib")

HINSTANCE g_hInstance;

class InputConversion {
private:
    static void RenderLoop() {


        auto window = NativeCore::GetCoreWindow();
        SetParent(window, nullptr);

    }

public:
	static void Init() {
        std::thread renderThread(RenderLoop);
		renderThread.detach();
    }
};

#endif // !INPUT_CONVERSION_H
