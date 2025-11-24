#include <Scenes/ChoiceMenu.hpp>
#include <cmath>

void ChoiceMenu::Init() {
    float screenWidth = GetRenderWidth();
    float screenHeight = GetRenderHeight();

    aiButton = {{screenWidth / 2 - 280, screenHeight / 2}, 90, CLITERAL(Color){141, 189, 174, 255}, "VS AI", false};
    friendButton = {{screenWidth / 2, screenHeight / 2}, 90, CLITERAL(Color){111, 168, 220, 255}, "VS Friend", false};
    onlineButton = {{screenWidth / 2 + 280, screenHeight / 2}, 90, CLITERAL(Color){255, 193, 86, 255}, "Online", false};
}

void ChoiceMenu::Deinit() {}

void ChoiceMenu::Update(float dt) {
    float screenWidth = GetRenderWidth();
    float screenHeight = GetRenderHeight();
    Vector2 mouse = GetMousePosition();

    aiButton.hovered = CheckCollisionPointCircle(mouse, aiButton.center, aiButton.radius);
    friendButton.hovered = CheckCollisionPointCircle(mouse, friendButton.center, friendButton.radius);
    onlineButton.hovered = CheckCollisionPointCircle(mouse, onlineButton.center, onlineButton.radius);

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        if (aiButton.hovered) {
            HandleButtonClick(aiButton);
        } else if (friendButton.hovered) {
            HandleButtonClick(friendButton);
        } else if (onlineButton.hovered) {
            HandleButtonClick(onlineButton);
        }
    }
    
    if (IsKeyPressed(KEY_ESCAPE)) {
        mgr->ReplaceScene("main_menu");
    }
}

void ChoiceMenu::HandleButtonClick(ChoiceButton& button) {
    if (button.label == "VS AI") {
        mgr->ReplaceScene("vs_ai");
    } else if (button.label == "VS Friend") {
        mgr->ReplaceScene("vs_friend");
    } else if (button.label == "Online") {
        mgr->ReplaceScene("vs_online");
    }
}

void ChoiceMenu::Draw() {
    float screenWidth = GetRenderWidth();
    float screenHeight = GetRenderHeight();

    Color leftColor = (Color){20, 160, 133, 255};
    Color rightColor = (Color){38, 185, 154, 255};
    
    ClearBackground(leftColor);
    DrawRectangle(screenWidth / 2, 0, screenWidth / 2, screenHeight, rightColor);
    
    DrawLine(screenWidth / 2, 0, screenWidth / 2, screenHeight, WHITE);
    
    DrawCircle(0, 0, 151, WHITE);
    DrawCircle(0, 0, 150, CLITERAL(Color){129, 204, 184, 255});
    DrawCircle(screenWidth, screenHeight, 101, WHITE);
    DrawCircle(screenWidth, screenHeight, 100, CLITERAL(Color){129, 204, 184, 255});

    const char* title = "Choose Game Mode";
    int titleFontSize = 50;
    int titleWidth = MeasureText(title, titleFontSize);
    DrawText(title, screenWidth / 2 - titleWidth / 2, 80, titleFontSize, WHITE);

    auto drawBtn = [](const ChoiceButton& btn) {
        float scale = btn.hovered ? 1.1f : 1.0f;
        float currentRadius = btn.radius * scale;
        
        if (btn.hovered) {
            DrawCircleV(btn.center, currentRadius + 8, Fade(WHITE, 0.3f));
        }
        
        DrawCircleV(btn.center, currentRadius + 3, WHITE);
        
        DrawCircleV(btn.center, currentRadius, btn.color);
        
        DrawCircleV({btn.center.x - 10, btn.center.y - 10}, currentRadius * 0.3f, Fade(WHITE, 0.3f));

        int fontSize = btn.hovered ? 32 : 28;
        int textW = MeasureText(btn.label.c_str(), fontSize);
        DrawText(btn.label.c_str(), btn.center.x - textW / 2, btn.center.y - fontSize / 2, fontSize, WHITE);
    };

    drawBtn(aiButton);
    drawBtn(friendButton);
    drawBtn(onlineButton);
    
    const char* hint = "Press ESC to go back";
    int hintFontSize = 20;
    int hintWidth = MeasureText(hint, hintFontSize);
    DrawText(hint, screenWidth / 2 - hintWidth / 2, screenHeight - 50, hintFontSize, Fade(WHITE, 0.7f));
}