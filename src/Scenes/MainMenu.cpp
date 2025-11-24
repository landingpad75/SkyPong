#include <Scenes/MainMenu.hpp>
#include <Discord/DiscordManager.hpp>
#include <raylib.h>

void MainMenu::Init() {
    DiscordManager::Instance().SetState(DiscordState::MainMenu);
    Image exitImg = LoadImage("./exit.png");
    exitTexture = LoadTextureFromImage(exitImg);
    UnloadImage(exitImg);
    
    Image playImg = LoadImage("./play.png");
    playTexture = LoadTextureFromImage(playImg);
    UnloadImage(playImg);
    
    maxCircleRadius = GetRenderWidth() * 1.3f;
}

void MainMenu::Deinit() {
    UnloadTexture(exitTexture);
    UnloadTexture(playTexture);
}

void MainMenu::Update(float dt) {
    DiscordManager::Instance().Update();
    
    if (!animationComplete) {
        UpdateAnimation(dt);
    } else {
        UpdateUIInteraction(dt);
    }
    
    if (IsKeyDown(KEY_R)) {
        playTextureSize = 0.25f;
        up = true;
        playButtonRadius = 0.0f;
        playButtonColor = BLACK;
        isAnimating = false;
    }
}

void MainMenu::UpdateAnimation(float dt) {
    int result = UpdateAnimationData(dt);
    if (result == 1) {
        animationComplete = true;
    }
}

int MainMenu::UpdateAnimationData(float dt) {
    elapsedTime += dt * 4.0f;
    
    if (currentPhase == AnimationPhase::TopCircle) {
        topCircleRadius = elapsedTime * (circleGrowthSpeed * 2);
        
        if (topCircleRadius > maxCircleRadius) {
            elapsedTime = 0;
            currentPhase = AnimationPhase::BottomCircle;
            backgroundColor = topCircleColor;
        }
    }
    else if (currentPhase == AnimationPhase::BottomCircle) {
        bottomCircleRadius = elapsedTime * (circleGrowthSpeed * 2);
        
        if (bottomCircleRadius > (maxCircleRadius / 1.5f)) {
            elapsedTime = 0;
            currentPhase = AnimationPhase::MiddleCircle;
            bottomCircleRadius = maxCircleRadius;
        }
    }
    else if (currentPhase == AnimationPhase::MiddleCircle) {
        middleCircleRadius = elapsedTime * (circleGrowthSpeed / 2);
        
        if (middleCircleRadius > 150.0f) {
            middleCircleRadius = 150.0f;
            currentPhase = AnimationPhase::Complete;
            return 1;
        }
    }
    
    return 0;
}

void MainMenu::UpdateUIInteraction(float dt) {
    Vector2 mousePos = GetMousePosition();
    float screenWidth = GetRenderWidth();
    float screenHeight = GetRenderHeight();
    
    bool exitCollision = CheckCollisionPointCircle(mousePos, {screenWidth, screenHeight}, 100);
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && exitCollision) {
        CloseWindow();
    }
    
    bool playCollision = CheckCollisionPointCircle(mousePos, {screenWidth / 2.0f, screenHeight / 2.0f}, 150);
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && playCollision && !isAnimating) {
        mgr->ReplaceScene("choice_menu");
    }
}

void MainMenu::Draw() {
    if (!animationComplete) {
        DrawAnimationPhase();
    } else {
        DrawUI();
    }
}

void MainMenu::DrawAnimationPhase() {
    float screenWidth = GetRenderWidth();
    float screenHeight = GetRenderHeight();
    
    ClearBackground(backgroundColor);
    
    if (currentPhase == AnimationPhase::MiddleCircle) {
        ClearBackground(bottomCircleColor);
        DrawRectangle(screenWidth / 2, 0, screenWidth / 2, screenHeight, topCircleColor);
    } else {
        if (currentPhase == AnimationPhase::BottomCircle) {
            Vector2 bottomCirclePosition = {0, screenHeight};
            DrawCircleV(bottomCirclePosition, bottomCircleRadius, bottomCircleColor);
        }
        
        if (currentPhase == AnimationPhase::TopCircle) {
            Vector2 topCirclePosition = {screenWidth, screenHeight};
            DrawCircleV(topCirclePosition, topCircleRadius, topCircleColor);
        } else {
            DrawRectangle(screenWidth / 2, 0, 800, 900, topCircleColor);
        }
    }
    
    for (int i = 0; i < middleCircleRadius * 4; i += 20) {
        DrawRectangle(screenWidth / 2 - 2, i, 4, 10, WHITE);
    }
    
    if (currentPhase == AnimationPhase::MiddleCircle) {
        DrawCircle(screenWidth / 2, screenHeight / 2, middleCircleRadius + 1, WHITE);
        DrawCircle(screenWidth / 2, screenHeight / 2, middleCircleRadius, middleCircleColor);
        
        DrawCircle(0, 0, middleCircleRadius >= 151 ? 151 : middleCircleRadius + 1, WHITE);
        DrawCircle(0, 0, middleCircleRadius >= 150 ? 150 : middleCircleRadius + 1, Color {129, 204, 184, 255});
        
        DrawCircle(GetRenderWidth(), GetRenderHeight(), middleCircleRadius >= 101 ? 101 : middleCircleRadius + 1, WHITE);
        DrawCircle(GetRenderWidth(), GetRenderHeight(), middleCircleRadius >= 100 ? 100 : middleCircleRadius, Color {129, 204, 184, 255});
    }
}

void MainMenu::DrawUI() {
    float screenWidth = GetRenderWidth();
    float screenHeight = GetRenderHeight();
    
    ClearBackground(bottomCircleColor);
    DrawRectangle(screenWidth / 2, 0, screenWidth / 2, screenHeight, topCircleColor);
    
    DrawCircle(0, 0, 151, WHITE);
    DrawCircle(0, 0, 150, Color {129, 204, 184, 255});
    DrawCircle(GetRenderWidth(), GetRenderHeight(), 101, WHITE);
    DrawCircle(GetRenderWidth(), GetRenderHeight(), 100, Color {129, 204, 184, 255});
    
    for (int i = 0; i < screenHeight; i += 20) {
        DrawRectangle(screenWidth / 2 - 2, i, 4, 10, WHITE);
    }
    DrawCircle(screenWidth / 2, screenHeight / 2, 151, WHITE);
    DrawCircle(screenWidth / 2, screenHeight / 2, 150, middleCircleColor);
    
    DrawTextureEx(exitTexture, {1220, 740}, 0.0f, 0.07f, WHITE);
    
    DrawTextureEx(playTexture, {597, 335}, 0.0f, playTextureSize, WHITE);
}