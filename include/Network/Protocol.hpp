#pragma once
#include <string>
#include <cstdint>

// Network protocol for SkyPong
namespace Protocol {
    enum class MessageType : uint8_t {
        // Client -> Server
        CreateRoom,
        JoinRoom,
        LeaveRoom,
        GameInput,
        
        // Server -> Client
        RoomCreated,
        RoomJoined,
        RoomLeft,
        RoomList,
        GameStart,
        GameState,
        PlayerDisconnected,
        Error
    };
    
    struct RoomInfo {
        std::string roomId;
        std::string hostName;
        uint8_t playerCount;
        uint8_t maxPlayers;
    };
    
    struct GameInput {
        float paddleY;
        bool moveUp;
        bool moveDown;
    };
    
    struct GameState {
        float ballX, ballY;
        float ballVelX, ballVelY;
        float player1Y, player2Y;
        uint8_t score1, score2;
    };
    
    // Simple message format: [type:1][dataLength:2][data:N]
    std::string Serialize(MessageType type, const std::string& data = "");
    bool Deserialize(const std::string& msg, MessageType& type, std::string& data);
}
