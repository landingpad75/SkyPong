#pragma once
#include <enet/enet.h>
#include <string>
#include <functional>
#include <thread>
#include <atomic>
#include <mutex>
#include <queue>

enum class NetworkEvent {
    Connected,
    Disconnected,
    DataReceived,
    Error
};

struct NetworkMessage {
    NetworkEvent event;
    std::string data;
};

class NetworkClient {
public:
    NetworkClient();
    ~NetworkClient();

    bool Init();
    void Shutdown();
    bool Connect(const std::string& host, uint16_t port);
    void Disconnect();
    void Send(const std::string& data);
    
    bool IsConnected() const { return connected.load(); }
    bool HasMessages() const;
    NetworkMessage PopMessage();
    
    void Update();
    


private:
    void NetworkThread();
    void PushMessage(NetworkEvent event, const std::string& data = "");

    ENetHost* client = nullptr;
    ENetPeer* peer = nullptr;
    
    std::atomic<bool> connected{false};
    std::atomic<bool> running{false};
    std::thread networkThread;
    
    std::mutex messageMutex;
    std::queue<NetworkMessage> messageQueue;
    
    std::mutex sendMutex;
    std::queue<std::string> sendQueue;
};

// Global network client for online play
extern NetworkClient* g_networkClient;
extern bool g_isHost;
