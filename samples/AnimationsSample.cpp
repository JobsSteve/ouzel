// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "AnimationsSample.h"
#include "MainMenu.h"

using namespace std;
using namespace ouzel;

AnimationsSample::AnimationsSample():
    backButton(new ouzel::gui::Button("button.png", "button_selected.png", "button_down.png", "", "Back", "arial.fnt", Color::BLACK, Color::BLACK, Color::BLACK))
{
    eventHandler.gamepadHandler = bind(&AnimationsSample::handleGamepad, this, placeholders::_1, placeholders::_2);
    eventHandler.uiHandler = bind(&AnimationsSample::handleUI, this, placeholders::_1, placeholders::_2);
    eventHandler.keyboardHandler = bind(&AnimationsSample::handleKeyboard, this, placeholders::_1, placeholders::_2);
    sharedEngine->getEventDispatcher()->addEventHandler(&eventHandler);

    camera.reset(new scene::Camera());
    camera->setScaleMode(scene::Camera::ScaleMode::SHOW_ALL);
    camera->setTargetContentSize(Size2(800.0f, 600.0f));

    layer.reset(new scene::Layer());
    layer->addChild(camera.get());
    addLayer(layer.get());

    shapeDrawable.reset(new scene::ShapeRenderer());
    shapeDrawable->rectangle(ouzel::Rectangle(100.0f, 100.0f), Color(0, 128, 128, 255), true);
    shapeDrawable->rectangle(ouzel::Rectangle(100.0f, 100.0f), Color::WHITE, false, 2.0f);
    shapeDrawable->line(Vector2(0.0f, 0.0f), Vector2(50.0f, 50.0f), Color::CYAN, 2.0f);

    shapeDrawable->curve({Vector2(50.0f, 50.0f),
                          Vector2(100.0f, 50.0f),
                          Vector2(50.0f, 0.0f),
                          Vector2(100.0f, 0.0f)},
                         Color::YELLOW);

    shapeDrawable->circle(Vector2(75.0f, 75.0f), 20.0f, Color::BLUE, false, 16, 4.0f);
    shapeDrawable->circle(Vector2(25.0f, 75.0f), 20.0f, Color::BLUE, true);

    shapeDrawable->polygon({Vector2(15.0f, 75.0f),
                            Vector2(25.0f, 75.0f),
                            Vector2(25.0f, 55.0f)},
                           Color::YELLOW, false);

    drawNode.reset(new scene::Node());
    drawNode->addComponent(shapeDrawable.get());
    drawNode->setPosition(Vector2(-300, 0.0f));
    layer->addChild(drawNode.get());

    shake.reset(new scene::Shake(10.0f, Vector2(10.0f, 20.0f), 20.0f));
    drawNode->addComponent(shake.get());
    shake->start();

    witchSprite.reset(new scene::Sprite());
    witchSprite->initFromFile("witch.png");

    witch.reset(new scene::Node());
    witch->setPosition(Vector2(200, 0.0f));
    witch->addComponent(witchSprite.get());
    layer->addChild(witch.get());

    witchScale.reset(new scene::Scale(2.0f, Vector2(0.1f, 0.1f), false));
    witchFade.reset(new scene::Fade(2.0f, 0.4f));

    vector<scene::Animator*> parallel = {
        witchScale.get(),
        witchFade.get()
    };

    witchRotate.reset(new scene::Rotate(1.0f, Vector3(0.0f, 0.0f, TAU), false));

    witchRepeat.reset(new scene::Repeat(witchRotate.get(), 3));
    witchParallel.reset(new scene::Parallel(parallel));

    vector<scene::Animator*> sequence = {
        witchRepeat.get(),
        witchParallel.get()
    };

    witchSequence.reset(new scene::Sequence(sequence));

    witch->addComponent(witchSequence.get());
    witchSequence->start();

    ballSprite.reset(new scene::Sprite());
    ballSprite->initFromFile("ball.png");

    ball.reset(new scene::Node());
    ball->addComponent(ballSprite.get());
    layer->addChild(ball.get());

    ballDelay.reset(new scene::Animator(1.0f));
    ballMove.reset(new scene::Move(2.0f, Vector2(0.0f, -240.0f), false));
    ballEase.reset(new scene::Ease(ballMove.get(), scene::Ease::Type::OUT, scene::Ease::Func::BOUNCE));

    vector<scene::Animator*> sequence2 = {
        ballDelay.get(),
        ballEase.get()
    };
     
    ballSequence.reset(new scene::Sequence(sequence2));

    ball->addComponent(ballSequence.get());
    ballSequence->start();

    guiCamera.reset(new scene::Camera());
    guiCamera->setScaleMode(scene::Camera::ScaleMode::SHOW_ALL);
    guiCamera->setTargetContentSize(Size2(800.0f, 600.0f));

    guiLayer.reset(new scene::Layer());
    guiLayer->addChild(guiCamera.get());
    addLayer(guiLayer.get());

    menu.reset(new gui::Menu());
    guiLayer->addChild(menu.get());

    backButton->setPosition(Vector2(-200.0f, -200.0f));
    menu->addWidget(backButton.get());
}

bool AnimationsSample::handleGamepad(Event::Type type, const GamepadEvent& event)
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

bool AnimationsSample::handleUI(Event::Type type, const UIEvent& event) const
{
    if (type == Event::Type::UI_CLICK_NODE && event.node == backButton.get())
    {
        sharedEngine->getSceneManager()->setScene(std::unique_ptr<scene::Scene>(new MainMenu()));
    }

    return true;
}

bool AnimationsSample::handleKeyboard(Event::Type type, const KeyboardEvent& event) const
{
    if (type == Event::Type::KEY_DOWN)
    {
        switch (event.key)
        {
            case input::KeyboardKey::ESCAPE:
                sharedEngine->getSceneManager()->setScene(std::unique_ptr<scene::Scene>(new MainMenu()));
                break;
            default:
                break;
        }
    }

    return true;
}
