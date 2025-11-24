#pragma once
#define WIN32_LEAN_AND_MEAN
#define NOGDI
#define NOUSER
#define NOKERNEL
#define NOSOUND
#define NOMINMAX
#include <raylib.h>

enum class GameMode {
    VsAI,
    VsFriend,
    VsOnline
};

enum class PlayerSide {
    Left,
    Right
};

struct Paddle {
    float x, y;
    float width = 20.0f;
    float height = 100.0f;
    float speed = 400.0f;
    Color color = WHITE;
};

struct Ball {
    float x, y;
    float radius = 10.0f;
    float velX = 300.0f;
    float velY = 200.0f;
    Color color = WHITE;
};

struct GameState {
    Paddle leftPaddle;
    Paddle rightPaddle;
    Ball ball;
    
    int leftScore = 0;
    int rightScore = 0;
    
    bool paused = false;
    bool gameOver = false;
    bool leftWon = false;
    
    float screenWidth = 1280.0f;
    float screenHeight = 720.0f;
    
    static constexpr int WINNING_SCORE = 20;
    
    void Reset() {
        // Reset ball to center
        ball.x = screenWidth / 2;
        ball.y = screenHeight / 2;
        
        // Random direction
        ball.velX = (GetRandomValue(0, 1) ? 1 : -1) * 300.0f;
        ball.velY = GetRandomValue(-200, 200);
        
        // Reset paddles
        leftPaddle.y = screenHeight / 2 - leftPaddle.height / 2;
        rightPaddle.y = screenHeight / 2 - rightPaddle.height / 2;
    }
    
    void Init(float width, float height) {
        screenWidth = width;
        screenHeight = height;
        
        // Position paddles
        leftPaddle.x = 50;
        leftPaddle.y = screenHeight / 2 - leftPaddle.height / 2;
        
        rightPaddle.x = screenWidth - 50 - rightPaddle.width;
        rightPaddle.y = screenHeight / 2 - rightPaddle.height / 2;
        
        Reset();
    }
};
