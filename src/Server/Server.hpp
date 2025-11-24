#pragma once

#include <unordered_map>
#include <string>
#include <memory>
#include <cstdint>
#include <enet/enet.h>

#include "Room.hpp"
#include "Player.hpp"

class Server {
public:
    bool Init();
    void Run();
    void Shutdown();

private:
    static constexpr uint16_t SERVER_PORT = 25189;
    static constexpr size_t MAX_CLIENTS = 256;
    ENetHost* server = nullptr;
    bool running = true;
    std::unordered_map<ENetPeer*, Player> players;
    std::unordered_map<std::string, std::unique_ptr<Room>> rooms;
    uint32_t nextRoomId = 1;
    
    void SendError(ENetPeer* peer, const std::string& error);
    void BroadcastRoomList();
    void SendRoomList(ENetPeer* peer);
    void LeaveRoom(ENetPeer* peer);
    
    void HandleConnect(ENetPeer* peer);
    void HandleDisconnect(ENetPeer* peer);

    void HandleReceive(ENetPeer* peer, ENetPacket* packet);

    void HandleCreateRoom(ENetPeer* peer, const std::string& hostName);
    void HandleJoinRoom(ENetPeer* peer, const std::string& roomId);
    void HandleGameInput(ENetPeer* peer, const std::string& data);
    void HandleGameState(ENetPeer* peer, const std::string& data);
};