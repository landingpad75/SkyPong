#pragma once
#include <Core/Scene.hpp>
#include <Core/SceneManager.hpp>
#include <Network/NetworkClient.hpp>
#include <Network/Protocol.hpp>
#include <raylib.h>
#include <string>
#include <vector>
#include <memory>

enum class LobbyState {
    Disconnected,
    Connecting,
    RoomList,
    InRoom
};

class OnlineLobby : public Scene {
public:
    OnlineLobby(SceneManager* mgr) : mgr(mgr) {}
    ~OnlineLobby() { Deinit(); }

    void Init() override;
    void Deinit() override;
    void Update(float dt) override;
    void Draw() override;

private:
    SceneManager* mgr;
    
    LobbyState state = LobbyState::Disconnected;
    std::vector<std::string> messages;
    std::string statusText = "Not connected";
    Color statusColor = RED;
    
    float connectTimer = 0.0f;
    
    // Room management
    std::vector<Protocol::RoomInfo> availableRooms;
    std::string currentRoomId;
    
    void ProcessNetworkMessages();
    void HandleProtocolMessage(const std::string& data);
    void ParseRoomList(const std::string& data);
    
    void HandleRoomListInput();
    void HandleInRoomInput();
    
    void CreateRoom();
    void JoinRoom(const std::string& roomId);
    void LeaveRoom();
    
    void DrawRoomList();
    void DrawInRoom();
    void StartGame();
};
