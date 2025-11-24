#include <Scenes/PongGame.hpp>
#include <Discord/DiscordManager.hpp>
#include <cmath>

void PongGame::Init() {
    state.Init(GetRenderWidth(), GetRenderHeight());
    
    std::string details = mode == GameMode::VsAI ? "VS AI" : "VS Friend";
    DiscordManager::Instance().SetState(DiscordState::InGame, details);
}

void PongGame::Deinit() {}

void PongGame::Update(float dt) {
    DiscordManager::Instance().Update();
    
    if (IsKeyPressed(KEY_ESCAPE)) {
        mgr->ReplaceScene("choice_menu");
        return;
    }
    
    if (state.gameOver) {
        if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ENTER)) {
            mgr->ReplaceScene("choice_menu");
        }
        return;
    }
    
    if (IsKeyPressed(KEY_P)) {
        state.paused = !state.paused;
    }
    
    if (state.paused) return;
    
    HandleInput(dt);
    
    if (mode == GameMode::VsAI) {
        HandleAIInput(dt);
    }
    
    UpdateBall(dt);
    CheckCollisions();
    CheckScore();
}

void PongGame::HandleInput(float dt) {
    float leftMove = 0.0f;
    if (IsKeyDown(KEY_W)) leftMove = -1.0f;
    if (IsKeyDown(KEY_S)) leftMove = 1.0f;
    UpdatePaddle(state.leftPaddle, leftMove, dt);
    
    if (mode == GameMode::VsFriend) {
        float rightMove = 0.0f;
        if (IsKeyDown(KEY_UP)) rightMove = -1.0f;
        if (IsKeyDown(KEY_DOWN)) rightMove = 1.0f;
        UpdatePaddle(state.rightPaddle, rightMove, dt);
    }
}

void PongGame::HandleAIInput(float dt) {
    aiReactionDelay -= dt;
    
    if (aiReactionDelay <= 0.0f) {
        aiReactionDelay = 0.1f;        aiTargetY = state.ball.y;
    }
    
    float paddleCenter = state.rightPaddle.y + state.rightPaddle.height / 2;
    float diff = aiTargetY - paddleCenter;
    
    float aiMove = 0.0f;
    if (std::abs(diff) > 10.0f) {
        aiMove = diff > 0 ? 1.0f : -1.0f;
    }
    
    UpdatePaddle(state.rightPaddle, aiMove, dt);
}

void PongGame::UpdatePaddle(Paddle& paddle, float moveDir, float dt) {
    if (moveDir == 0.0f) return;
    
    paddle.y += moveDir * paddle.speed * dt;
    
    if (paddle.y < 0) paddle.y = 0;
    if (paddle.y + paddle.height > state.screenHeight) {
        paddle.y = state.screenHeight - paddle.height;
    }
}

void PongGame::UpdateBall(float dt) {
    state.ball.x += state.ball.velX * dt;
    state.ball.y += state.ball.velY * dt;
    
    if (state.ball.y - state.ball.radius <= 0 || 
        state.ball.y + state.ball.radius >= state.screenHeight) {
        state.ball.velY *= -1;
        
        if (state.ball.y - state.ball.radius < 0) {
            state.ball.y = state.ball.radius;
        }
        if (state.ball.y + state.ball.radius > state.screenHeight) {
            state.ball.y = state.screenHeight - state.ball.radius;
        }
    }
}

void PongGame::CheckCollisions() {
    if (state.ball.velX < 0) {
        Rectangle leftRect = {
            state.leftPaddle.x,
            state.leftPaddle.y,
            state.leftPaddle.width,
            state.leftPaddle.height
        };
        
        if (CheckCollisionCircleRec({state.ball.x, state.ball.y}, state.ball.radius, leftRect)) {
            state.ball.velX = std::abs(state.ball.velX);
            
            float paddleCenter = state.leftPaddle.y + state.leftPaddle.height / 2;
            float hitPos = (state.ball.y - paddleCenter) / (state.leftPaddle.height / 2);
            state.ball.velY = hitPos * 400.0f;
            
            state.ball.velX *= 1.05f;
        }
    }
    
    if (state.ball.velX > 0) {
        Rectangle rightRect = {
            state.rightPaddle.x,
            state.rightPaddle.y,
            state.rightPaddle.width,
            state.rightPaddle.height
        };
        
        if (CheckCollisionCircleRec({state.ball.x, state.ball.y}, state.ball.radius, rightRect)) {
            state.ball.velX = -std::abs(state.ball.velX);
            
            float paddleCenter = state.rightPaddle.y + state.rightPaddle.height / 2;
            float hitPos = (state.ball.y - paddleCenter) / (state.rightPaddle.height / 2);
            state.ball.velY = hitPos * 400.0f;
            
            state.ball.velX *= 1.05f;
        }
    }
}

void PongGame::CheckScore() {
    bool scored = false;
    
    if (state.ball.x - state.ball.radius <= 0) {
        state.rightScore++;
        scored = true;
    }
    
    if (state.ball.x + state.ball.radius >= state.screenWidth) {
        state.leftScore++;
        scored = true;
    }
    
    if (scored) {
        if (state.leftScore >= GameState::WINNING_SCORE) {
            state.gameOver = true;
            state.leftWon = true;
            state.paused = true;
        } else if (state.rightScore >= GameState::WINNING_SCORE) {
            state.gameOver = true;
            state.leftWon = false;
            state.paused = true;
        } else {
            state.Reset();
        }
    }
}

void PongGame::Draw() {
    DrawGame();
    DrawUI();
}

void PongGame::DrawGame() {
    float screenWidth = GetRenderWidth();
    float screenHeight = GetRenderHeight();
    
    Color leftColor = Color {20, 160, 133, 255};
    Color rightColor = Color {38, 185, 154, 255};
    
    ClearBackground(leftColor);
    DrawRectangle(screenWidth / 2, 0, screenWidth / 2, screenHeight, rightColor);
    
    for (int i = 0; i < screenHeight; i += 20) {
        DrawRectangle(screenWidth / 2 - 2, i, 4, 10, WHITE);
    }
    
    DrawRectangle(state.leftPaddle.x, state.leftPaddle.y, 
                  state.leftPaddle.width, state.leftPaddle.height, WHITE);
    DrawRectangle(state.rightPaddle.x, state.rightPaddle.y, 
                  state.rightPaddle.width, state.rightPaddle.height, WHITE);
    
    DrawCircle(state.ball.x, state.ball.y, state.ball.radius, WHITE);
}

void PongGame::DrawUI() {
    float screenWidth = GetRenderWidth();
    
    std::string leftScoreText = std::to_string(state.leftScore);
    std::string rightScoreText = std::to_string(state.rightScore);
    
    DrawText(leftScoreText.c_str(), screenWidth / 4, 40, 60, WHITE);
    DrawText(rightScoreText.c_str(), 3 * screenWidth / 4 - 30, 40, 60, WHITE);
    
    const char* modeText = mode == GameMode::VsAI ? "VS AI" : "VS Friend";
    int modeWidth = MeasureText(modeText, 20);
    DrawText(modeText, screenWidth / 2 - modeWidth / 2, 20, 20, Fade(WHITE, 0.7f));
    
    const char* controls = mode == GameMode::VsAI ? 
        "W/S to move | P to pause | ESC to quit" :
        "W/S and UP/DOWN to move | P to pause | ESC to quit";
    int controlsWidth = MeasureText(controls, 16);
    DrawText(controls, screenWidth / 2 - controlsWidth / 2, GetRenderHeight() - 30, 16, Fade(WHITE, 0.6f));
    
    if (state.paused && !state.gameOver) {
        DrawRectangle(0, 0, screenWidth, GetRenderHeight(), Fade(BLACK, 0.5f));
        const char* pauseText = "PAUSED";
        int pauseWidth = MeasureText(pauseText, 80);
        DrawText(pauseText, screenWidth / 2 - pauseWidth / 2, GetRenderHeight() / 2 - 40, 80, WHITE);
    }
    
    if (state.gameOver) {
        DrawRectangle(0, 0, screenWidth, GetRenderHeight(), Fade(BLACK, 0.7f));
        
        const char* resultText = state.leftWon ? "LEFT PLAYER WINS!" : "RIGHT PLAYER WINS!";
        if (mode == GameMode::VsAI) {
            resultText = state.leftWon ? "YOU WIN!" : "AI WINS!";
        }
        
        int resultWidth = MeasureText(resultText, 70);
        DrawText(resultText, screenWidth / 2 - resultWidth / 2, GetRenderHeight() / 2 - 80, 70, 
                 state.leftWon ? GREEN : RED);
        
        std::string scoreText = std::to_string(state.leftScore) + " - " + std::to_string(state.rightScore);
        int scoreWidth = MeasureText(scoreText.c_str(), 50);
        DrawText(scoreText.c_str(), screenWidth / 2 - scoreWidth / 2, GetRenderHeight() / 2, 50, WHITE);
        
        const char* continueText = "Press SPACE or ENTER to continue";
        int continueWidth = MeasureText(continueText, 25);
        DrawText(continueText, screenWidth / 2 - continueWidth / 2, GetRenderHeight() / 2 + 80, 25, Fade(WHITE, 0.7f));
    }
}
