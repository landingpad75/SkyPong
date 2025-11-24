#pragma once
#include <raylib.h>
#include <Core/Scene.hpp>
#include <Core/SceneManager.hpp>
#include <Game/GameState.hpp>

class PongGame : public Scene {
public:
    PongGame(SceneManager* mgr, GameMode mode) : mgr(mgr), mode(mode) {}
    ~PongGame() { Deinit(); }

    void Init() override;
    void Deinit() override;
    void Update(float dt) override;
    void Draw() override;

private:
    SceneManager* mgr;
    GameMode mode;
    GameState state;
    
    // AI
    float aiReactionDelay = 0.0f;
    float aiTargetY = 0.0f;
    
    // Input handling
    void HandleInput(float dt);
    void HandleAIInput(float dt);
    
    // Game logic (server-friendly)
    void UpdatePaddle(Paddle& paddle, float moveDir, float dt);
    void UpdateBall(float dt);
    void CheckCollisions();
    void CheckScore();
    
    // Rendering
    void DrawGame();
    void DrawUI();
};
