#ifndef BITMAP_UTILS_H
#define BITMAP_UTILS_H

#include "../Libs/stb_image.h"

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

HBITMAP ScaleBitmap(HBITMAP hBmp, int oWidth, int oHeight, int scale, HDC hdc) {
    HDC srcDC = CreateCompatibleDC(hdc);
    HDC dstDC = CreateCompatibleDC(hdc);
    HBITMAP scaledBmp = CreateCompatibleBitmap(hdc, oWidth * scale, oHeight * scale);

    HBITMAP oldSrcBmp = (HBITMAP)SelectObject(srcDC, hBmp);
    HBITMAP oldDstBmp = (HBITMAP)SelectObject(dstDC, scaledBmp);

    StretchBlt(dstDC, 0, 0, oWidth * scale, oHeight * scale,
        srcDC, 0, 0, oWidth, oHeight, SRCCOPY);

    SelectObject(srcDC, oldSrcBmp);
    SelectObject(dstDC, oldDstBmp);
    DeleteDC(srcDC);
    DeleteDC(dstDC);

    return scaledBmp;
}

#endif // !BITMAP_UTILS_H
