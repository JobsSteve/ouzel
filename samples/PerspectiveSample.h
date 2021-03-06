// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

class PerspectiveSample: public ouzel::scene::Scene
{
public:
    PerspectiveSample();

private:
    bool handleUI(ouzel::Event::Type type, const ouzel::UIEvent& event);
    bool handleKeyboard(ouzel::Event::Type type, const ouzel::KeyboardEvent& event);
    bool handleMouse(ouzel::Event::Type type, const ouzel::MouseEvent& event);
    bool handleTouch(ouzel::Event::Type type, const ouzel::TouchEvent& event);
    bool handleGamepad(ouzel::Event::Type type, const ouzel::GamepadEvent& event);

    std::unique_ptr<ouzel::scene::Layer> layer;
    std::unique_ptr<ouzel::scene::Camera> camera;

    std::unique_ptr<ouzel::scene::Sprite> characterSprite;
    std::unique_ptr<ouzel::scene::Node> character;

    std::unique_ptr<ouzel::scene::Sprite> floorSprite;
    std::unique_ptr<ouzel::scene::Node> floor;

    std::unique_ptr<ouzel::audio::Sound> jumpSound;

    std::unique_ptr<ouzel::scene::Animator> rotate;

    ouzel::EventHandler eventHandler;

    std::unique_ptr<ouzel::scene::Layer> guiLayer;
    std::unique_ptr<ouzel::scene::Camera> guiCamera;
    std::unique_ptr<ouzel::gui::Menu> menu;
    std::unique_ptr<ouzel::gui::Button> backButton;

    ouzel::input::Cursor cursor;
};
