#pragma once
#include <raylib.h>
#include <Core/Scene.hpp>
#include <Core/SceneManager.hpp>

class MainMenu : public Scene {
public:
    MainMenu(SceneManager* mgr) : mgr(mgr) {}
    ~MainMenu() { Deinit(); }

    void Init() override;
    void Deinit() override;
    
    void Update(float dt) override;
    void Draw() override;
private:
    SceneManager* mgr;

    enum class AnimationPhase {
        TopCircle,
        BottomCircle,
        MiddleCircle,
        Complete
    };
    
    AnimationPhase currentPhase = AnimationPhase::TopCircle;
    
    Color bottomCircleColor = Color {20, 160, 133, 255};
    Color topCircleColor = Color {38, 185, 154, 255};
    Color middleCircleColor = Color {129, 204, 184, 255};
    Color backgroundColor = BLACK;
    
    float topCircleRadius = 0.0f;
    float middleCircleRadius = 0.0f;
    float bottomCircleRadius = 0.0f;
    float maxCircleRadius = 0.0f;
    float elapsedTime = 0.0f;
    const float circleGrowthSpeed = 100.0f;
    
    bool isAnimating = false;
    bool animationComplete = false;
    bool up = true;
    float playButtonRadius = 0.0f;
    Color playButtonColor = BLACK;
    
    Texture2D exitTexture;
    Texture2D playTexture;
    float playTextureSize = 0.25f;
    
    void UpdateAnimation(float dt);
    void UpdateUIInteraction(float dt);
    void DrawAnimationPhase();
    void DrawUI();
    int UpdateAnimationData(float dt);
};