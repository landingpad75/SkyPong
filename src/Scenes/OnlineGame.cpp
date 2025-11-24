#include <Scenes/OnlineGame.hpp>
#include <Network/Protocol.hpp>
#include <Discord/DiscordManager.hpp>
#include <iostream>
#include <sstream>

void OnlineGame::Init() {
    state.Init(GetRenderWidth(), GetRenderHeight());
    
    mySide = g_isHost ? PlayerSide::Left : PlayerSide::Right;
    
    if (!g_networkClient || !g_networkClient->IsConnected()) {
        statusText = "Not connected to server!";
        state.paused = true;
    }
}

void OnlineGame::Deinit() {}

void OnlineGame::Update(float dt) {
    DiscordManager::Instance().Update();
    
    if (IsKeyPressed(KEY_ESCAPE)) {
        mgr->ReplaceScene("vs_online");
        return;
    }
    
    if (state.gameOver) {
        if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ENTER)) {
            mgr->ReplaceScene("vs_online");
        }
        return;
    }
    
    if (!g_networkClient || !g_networkClient->IsConnected()) {
        statusText = "Connection lost";
        state.paused = true;
        return;
    }
    
    ProcessNetworkMessages();
    
    if (state.paused) return;
    
    HandleLocalInput(dt);
    
    if (g_isHost) {
        UpdateBall(dt);
        CheckCollisions();
        CheckScore();
        
        syncTimer += dt;
        if (syncTimer >= syncInterval) {
            syncTimer = 0.0f;
            SendGameState();
        }
    }
}

void OnlineGame::HandleLocalInput(float dt) {
    float moveDir = 0.0f;
    
    if (mySide == PlayerSide::Left) {
        if (IsKeyDown(KEY_W)) moveDir = -1.0f;
        if (IsKeyDown(KEY_S)) moveDir = 1.0f;
    } else {
        if (IsKeyDown(KEY_UP)) moveDir = -1.0f;
        if (IsKeyDown(KEY_DOWN)) moveDir = 1.0f;
    }
    
    if (moveDir != 0.0f) {
        Paddle& myPaddle = mySide == PlayerSide::Left ? state.leftPaddle : state.rightPaddle;
        UpdatePaddle(myPaddle, moveDir, dt);
        
        SendInput(myPaddle.y);
    }
}

void OnlineGame::SendInput(float paddleY) {
    if (!g_networkClient) return;
    
    std::ostringstream oss;
    oss << (mySide == PlayerSide::Left ? "L" : "R") << ":" << paddleY;
    
    std::string msg = Protocol::Serialize(Protocol::MessageType::GameInput, oss.str());
    g_networkClient->Send(msg);
}

void OnlineGame::SendGameState() {
    if (!g_networkClient) return;
    
    std::ostringstream oss;
    oss << state.ball.x << "," << state.ball.y << ","
        << state.ball.velX << "," << state.ball.velY << ","
        << state.leftPaddle.y << "," << state.rightPaddle.y << ","
        << state.leftScore << "," << state.rightScore;
    
    std::string msg = Protocol::Serialize(Protocol::MessageType::GameState, oss.str());
    g_networkClient->Send(msg);
}

void OnlineGame::ProcessNetworkMessages() {
    while (g_networkClient && g_networkClient->HasMessages()) {
        NetworkMessage msg = g_networkClient->PopMessage();
        
        switch (msg.event) {
            case NetworkEvent::DataReceived:
                HandleProtocolMessage(msg.data);
                break;
                
            case NetworkEvent::Disconnected:
                statusText = "Opponent disconnected";
                state.paused = true;
                break;
                
            case NetworkEvent::Error:
                statusText = "Network error";
                break;
                
            default:
                break;
        }
    }
}

void OnlineGame::HandleProtocolMessage(const std::string& data) {
    Protocol::MessageType type;
    std::string payload;
    
    if (!Protocol::Deserialize(data, type, payload)) {
        return;
    }
    
    switch (type) {
        case Protocol::MessageType::GameInput:
            HandleOpponentInput(payload);
            break;
            
        case Protocol::MessageType::GameState:
            if (!g_isHost) {
                HandleGameStateUpdate(payload);
            }
            break;
            
        case Protocol::MessageType::PlayerDisconnected:
            statusText = "Opponent disconnected";
            state.paused = true;
            break;
            
        default:
            break;
    }
}

void OnlineGame::HandleOpponentInput(const std::string& data) {
    if (data.size() < 3) return;
    
    char side = data[0];
    float paddleY = std::stof(data.substr(2));
    
    if (side == 'L' && mySide != PlayerSide::Left) {
        state.leftPaddle.y = paddleY;
    } else if (side == 'R' && mySide != PlayerSide::Right) {
        state.rightPaddle.y = paddleY;
    }
}

void OnlineGame::HandleGameStateUpdate(const std::string& data) {
    std::istringstream iss(data);
    std::string token;
    std::vector<std::string> tokens;
    
    while (std::getline(iss, token, ',')) {
        tokens.push_back(token);
    }
    
    if (tokens.size() >= 8) {
        state.ball.x = std::stof(tokens[0]);
        state.ball.y = std::stof(tokens[1]);
        state.ball.velX = std::stof(tokens[2]);
        state.ball.velY = std::stof(tokens[3]);
        state.leftPaddle.y = std::stof(tokens[4]);
        state.rightPaddle.y = std::stof(tokens[5]);
        state.leftScore = std::stoi(tokens[6]);
        state.rightScore = std::stoi(tokens[7]);
    }
}

void OnlineGame::UpdatePaddle(Paddle& paddle, float moveDir, float dt) {
    paddle.y += moveDir * paddle.speed * dt;
    
    if (paddle.y < 0) paddle.y = 0;
    if (paddle.y + paddle.height > state.screenHeight) {
        paddle.y = state.screenHeight - paddle.height;
    }
}

void OnlineGame::UpdateBall(float dt) {
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

void OnlineGame::CheckCollisions() {
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

void OnlineGame::CheckScore() {
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

void OnlineGame::Draw() {
    DrawGame();
    DrawUI();
}

void OnlineGame::DrawGame() {
    float screenWidth = GetRenderWidth();
    float screenHeight = GetRenderHeight();
    
    Color leftColor = (Color){20, 160, 133, 255};
    Color rightColor = (Color){38, 185, 154, 255};
    
    ClearBackground(leftColor);
    DrawRectangle(screenWidth / 2, 0, screenWidth / 2, screenHeight, rightColor);
    
    for (int i = 0; i < screenHeight; i += 20) {
        DrawRectangle(screenWidth / 2 - 2, i, 4, 10, WHITE);
    }
    
    Color myPaddleColor = CLITERAL(Color){255, 255, 100, 255};
    Color opponentPaddleColor = WHITE;
    
    DrawRectangle(state.leftPaddle.x, state.leftPaddle.y, 
                  state.leftPaddle.width, state.leftPaddle.height, 
                  mySide == PlayerSide::Left ? myPaddleColor : opponentPaddleColor);
    DrawRectangle(state.rightPaddle.x, state.rightPaddle.y, 
                  state.rightPaddle.width, state.rightPaddle.height,
                  mySide == PlayerSide::Right ? myPaddleColor : opponentPaddleColor);
    
    DrawCircle(state.ball.x, state.ball.y, state.ball.radius, WHITE);
}

void OnlineGame::DrawUI() {
    float screenWidth = GetRenderWidth();
    float screenHeight = GetRenderHeight();
    
    std::string leftScoreText = std::to_string(state.leftScore);
    std::string rightScoreText = std::to_string(state.rightScore);
    
    DrawText(leftScoreText.c_str(), screenWidth / 4, 40, 60, WHITE);
    DrawText(rightScoreText.c_str(), 3 * screenWidth / 4 - 30, 40, 60, WHITE);
    
    const char* roleText = g_isHost ? "HOST (Left)" : "CLIENT (Right)";
    int roleWidth = MeasureText(roleText, 20);
    DrawText(roleText, screenWidth / 2 - roleWidth / 2, 20, 20, Fade(WHITE, 0.7f));
    
    const char* controls = mySide == PlayerSide::Left ? 
        "W/S to move | ESC to quit" :
        "UP/DOWN to move | ESC to quit";
    int controlsWidth = MeasureText(controls, 16);
    DrawText(controls, screenWidth / 2 - controlsWidth / 2, screenHeight - 30, 16, Fade(WHITE, 0.6f));
    
    if (!statusText.empty()) {
        int statusWidth = MeasureText(statusText.c_str(), 25);
        DrawRectangle(0, screenHeight / 2 - 40, screenWidth, 80, Fade(BLACK, 0.7f));
        DrawText(statusText.c_str(), screenWidth / 2 - statusWidth / 2, screenHeight / 2 - 12, 25, RED);
    }
    
    const char* netStatus = g_networkClient && g_networkClient->IsConnected() ? "Connected" : "Disconnected";
    Color netColor = g_networkClient && g_networkClient->IsConnected() ? GREEN : RED;
    DrawText(netStatus, 10, 10, 16, netColor);
    
    if (state.gameOver) {
        DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, 0.7f));
        
        bool iWon = (mySide == PlayerSide::Left && state.leftWon) || 
                    (mySide == PlayerSide::Right && !state.leftWon);
        
        const char* resultText = iWon ? "YOU WIN!" : "YOU LOSE!";
        int resultWidth = MeasureText(resultText, 70);
        DrawText(resultText, screenWidth / 2 - resultWidth / 2, screenHeight / 2 - 80, 70, 
                 iWon ? GREEN : RED);
        
        std::string scoreText = std::to_string(state.leftScore) + " - " + std::to_string(state.rightScore);
        int scoreWidth = MeasureText(scoreText.c_str(), 50);
        DrawText(scoreText.c_str(), screenWidth / 2 - scoreWidth / 2, screenHeight / 2, 50, WHITE);
        
        const char* continueText = "Press SPACE or ENTER to continue";
        int continueWidth = MeasureText(continueText, 25);
        DrawText(continueText, screenWidth / 2 - continueWidth / 2, screenHeight / 2 + 80, 25, Fade(WHITE, 0.7f));
    }
}
