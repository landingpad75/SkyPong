#pragma once

#include <string>
#include <vector>
#include <Network/Protocol.hpp>
#include <enet/enet.h>

struct Room {
    std::string id;
    std::string hostName;
    std::vector<ENetPeer*> players;
    bool gameStarted = false;
    Protocol::GameState gameState{};
    
    Room(const std::string& roomId, const std::string& host) 
        : id(roomId), hostName(host) {
        gameState = {640.0f, 360.0f, 5.0f, 5.0f, 360.0f, 360.0f, 0, 0};
    }
};