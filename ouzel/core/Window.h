// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <string>
#include "utils/Noncopyable.h"
#include "math/Size2.h"
#include "events/EventHandler.h"

namespace ouzel
{
    class Engine;

    class Window: public Noncopyable
    {
        friend Engine;
    public:
        virtual ~Window();

        virtual void close();

        virtual const Size2& getSize() const { return size; }
        virtual void setSize(const Size2& newSize);
        virtual bool getResizable() const { return resizable; }

        virtual void setFullscreen(bool newFullscreen);
        virtual bool isFullscreen() const { return fullscreen; }

        virtual const std::string& getTitle() const { return title; }
        virtual void setTitle(const std::string& newTitle);

        float getContentScale() const { return contentScale; }

        Vector2 convertWindowToNormalizedLocation(const Vector2& position) const
        {
            return Vector2(position.v[0] / size.v[0],
                           1.0f - (position.v[1] / size.v[1]));
        }

        Vector2 convertWindowToNormalizedLocationRelative(const Vector2& position) const
        {
            return Vector2(position.v[0] / size.v[0],
                           -position.v[1] / size.v[1]);
        }

        Vector2 convertNormalizedToWindowLocation(const Vector2& position) const
        {
            return Vector2(position.v[0] * size.v[0],
                           (1.0f - position.v[1]) * size.v[1]);
        }

        Vector2 convertNormalizedToWindowLocationRelative(const Vector2& position) const
        {
            return Vector2(position.v[0] * size.v[0],
                           -position.v[1] * size.v[1]);
        }

    protected:
        Window(const Size2& aSize,
               bool aResizable,
               bool aFullscreen,
               const std::string& aTitle,
               bool aHighDpi);
        virtual bool init();

        bool handleWindowChange(Event::Type type, const WindowEvent& event);

        Size2 size;
        bool resizable = false;
        bool fullscreen = false;
        bool highDpi = true;

        std::string title;
        float contentScale = 1.0f;

        ouzel::EventHandler eventHandler;
    };
}
