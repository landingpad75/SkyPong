#include <enet/enet.h>
#include <Network/Protocol.hpp>
#include <print>
#include <algorithm>
#include "Server.hpp"

bool Server::Init() {
    if (enet_initialize() != 0) {
        std::println("ENet failed to initialize");
        return false;
    }
    
    ENetAddress address;
    address.host = ENET_HOST_ANY;
    address.port = SERVER_PORT;
    
    server = enet_host_create(&address, MAX_CLIENTS, 2, 0, 0);
    if (server == nullptr) {
        std::println("Failed to create ENet Server");
        enet_deinitialize();
        return false;
    }

    std::println("Server ready, port: {}", SERVER_PORT);
    return true;
}

void Server::HandleConnect(ENetPeer* peer) {
    std::println("Player Connected");
    players[peer] = Player{peer, "Player", ""};
    SendRoomList(peer);
}

void Server::HandleReceive(ENetPeer* peer, ENetPacket* packet) {
    std::string msg((char*)packet->data, packet->dataLength);
    Protocol::MessageType type;
    std::string data;
    
    if (!Protocol::Deserialize(msg, type, data)) {
        return;
    }
    
    switch (type) {
        case Protocol::MessageType::CreateRoom:
            HandleCreateRoom(peer, data);
            break;
            
        case Protocol::MessageType::JoinRoom:
            HandleJoinRoom(peer, data);
            break;
            
        case Protocol::MessageType::LeaveRoom:
            LeaveRoom(peer);
            break;
            
        case Protocol::MessageType::GameInput:
            HandleGameInput(peer, data);
            break;
            
        case Protocol::MessageType::GameState:
            HandleGameState(peer, data);
            break;
            
        default:
            break;
    }
}

void Server::HandleCreateRoom(ENetPeer* peer, const std::string& hostName) {
    std::string roomId = "ROOM" + std::to_string(nextRoomId++);
    
    auto room = std::make_unique<Room>(roomId, hostName);
    room->players.push_back(peer);
    
    players[peer].currentRoom = roomId;
    players[peer].name = hostName;
    
    rooms[roomId] = std::move(room);
    
    std::println("Room created: {} by {}", roomId, hostName);
    
    std::string response = Protocol::Serialize(Protocol::MessageType::RoomCreated, roomId);
    ENetPacket* packet = enet_packet_create(response.c_str(), response.length(), ENET_PACKET_FLAG_RELIABLE);
    enet_peer_send(peer, 0, packet);
    
    BroadcastRoomList();
}

void Server::HandleJoinRoom(ENetPeer* peer, const std::string& roomId) {
    auto roomIt = rooms.find(roomId);
    if (roomIt == rooms.end()) {
        SendError(peer, "Room not found");
        return;
    }
    
    Room* room = roomIt->second.get();
    if (room->players.size() >= 2) {
        SendError(peer, "Room is full");
        return;
    }
    
    room->players.push_back(peer);
    players[peer].currentRoom = roomId;

    std::println("Player joined {}", roomId);
    
    std::string response = Protocol::Serialize(Protocol::MessageType::RoomJoined, roomId);
    ENetPacket* packet = enet_packet_create(response.c_str(), response.length(), ENET_PACKET_FLAG_RELIABLE);
    enet_peer_send(peer, 0, packet);
    
    if (room->players.size() == 2) {
        room->gameStarted = true;
        std::string startMsg = Protocol::Serialize(Protocol::MessageType::GameStart, "");
        
        for (auto* player : room->players) {
            ENetPacket* startPacket = enet_packet_create(startMsg.c_str(), startMsg.length(), ENET_PACKET_FLAG_RELIABLE);
            enet_peer_send(player, 0, startPacket);
        }
        
        std::println("Game started in {}", roomId);
    }
    
    BroadcastRoomList();
}

void Server::LeaveRoom(ENetPeer* peer) {
    auto playerIt = players.find(peer);
    if (playerIt == players.end() || playerIt->second.currentRoom.empty()) {
        return;
    }
    
    std::string roomId = playerIt->second.currentRoom;
    auto roomIt = rooms.find(roomId);
    
    if (roomIt != rooms.end()) {
        Room* room = roomIt->second.get();
        room->players.erase(
            std::remove(room->players.begin(), room->players.end(), peer),
            room->players.end()
        );
        
        if (!room->players.empty()) {
            std::string msg = Protocol::Serialize(Protocol::MessageType::PlayerDisconnected, "");
            for (auto* player : room->players) {
                ENetPacket* packet = enet_packet_create(msg.c_str(), msg.length(), ENET_PACKET_FLAG_RELIABLE);
                enet_peer_send(player, 0, packet);
            }
        }
        
        if (room->players.empty()) {
            rooms.erase(roomIt);
            std::println("Deleted {}", roomId);
        }
    }
    
    playerIt->second.currentRoom.clear();
    BroadcastRoomList();
}

void Server::HandleGameInput(ENetPeer* peer, const std::string& data) {
    auto playerIt = players.find(peer);
    if (playerIt == players.end() || playerIt->second.currentRoom.empty()) {
        return;
    }
    
    std::string roomId = playerIt->second.currentRoom;
    auto roomIt = rooms.find(roomId);
    if (!rooms.contains(roomId) || !roomIt->second->gameStarted) {
        return;
    }
    
    std::string msg = Protocol::Serialize(Protocol::MessageType::GameInput, data);
    Room* room = roomIt->second.get();
    
    for (auto* player : room->players) {
        if (player != peer) {
            ENetPacket* packet = enet_packet_create(msg.c_str(), msg.length(), ENET_PACKET_FLAG_RELIABLE);
            enet_peer_send(player, 0, packet);
        }
    }
}

void Server::HandleGameState(ENetPeer* peer, const std::string& data) {
    auto playerIt = players.find(peer);
    if (playerIt == players.end() || playerIt->second.currentRoom.empty()) {
        return;
    }
    
    std::string roomId = playerIt->second.currentRoom;
    auto roomIt = rooms.find(roomId);
    if (roomIt == rooms.end() || !roomIt->second->gameStarted) {
        return;
    }
    
    std::string msg = Protocol::Serialize(Protocol::MessageType::GameState, data);
    Room* room = roomIt->second.get();
    
    for (auto* player : room->players) {
        if (player != peer) {
            ENetPacket* packet = enet_packet_create(msg.c_str(), msg.length(), ENET_PACKET_FLAG_RELIABLE);
            enet_peer_send(player, 0, packet);
        }
    }
}

void Server::SendRoomList(ENetPeer* peer) {
    std::string roomList;
    for (const auto& [id, room] : rooms) {
        if (!room->gameStarted) {
            roomList += id + ":" + room->hostName + ":" + std::to_string(room->players.size()) + ";";
        }
    }
    
    std::string msg = Protocol::Serialize(Protocol::MessageType::RoomList, roomList);
    ENetPacket* packet = enet_packet_create(msg.c_str(), msg.length(), ENET_PACKET_FLAG_RELIABLE);
    enet_peer_send(peer, 0, packet);
}

void Server::HandleDisconnect(ENetPeer* peer) {
    std::println("Player died");
    
    auto it = players.find(peer);
    if (it != players.end()) {
        if (!it->second.currentRoom.empty()) {
            LeaveRoom(peer);
        }
        players.erase(it);
    }
}

void Server::BroadcastRoomList() {
    for (const auto& [peer, player] : players) {
        if (player.currentRoom.empty()) {
            SendRoomList(peer);
        }
    }
}

void Server::SendError(ENetPeer* peer, const std::string& error) {
    std::string msg = Protocol::Serialize(Protocol::MessageType::Error, error);
    ENetPacket* packet = enet_packet_create(msg.c_str(), msg.length(), ENET_PACKET_FLAG_RELIABLE);
    enet_peer_send(peer, 0, packet);
}

void Server::Run() {
    while (running) {
        ENetEvent event;
        
        while (enet_host_service(server, &event, 1000) > 0) {
            switch (event.type) {
                case ENET_EVENT_TYPE_CONNECT:
                    HandleConnect(event.peer);
                    break;
                    
                case ENET_EVENT_TYPE_RECEIVE:
                    HandleReceive(event.peer, event.packet);
                    enet_packet_destroy(event.packet);
                    break;
                    
                case ENET_EVENT_TYPE_DISCONNECT:
                    HandleDisconnect(event.peer);
                    break;
                    
                case ENET_EVENT_TYPE_NONE:
                    break;
            }
        }
    }
}

void Server::Shutdown() {
    if (server) {
        enet_host_destroy(server);
    }
    enet_deinitialize();
}