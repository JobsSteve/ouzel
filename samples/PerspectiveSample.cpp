// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "PerspectiveSample.h"
#include "MainMenu.h"

using namespace std;
using namespace ouzel;

PerspectiveSample::PerspectiveSample()
{
    cursor.init(input::SystemCursor::CROSS);
    sharedEngine->getInput()->setCursor(&cursor);

    eventHandler.keyboardHandler = bind(&PerspectiveSample::handleKeyboard, this, placeholders::_1, placeholders::_2);
    eventHandler.mouseHandler = bind(&PerspectiveSample::handleMouse, this, placeholders::_1, placeholders::_2);
    eventHandler.touchHandler = bind(&PerspectiveSample::handleTouch, this, placeholders::_1, placeholders::_2);
    eventHandler.gamepadHandler = bind(&PerspectiveSample::handleGamepad, this, placeholders::_1, placeholders::_2);
    eventHandler.uiHandler = bind(&PerspectiveSample::handleUI, this, placeholders::_1, placeholders::_2);

    sharedEngine->getEventDispatcher()->addEventHandler(&eventHandler);

    sharedEngine->getRenderer()->setClearDepthBuffer(true);

    camera.reset(new scene::Camera());
    camera->setDepthTest(true);
    camera->setDepthWrite(true);

    camera->setType(scene::Camera::Type::PERSPECTIVE);
    camera->setFarPlane(1000.0f);
    camera->setPosition(Vector3(0.0f, 0.0f, -400.0f));

    layer.reset(new scene::Layer());
    layer->addChild(camera.get());
    addLayer(layer.get());

    // floor
    floorSprite.reset(new scene::Sprite());
    floorSprite->initFromFile("floor.jpg");

    for (const scene::SpriteFrame& spriteFrame : floorSprite->getFrames())
    {
        spriteFrame.getTexture()->setMaxAnisotropy(4);
    }

    floor.reset(new scene::Node());
    floor->addComponent(floorSprite.get());
    layer->addChild(floor.get());
    floor->setPosition(Vector2(0.0f, -50.0f));
    floor->setRotation(Vector3(TAU_4, TAU / 8.0f, 0.0f));
    
    // character
    characterSprite.reset(new scene::Sprite());
    characterSprite->initFromFile("run.json");
    characterSprite->play(true);

    for (const scene::SpriteFrame& spriteFrame : characterSprite->getFrames())
    {
        spriteFrame.getTexture()->setMaxAnisotropy(4);
    }

    character.reset(new scene::Node());
    character->addComponent(characterSprite.get());
    layer->addChild(character.get());
    character->setPosition(Vector2(10.0f, 0.0f));

    jumpSound.reset(new audio::Sound());
    std::shared_ptr<ouzel::audio::SoundDataWave> soundData = std::make_shared<ouzel::audio::SoundDataWave>();
    soundData->initFromFile("jump.wav");
    jumpSound->init(soundData);
    jumpSound->setPosition(character->getPosition());

    rotate.reset(new scene::Rotate(10.0f, Vector3(0.0f, TAU, 0.0f)));
    character->addComponent(rotate.get());
    rotate->start();

    guiCamera.reset(new scene::Camera());
    guiCamera->setScaleMode(scene::Camera::ScaleMode::SHOW_ALL);
    guiCamera->setTargetContentSize(Size2(800.0f, 600.0f));

    guiLayer.reset(new scene::Layer());
    guiLayer->addChild(guiCamera.get());
    addLayer(guiLayer.get());

    menu.reset(new gui::Menu());
    guiLayer->addChild(menu.get());

    backButton.reset(new ouzel::gui::Button("button.png", "button_selected.png", "button_down.png", "", "Back", "arial.fnt", Color::BLACK, Color::BLACK, Color::BLACK));
    backButton->setPosition(Vector2(-200.0f, -200.0f));
    menu->addWidget(backButton.get());
}

bool PerspectiveSample::handleUI(ouzel::Event::Type type, const ouzel::UIEvent& event)
{
    if (type == Event::Type::UI_CLICK_NODE)
    {
        if (event.node == backButton.get())
        {
            sharedEngine->getSceneManager()->setScene(std::unique_ptr<scene::Scene>(new MainMenu()));
        }
    }

    return true;
}

bool PerspectiveSample::handleKeyboard(ouzel::Event::Type type, const ouzel::KeyboardEvent& event)
{
    if (type == Event::Type::KEY_DOWN)
    {
        Quaternion rotation = Quaternion::IDENTITY;

        switch (event.key)
        {
            case input::KeyboardKey::UP:
                rotation.setEulerAngles(Vector3(-TAU / 100.0f, 0.0f, 0.0f));
                break;
            case input::KeyboardKey::DOWN:
                rotation.setEulerAngles(Vector3(TAU / 100.0f, 0.0f, 0.0f));
                break;
            case input::KeyboardKey::LEFT:
                rotation.setEulerAngles(Vector3(0.0f, -TAU / 100.0f, 0.0f));
                break;
            case input::KeyboardKey::RIGHT:
                rotation.setEulerAngles(Vector3(0.0f, TAU / 100.0f, 0.0f));
                break;
            case input::KeyboardKey::ESCAPE:
                sharedEngine->getSceneManager()->setScene(std::unique_ptr<scene::Scene>(new MainMenu()));
                return true;
            case input::KeyboardKey::TAB:
                jumpSound->play();
                break;
            case input::KeyboardKey::KEY_S:
                sharedEngine->getRenderer()->saveScreenshot("test.png");
                break;
            default:
                break;
        }

        camera->setRotation(camera->getRotation() * rotation);
        sharedEngine->getAudio()->setListenerRotation(camera->getRotation());
    }

    return true;
}

bool PerspectiveSample::handleMouse(ouzel::Event::Type type, const ouzel::MouseEvent& event)
{    
    if (event.modifiers & LEFT_MOUSE_DOWN)
    {
        if (type == Event::Type::MOUSE_MOVE)
        {
            Quaternion rotation;

            rotation.setEulerAngles(Vector3(event.difference.y() / 2.0f,
                                            -event.difference.x() / 2.0f,
                                            0.0f));

            camera->setRotation(camera->getRotation() * rotation);
        }
    }

    return true;
}

bool PerspectiveSample::handleTouch(ouzel::Event::Type type, const ouzel::TouchEvent& event)
{
    if (type == Event::Type::TOUCH_MOVE)
    {
        Quaternion rotation;

        rotation.setEulerAngles(Vector3(event.difference.y() / 2.0f,
                                        -event.difference.x() / 2.0f,
                                        0.0f));

        camera->setRotation(camera->getRotation() * rotation);
    }

    return true;
}

bool PerspectiveSample::handleGamepad(Event::Type type, const GamepadEvent& event)
{
    if (type == Event::Type::GAMEPAD_BUTTON_CHANGE)
    {
        if (event.pressed &&
            event.button == input::GamepadButton::FACE2)
        {
            sharedEngine->getSceneManager()->setScene(std::unique_ptr<scene::Scene>(new MainMenu()));
        }
    }

    return true;
}
