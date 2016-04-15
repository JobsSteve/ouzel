// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "WindowIOS.h"
#import "OpenGLView.h"
#import "MetalView.h"

@interface ViewController: UIViewController

@end

@implementation ViewController

-(BOOL)prefersStatusBarHidden
{
    return YES;
}

@end

namespace ouzel
{
    WindowIOS::WindowIOS(const Size2& size, bool resizable, bool fullscreen, const std::string& title, video::Renderer::Driver driver):
        Window(size, resizable, fullscreen, title, driver)
    {

    }

    WindowIOS::~WindowIOS()
    {
        if (_viewController) [_viewController release];
        if (_view) [_view release];
        if (_window) [_window release];
    }

    bool WindowIOS::init()
    {
        _window = [[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]];

        _viewController = [[[ViewController alloc] init] autorelease];
        _window.rootViewController = _viewController;

        CGRect windowFrame = [_window bounds];

        switch (_driver)
        {
            case video::Renderer::Driver::OPENGL:
            {
                OpenGLView* openGLView = [[OpenGLView alloc] initWithFrame:windowFrame];
                _view = openGLView;
                _size.width = openGLView.backingWidth;
                _size.height = openGLView.backingHeight;
                break;
            }
            case video::Renderer::Driver::METAL:
                _view = [[MetalView alloc] initWithFrame:windowFrame];
                break;
            default:
                return false;
        }

        _viewController.view = _view;

        [_window makeKeyAndVisible];

        return Window::init();
    }
}
