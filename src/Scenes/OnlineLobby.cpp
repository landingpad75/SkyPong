#include <Scenes/OnlineLobby.hpp>
#include <Network/Protocol.hpp>
#include <Discord/DiscordManager.hpp>
#include <iostream>
#include <cmath>

void OnlineLobby::Init() {
    DiscordManager::Instance().SetState(DiscordState::InLobby);
    
    DiscordManager::Instance().SetJoinCallback([this](const std::string& roomId) {
        if (state == LobbyState::RoomList && g_networkClient && g_networkClient->IsConnected()) {
            JoinRoom(roomId);
        }
    });
    
    if (g_networkClient && g_networkClient->IsConnected()) {
        statusText = "Connected to server";
        statusColor = GREEN;
        state = LobbyState::RoomList;
        return;
    }
    
    if (!g_networkClient) {
        g_networkClient = new NetworkClient();
    }
    
    if (!g_networkClient->Init()) {
        statusText = "Failed to initialize network";
        statusColor = RED;
        return;
    }
    
    statusText = "Connecting to server...";
    statusColor = YELLOW;
    state = LobbyState::Connecting;
    
    if (g_networkClient->Connect("de-fra04.altr.cc", 25189)) {
        messages.push_back("Attempting connection...");
    } else {
        statusText = "Connection failed";
        statusColor = RED;
        state = LobbyState::Disconnected;
    }
}

void OnlineLobby::Deinit() {
}

void OnlineLobby::Update(float dt) {
    DiscordManager::Instance().Update();
    
    if (!g_networkClient) {
        return;
    }
    
    g_networkClient->Update();
    ProcessNetworkMessages();
    
    if (state == LobbyState::Connecting) {
        connectTimer += dt;
        if (connectTimer > 5.0f && (!g_networkClient || !g_networkClient->IsConnected())) {
            statusText = "Connection timeout";
            statusColor = RED;
            state = LobbyState::Disconnected;
        }
    }
    
    if (state == LobbyState::RoomList) {
        HandleRoomListInput();
    } else if (state == LobbyState::InRoom) {
        HandleInRoomInput();
    }
    
    if (IsKeyPressed(KEY_ESCAPE)) {
        if (state == LobbyState::InRoom) {
            LeaveRoom();
        } else {
            mgr->ReplaceScene("choice_menu");
        }
    }
}

void OnlineLobby::HandleRoomListInput() {
    Vector2 mouse = GetMousePosition();
    float screenWidth = GetRenderWidth();
    float screenHeight = GetRenderHeight();
    
    Vector2 newBtnPos = {0, 0};
    float newBtnRadius = 150;
    if (CheckCollisionPointCircle(mouse, newBtnPos, newBtnRadius) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        CreateRoom();
    }
    
    Vector2 backBtnPos = {screenWidth, screenHeight};
    float backBtnRadius = 100;
    if (CheckCollisionPointCircle(mouse, backBtnPos, backBtnRadius) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        mgr->ReplaceScene("choice_menu");
    }
    
    float centerX = screenWidth / 2;
    float startY = 280;
    
    for (size_t i = 0; i < availableRooms.size(); i++) {
        Vector2 roomCenter = {centerX, startY + i * 90};
        float roomRadius = 35;
        
        if (CheckCollisionPointCircle(mouse, roomCenter, roomRadius) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            JoinRoom(availableRooms[i].roomId);
        }
    }
}

void OnlineLobby::HandleInRoomInput() {
    Vector2 mouse = GetMousePosition();
    float screenWidth = GetRenderWidth();
    float screenHeight = GetRenderHeight();
    
    Vector2 backBtnPos = {screenWidth, screenHeight};
    float backBtnRadius = 100;
    if (CheckCollisionPointCircle(mouse, backBtnPos, backBtnRadius) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        LeaveRoom();
    }
}

void OnlineLobby::CreateRoom() {
    if (!g_networkClient) return;
    
    std::string playerName = DiscordManager::Instance().GetPlayerName();
    std::string msg = Protocol::Serialize(Protocol::MessageType::CreateRoom, playerName);
    g_networkClient->Send(msg);
    statusText = "Creating room...";
    statusColor = YELLOW;
    g_isHost = true;
}

void OnlineLobby::StartGame() {
    if (!g_networkClient) {
        statusText = "No client connection";
        return;
    }
    
    mgr->ReplaceScene("online_game");
}

void OnlineLobby::JoinRoom(const std::string& roomId) {
    if (!g_networkClient) return;
    
    std::string msg = Protocol::Serialize(Protocol::MessageType::JoinRoom, roomId);
    g_networkClient->Send(msg);
    statusText = "Joining room...";
    statusColor = YELLOW;
    g_isHost = false;
}

void OnlineLobby::LeaveRoom() {
    if (!g_networkClient) return;
    
    std::string msg = Protocol::Serialize(Protocol::MessageType::LeaveRoom, "");
    g_networkClient->Send(msg);
    state = LobbyState::RoomList;
    statusText = "Connected to server";
    statusColor = GREEN;
}

void OnlineLobby::ProcessNetworkMessages() {
    while (g_networkClient && g_networkClient->HasMessages()) {
        NetworkMessage msg = g_networkClient->PopMessage();
        
        switch (msg.event) {
            case NetworkEvent::Connected:
                statusText = "Connected to server";
                statusColor = GREEN;
                state = LobbyState::RoomList;
                break;
                
            case NetworkEvent::Disconnected:
                statusText = "Disconnected";
                statusColor = RED;
                state = LobbyState::Disconnected;
                break;
                
            case NetworkEvent::DataReceived:
                HandleProtocolMessage(msg.data);
                break;
                
            case NetworkEvent::Error:
                statusText = "Error: " + msg.data;
                statusColor = RED;
                messages.push_back("Error: " + msg.data);
                break;
        }
    }
}

void OnlineLobby::HandleProtocolMessage(const std::string& data) {
    Protocol::MessageType type;
    std::string payload;
    
    if (!Protocol::Deserialize(data, type, payload)) {
        return;
    }
    
    switch (type) {
        case Protocol::MessageType::RoomList:
            ParseRoomList(payload);
            break;
            
        case Protocol::MessageType::RoomCreated:
            currentRoomId = payload;
            state = LobbyState::InRoom;
            statusText = "Room created: " + payload;
            statusColor = GREEN;
            messages.push_back("Waiting for opponent...");
            
            DiscordManager::Instance().SetState(DiscordState::Matchmaking, "Waiting for opponent", payload);
            break;
            
        case Protocol::MessageType::RoomJoined:
            currentRoomId = payload;
            state = LobbyState::InRoom;
            statusText = "Joined room: " + payload;
            statusColor = GREEN;
            messages.push_back("Waiting for game to start...");
            
            DiscordManager::Instance().SetState(DiscordState::Matchmaking, "Joined room", payload);
            break;
            
        case Protocol::MessageType::GameStart:
            messages.push_back("Game starting!");
            
            DiscordManager::Instance().SetState(DiscordState::InGame, "Online Match", currentRoomId);
            
            StartGame();
            break;
            
        case Protocol::MessageType::PlayerDisconnected:
            messages.push_back("Opponent disconnected");
            state = LobbyState::RoomList;
            break;
            
        case Protocol::MessageType::Error:
            statusText = "Error: " + payload;
            statusColor = RED;
            messages.push_back("Error: " + payload);
            break;
            
        default:
            break;
    }
}

void OnlineLobby::ParseRoomList(const std::string& data) {
    availableRooms.clear();
    
    if (data.empty()) return;
    
    size_t pos = 0;
    while (pos < data.size()) {
        size_t end = data.find(';', pos);
        if (end == std::string::npos) break;
        
        std::string roomData = data.substr(pos, end - pos);
        
        size_t colon1 = roomData.find(':');
        size_t colon2 = roomData.find(':', colon1 + 1);
        
        if (colon1 != std::string::npos && colon2 != std::string::npos) {
            Protocol::RoomInfo room;
            room.roomId = roomData.substr(0, colon1);
            room.hostName = roomData.substr(colon1 + 1, colon2 - colon1 - 1);
            room.playerCount = std::stoi(roomData.substr(colon2 + 1));
            room.maxPlayers = 2;
            
            availableRooms.push_back(room);
        }
        
        pos = end + 1;
    }
}

void OnlineLobby::Draw() {
    float screenWidth = GetRenderWidth();
    float screenHeight = GetRenderHeight();

    Color leftColor = Color {20, 160, 133, 255};
    Color rightColor = Color {38, 185, 154, 255};
    
    ClearBackground(leftColor);
    DrawRectangle(screenWidth / 2, 0, screenWidth / 2, screenHeight, rightColor);
    DrawLine(screenWidth / 2, 0, screenWidth / 2, screenHeight, WHITE);
    
    Vector2 mouse = GetMousePosition();
    
    Vector2 newBtnPos = {0, 0};
    float newBtnRadius = 150;
    bool newHovered = CheckCollisionPointCircle(mouse, newBtnPos, newBtnRadius);
    
    DrawCircle(0, 0, 151, WHITE);
    DrawCircle(0, 0, 150, newHovered ? Color {100, 230, 200, 255} : Color {129, 204, 184, 255});
    
    const char* newText = "NEW";
    int newFontSize = 35;
    int newWidth = MeasureText(newText, newFontSize);
    DrawText(newText, 75 - newWidth / 2, 75 - newFontSize / 2, newFontSize, WHITE);
    
    Vector2 backBtnPos = {screenWidth, screenHeight};
    float backBtnRadius = 100;
    bool backHovered = CheckCollisionPointCircle(mouse, backBtnPos, backBtnRadius);
    
    DrawCircle(screenWidth, screenHeight, 101, WHITE);
    DrawCircle(screenWidth, screenHeight, 100, backHovered ? Color {100, 230, 200, 255} : Color{129, 204, 184, 255});
    
    const char* backText = state == LobbyState::InRoom ? "LEAVE" : "BACK";
    int backFontSize = 28;
    int backWidth = MeasureText(backText, backFontSize);
    DrawText(backText, screenWidth - 70 - backWidth / 2, screenHeight - 70 - backFontSize / 2, backFontSize, WHITE);

    const char* title = "Online Lobby";
    int titleFontSize = 50;
    int titleWidth = MeasureText(title, titleFontSize);
    DrawText(title, screenWidth / 2 - titleWidth / 2, 40, titleFontSize, WHITE);
    
    int statusWidth = MeasureText(statusText.c_str(), 25);
    DrawText(statusText.c_str(), screenWidth / 2 - statusWidth / 2, 110, 25, statusColor);
    
    if (state == LobbyState::RoomList) {
        DrawRoomList();
    } else if (state == LobbyState::InRoom) {
        DrawInRoom();
    }
}

void OnlineLobby::DrawRoomList() {
    float screenWidth = GetRenderWidth();
    float screenHeight = GetRenderHeight();
    Vector2 mouse = GetMousePosition();
    
    float centerX = screenWidth / 2;
    float startY = 280;
    
    if (availableRooms.empty()) {
        const char* emptyText = "No rooms available";
        int emptyWidth = MeasureText(emptyText, 25);
        DrawText(emptyText, centerX - emptyWidth / 2, startY + 50, 25, Fade(WHITE, 0.7f));
        
        DrawCircle(centerX - 150, startY + 50, 8, Fade(WHITE, 0.3f));
        DrawCircle(centerX + 150, startY + 50, 8, Fade(WHITE, 0.3f));
    } else {
        for (size_t i = 0; i < availableRooms.size(); i++) {
            const auto& room = availableRooms[i];
            float yPos = startY + i * 90;
            
            Vector2 roomCenter = {centerX, yPos};
            float circleRadius = 35;
            bool roomHovered = CheckCollisionPointCircle(mouse, roomCenter, circleRadius);
            
            float containerWidth = 500;
            float containerHeight = 70;
            float containerX = centerX - containerWidth / 2;
            float containerY = yPos - containerHeight / 2;
            
            Color bgColor = roomHovered ? Fade(Color {70, 200, 180, 255}, 0.5f) : Fade(Color {50, 180, 160, 255}, 0.4f);
            DrawRectangle(containerX + 35, containerY, containerWidth - 70, containerHeight, bgColor);
            DrawCircle(containerX + 35, yPos, 35, bgColor);
            DrawCircle(containerX + containerWidth - 35, yPos, 35, bgColor);
            
            DrawCircle(containerX + 35, yPos, 36, WHITE);
            DrawCircle(containerX + 35, yPos, 35, bgColor);
            DrawCircle(containerX + containerWidth - 35, yPos, 36, WHITE);
            DrawCircle(containerX + containerWidth - 35, yPos, 35, bgColor);
            
            DrawRectangle(containerX + 35, containerY - 1, containerWidth - 70, 2, WHITE);
            DrawRectangle(containerX + 35, containerY + containerHeight - 1, containerWidth - 70, 2, WHITE);
            
            std::string roomName = room.hostName;
            std::string roomId = room.roomId;
            std::string playerInfo = std::to_string(room.playerCount) + "/" + std::to_string(room.maxPlayers);
            
            DrawText(roomName.c_str(), containerX + 60, yPos - 25, 24, WHITE);
            DrawText(roomId.c_str(), containerX + 60, yPos + 2, 18, Fade(WHITE, 0.7f));
            
            float badgeX = containerX + containerWidth - 80;
            DrawCircle(badgeX, yPos, 25, Fade(WHITE, 0.3f));
            int playerWidth = MeasureText(playerInfo.c_str(), 20);
            DrawText(playerInfo.c_str(), badgeX - playerWidth / 2, yPos - 10, 20, WHITE);
            
            if (i < availableRooms.size() - 1) {
                DrawLine(centerX, yPos + 35, centerX, yPos + 55, Fade(WHITE, 0.2f));
            }
        }
    }
}

void OnlineLobby::DrawInRoom() {
    float screenWidth = GetRenderWidth();
    float screenHeight = GetRenderHeight();
    
    float centerX = screenWidth / 2;
    float centerY = screenHeight / 2;
    
    float containerRadius = 220;
    DrawCircle(centerX, centerY, containerRadius + 2, WHITE);
    DrawCircle(centerX, centerY, containerRadius, Fade(Color {40, 170, 150, 255}, 0.6f));
    
    DrawCircle(centerX, centerY, containerRadius - 20, Fade(WHITE, 0.1f));
    
    std::string roomText = currentRoomId;
    int roomWidth = MeasureText(roomText.c_str(), 32);
    DrawText(roomText.c_str(), centerX - roomWidth / 2, centerY - 120, 32, WHITE);
    
    DrawCircle(centerX - roomWidth / 2 - 20, centerY - 105, 5, Fade(WHITE, 0.5f));
    DrawCircle(centerX + roomWidth / 2 + 20, centerY - 105, 5, Fade(WHITE, 0.5f));
    
    int yOffset = -40;
    for (size_t i = 0; i < messages.size(); i++) {
        const auto& msg = messages[i];
        int msgWidth = MeasureText(msg.c_str(), 22);
        DrawText(msg.c_str(), centerX - msgWidth / 2, centerY + yOffset, 22, WHITE);
        yOffset += 35;
    }
    
    static float pulseTimer = 0;
    pulseTimer += GetFrameTime() * 3;
    
    for (int i = 0; i < 3; i++) {
        float pulse = fabs(sin(pulseTimer + i * 0.5f));
        float dotRadius = 6 + pulse * 4;
        DrawCircle(centerX - 30 + i * 30, centerY + 80, dotRadius, Fade(WHITE, 0.5f + pulse * 0.5f));
    }
}