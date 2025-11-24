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
#include <Game/GameState.hpp>
#include <Network/NetworkClient.hpp>
#include <memory>
#include <string>

class OnlineGame : public Scene {
public:
    OnlineGame(SceneManager* mgr) : mgr(mgr) {}
    ~OnlineGame() { Deinit(); }

    void Init() override;
    void Deinit() override;
    void Update(float dt) override;
    void Draw() override;

private:
    SceneManager* mgr;
    GameState state;
    PlayerSide mySide;
    
    std::string statusText;
    
    // Network sync
    float syncTimer = 0.0f;
    float syncInterval = 1.0f / 30.0f; // 30 updates per second
    
    // Input
    void HandleLocalInput(float dt);
    void SendInput(float paddleY);
    void SendGameState();
    
    // Network
    void ProcessNetworkMessages();
    void HandleProtocolMessage(const std::string& data);
    void HandleOpponentInput(const std::string& data);
    void HandleGameStateUpdate(const std::string& data);
    
    // Game logic (same as PongGame)
    void UpdatePaddle(Paddle& paddle, float moveDir, float dt);
    void UpdateBall(float dt);
    void CheckCollisions();
    void CheckScore();
    
    // Rendering
    void DrawGame();
    void DrawUI();
};
