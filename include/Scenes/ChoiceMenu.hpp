#pragma once
#define WIN32_LEAN_AND_MEAN
#define NOGDI
#define NOUSER
#define NOKERNEL
#define NOSOUND
#define NOMINMAX

#include <raylib.h>
#include <Core/Scene.hpp>
#include <Core/SceneManager.hpp>
#include <string>

class ChoiceMenu : public Scene {
public:
    ChoiceMenu(SceneManager* mgr) : mgr(mgr) {}
    ~ChoiceMenu() { Deinit(); }

    void Init() override;
    void Deinit() override;
    void Update(float dt) override;
    void Draw() override;
private:
    SceneManager* mgr;

    struct ChoiceButton {
        Vector2 center;
        float radius;
        Color color;
        std::string label;
        bool hovered;
    };

    ChoiceButton aiButton;
    ChoiceButton friendButton;
    ChoiceButton onlineButton;
    ChoiceButton* hoveredButton = nullptr;

    void HandleButtonClick(ChoiceButton& button);
    void UpdateButtons(float screenWidth, float screenHeight);
};