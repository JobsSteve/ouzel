// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "RendererOGLMacOS.h"
#include "core/macos/WindowMacOS.h"
#include "core/Application.h"
#include "core/Engine.h"
#include "utils/Log.h"

static CVReturn renderCallback(CVDisplayLinkRef,
                               const CVTimeStamp*,
                               const CVTimeStamp*,
                               CVOptionFlags,
                               CVOptionFlags*,
                               void*)
{
    @autoreleasepool
    {
        if (ouzel::sharedEngine->isRunning() && !ouzel::sharedEngine->draw())
        {
            ouzel::sharedApplication->execute([] {
                ouzel::sharedEngine->getWindow()->close();
            });
        }
    }

    return kCVReturnSuccess;
}

namespace ouzel
{
    namespace graphics
    {
        RendererOGLMacOS::~RendererOGLMacOS()
        {
            if (displayLink)
            {
                if (CVDisplayLinkStop(displayLink) != kCVReturnSuccess)
                {
                    Log(Log::Level::ERR) << "Failed to stop display link";
                }

                CVDisplayLinkRelease(displayLink);
            }

            if (openGLContext)
            {
                [NSOpenGLContext clearCurrentContext];
                [openGLContext release];
            }

            if (pixelFormat)
            {
                [pixelFormat release];
            }
        }

        bool RendererOGLMacOS::init(Window* newWindow,
                                    const Size2& newSize,
                                    uint32_t newSampleCount,
                                    Texture::Filter newTextureFilter,
                                    uint32_t newMaxAnisotropy,
                                    bool newVerticalSync,
                                    bool newDepth,
                                    bool newDebugRenderer)
        {
            // Create pixel format
            std::vector<NSOpenGLPixelFormatAttribute> openGL3Attributes =
            {
                NSOpenGLPFAAccelerated,
                NSOpenGLPFANoRecovery,
                NSOpenGLPFADoubleBuffer,
                NSOpenGLPFAOpenGLProfile, NSOpenGLProfileVersion3_2Core,
                NSOpenGLPFAColorSize, 24,
                NSOpenGLPFAAlphaSize, 8
            };

            if (newDepth)
            {
                openGL3Attributes.push_back(NSOpenGLPFADepthSize);
                openGL3Attributes.push_back(static_cast<NSOpenGLPixelFormatAttribute>(newDepth ? 24 : 0));
            }

            if (newSampleCount)
            {
                openGL3Attributes.push_back(NSOpenGLPFAMultisample);
                openGL3Attributes.push_back(NSOpenGLPFASampleBuffers);
                openGL3Attributes.push_back(1);
                openGL3Attributes.push_back(NSOpenGLPFASamples);
                openGL3Attributes.push_back(newSampleCount);
            }

            openGL3Attributes.push_back(0);

            pixelFormat = [[NSOpenGLPixelFormat alloc] initWithAttributes:openGL3Attributes.data()];

            if (pixelFormat)
            {
                apiMajorVersion = 3;
                apiMinorVersion = 2;
                Log(Log::Level::INFO) << "OpenGL 3.2 pixel format created";
            }
            else
            {
                std::vector<NSOpenGLPixelFormatAttribute> openGL2Attributes =
                {
                    NSOpenGLPFAAccelerated,
                    NSOpenGLPFANoRecovery,
                    NSOpenGLPFADoubleBuffer,
                    NSOpenGLPFAOpenGLProfile, NSOpenGLProfileVersionLegacy,
                    NSOpenGLPFAColorSize, 24,
                };

                if (newDepth)
                {
                    openGL2Attributes.push_back(NSOpenGLPFADepthSize);
                    openGL2Attributes.push_back(static_cast<NSOpenGLPixelFormatAttribute>(newDepth ? 24 : 0));
                }

                if (newSampleCount)
                {
                    openGL2Attributes.push_back(NSOpenGLPFAMultisample);
                    openGL2Attributes.push_back(NSOpenGLPFASampleBuffers);
                    openGL2Attributes.push_back(1);
                    openGL2Attributes.push_back(NSOpenGLPFASamples);
                    openGL2Attributes.push_back(newSampleCount);
                }

                openGL2Attributes.push_back(0);

                pixelFormat = [[NSOpenGLPixelFormat alloc] initWithAttributes:openGL2Attributes.data()];

                if (pixelFormat)
                {
                    apiMajorVersion = 2;
                    apiMinorVersion = 0;
                    Log(Log::Level::INFO) << "OpenGL 2 pixel format created";
                }
            }

            if (!pixelFormat)
            {
                Log(Log::Level::ERR) << "Failed to crete OpenGL pixel format";
                return Nil;
            }

            // Create OpenGL context
            openGLContext = [[NSOpenGLContext alloc] initWithFormat:pixelFormat shareContext:NULL];
            [openGLContext makeCurrentContext];

            WindowMacOS* windowMacOS = static_cast<WindowMacOS*>(newWindow);
            [openGLContext setView:windowMacOS->getNativeView()];

            GLint swapInt = newVerticalSync ? 1 : 0;
            [openGLContext setValues:&swapInt forParameter:NSOpenGLCPSwapInterval];

            if (!RendererOGL::init(newWindow,
                                   newSize,
                                   newSampleCount,
                                   newTextureFilter,
                                   newMaxAnisotropy,
                                   newVerticalSync,
                                   newDepth,
                                   newDebugRenderer))
            {
                return false;
            }

            eventHandler.windowHandler = std::bind(&RendererOGLMacOS::handleWindow, this, std::placeholders::_1, std::placeholders::_2);
            sharedEngine->getEventDispatcher()->addEventHandler(&eventHandler);

            const CGDirectDisplayID displayId = [[[[windowMacOS->getNativeWindow() screen] deviceDescription] objectForKey:@"NSScreenNumber"] unsignedIntValue];

            if (CVDisplayLinkCreateWithCGDisplay(displayId, &displayLink) != kCVReturnSuccess)
            {
                Log(Log::Level::ERR) << "Failed to create display link";
                return false;
            }

            if (CVDisplayLinkSetOutputCallback(displayLink, renderCallback, nullptr) != kCVReturnSuccess)
            {
                Log(Log::Level::ERR) << "Failed to set output callback for the display link";
                return false;
            }

            if (CVDisplayLinkStart(displayLink) != kCVReturnSuccess)
            {
                Log(Log::Level::ERR) << "Failed to start display link";
                return false;
            }

            return true;
        }

        bool RendererOGLMacOS::lockContext()
        {
            [openGLContext makeCurrentContext];

            return true;
        }

        bool RendererOGLMacOS::swapBuffers()
        {
            [openGLContext flushBuffer];

            return true;
        }

        bool RendererOGLMacOS::upload()
        {
            [openGLContext update];

            return RendererOGL::upload();
        }

        bool RendererOGLMacOS::handleWindow(Event::Type type, const WindowEvent& event)
        {
            if (type == Event::Type::WINDOW_SCREEN_CHANGE)
            {
                sharedApplication->execute([this, event]() {
                    if (displayLink)
                    {
                        if (CVDisplayLinkStop(displayLink) != kCVReturnSuccess)
                        {
                            Log(Log::Level::ERR) << "Failed to stop display link";
                        }

                        CVDisplayLinkRelease(displayLink);
                        displayLink = nullptr;
                    }

                    const CGDirectDisplayID displayId = event.screenId;

                    if (CVDisplayLinkCreateWithCGDisplay(displayId, &displayLink) != kCVReturnSuccess)
                    {
                        Log(Log::Level::ERR) << "Failed to create display link";
                        return;
                    }

                    if (CVDisplayLinkSetOutputCallback(displayLink, renderCallback, nullptr) != kCVReturnSuccess)
                    {
                        Log(Log::Level::ERR) << "Failed to set output callback for the display link";
                        return;
                    }

                    if (CVDisplayLinkStart(displayLink) != kCVReturnSuccess)
                    {
                        Log(Log::Level::ERR) << "Failed to start display link";
                        return;
                    }
                });
            }

            return true;
        }
    } // namespace graphics
} // namespace ouzel
