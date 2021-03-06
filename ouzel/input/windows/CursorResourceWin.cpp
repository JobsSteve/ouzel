// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "CursorResourceWin.h"
#include "utils/Log.h"

namespace ouzel
{
    namespace input
    {
        CursorResourceWin::~CursorResourceWin()
        {
            if (cursor && !shared) DestroyCursor(cursor);
        }

        bool CursorResourceWin::upload()
        {
            std::lock_guard<std::mutex> lock(uploadMutex);

            if (dirty)
            {
                if (cursor)
                {
                    if (!shared) DestroyCursor(cursor);
                    cursor = 0;
                }

                if (data.empty())
                {
                    switch (systemCursor)
                    {
                        case SystemCursor::DEFAULT:
                            cursor = LoadCursor(nullptr, IDC_ARROW);
                            break;
                        case SystemCursor::ARROW:
                            cursor = LoadCursor(nullptr, IDC_ARROW);
                            break;
                        case SystemCursor::HAND:
                            cursor = LoadCursor(nullptr, IDC_HAND);
                            break;
                        case SystemCursor::HORIZONTAL_RESIZE:
                            cursor = LoadCursor(nullptr, IDC_SIZEWE);
                            break;
                        case SystemCursor::VERTICAL_RESIZE:
                            cursor = LoadCursor(nullptr, IDC_SIZENS);
                            break;
                        case SystemCursor::CROSS:
                            cursor = LoadCursor(nullptr, IDC_CROSS);
                            break;
                        case SystemCursor::I_BEAM:
                            cursor = LoadCursor(nullptr, IDC_IBEAM);
                            break;
                    }

                    if (!cursor)
                    {
                        Log(Log::Level::ERR) << "Failed to load cursor";
                        return false;
                    }

                    shared = true;
                }
                else
                {
                    BITMAPV5HEADER bitmapHeader;
                    ZeroMemory(&bitmapHeader, sizeof(bitmapHeader));
                    bitmapHeader.bV5Size = sizeof(BITMAPV5HEADER);
                    bitmapHeader.bV5Width = static_cast<LONG>(size.v[0]);
                    bitmapHeader.bV5Height = -static_cast<LONG>(size.v[1]);
                    bitmapHeader.bV5Planes = 1;
                    bitmapHeader.bV5BitCount = 32;
                    bitmapHeader.bV5Compression = BI_BITFIELDS;
                    bitmapHeader.bV5RedMask = 0x00ff0000;
                    bitmapHeader.bV5GreenMask = 0x000ff00;
                    bitmapHeader.bV5BlueMask = 0x000000ff;
                    bitmapHeader.bV5AlphaMask = 0xff000000;

                    HDC dc = GetDC(nullptr);
                    unsigned char* target = nullptr;
                    HBITMAP color = CreateDIBSection(dc,
                                                     reinterpret_cast<BITMAPINFO*>(&bitmapHeader),
                                                     DIB_RGB_COLORS,
                                                     reinterpret_cast<void**>(&target),
                                                     NULL,
                                                     static_cast<DWORD>(0));
                    ReleaseDC(nullptr, dc);

                    if (!color)
                    {
                        Log(Log::Level::ERR) << "Failed to create RGBA bitmap";
                        return false;
                    }

                    HBITMAP mask = CreateBitmap(static_cast<LONG>(size.v[0]),
                                                static_cast<LONG>(size.v[1]),
                                                1, 1, nullptr);
                    if (!mask)
                    {
                        Log(Log::Level::ERR) << "Failed to create mask bitmap";
                        DeleteObject(color);
                        return false;
                    }

                    for (int i = 0; i < static_cast<int>(size.v[0]) * static_cast<int>(size.v[1]); i++)
                    {
                        target[i * 4 + 0] = data[i * 4 + 2];
                        target[i * 4 + 1] = data[i * 4 + 1];
                        target[i * 4 + 2] = data[i * 4 + 0];
                        target[i * 4 + 3] = data[i * 4 + 3];
                    }

                    ICONINFO iconInfo;
                    ZeroMemory(&iconInfo, sizeof(iconInfo));
                    iconInfo.fIcon = FALSE;
                    iconInfo.xHotspot = static_cast<DWORD>(hotSpot.v[0]);
                    iconInfo.yHotspot = static_cast<int>(size.v[1]) - static_cast<DWORD>(hotSpot.v[1]) - 1;
                    iconInfo.hbmMask = mask;
                    iconInfo.hbmColor = color;

                    cursor = CreateIconIndirect(&iconInfo);

                    DeleteObject(color);
                    DeleteObject(mask);

                    if (!cursor)
                    {
                        Log(Log::Level::ERR) << "Failed to create cursor";
                        return false;
                    }

                    shared = false;
                }

                dirty = false;
            }

            return true;
        }
    } // namespace input
} // namespace ouzel
