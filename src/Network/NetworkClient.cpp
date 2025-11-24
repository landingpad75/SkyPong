#include <Network/NetworkClient.hpp>
#include <enet/enet.h>
#include <iostream>

NetworkClient::NetworkClient() {}

NetworkClient::~NetworkClient() {
    Shutdown();
}

bool NetworkClient::Init() {
    if (enet_initialize() != 0) {
        std::cerr << "Failed to initialize ENet" << std::endl;
        return false;
    }
    
    client = enet_host_create(nullptr, 1, 2, 0, 0);
    if (client == nullptr) {
        std::cerr << "Failed to create ENet client host" << std::endl;
        enet_deinitialize();
        return false;
    }
    
    return true;
}

void NetworkClient::Shutdown() {
    if (running.load()) {
        running.store(false);
        if (networkThread.joinable()) {
            networkThread.join();
        }
    }
    
    Disconnect();
    
    if (client != nullptr) {
        enet_host_destroy(client);
        client = nullptr;
    }
    
    enet_deinitialize();
}

bool NetworkClient::Connect(const std::string& host, uint16_t port) {
    if (connected.load()) {
        return true;
    }
    
    ENetAddress address;
    enet_address_set_host(&address, host.c_str());
    address.port = port;
    
    peer = enet_host_connect(client, &address, 2, 0);
    if (peer == nullptr) {
        PushMessage(NetworkEvent::Error, "Failed to connect to server");
        return false;
    }
    
    running.store(true);
    networkThread = std::thread(&NetworkClient::NetworkThread, this);
    
    return true;
}

void NetworkClient::Disconnect() {
    if (peer != nullptr && connected.load()) {
        enet_peer_disconnect(peer, 0);
        
        ENetEvent event;
        while (enet_host_service(client, &event, 3000) > 0) {
            if (event.type == ENET_EVENT_TYPE_DISCONNECT) {
                break;
            }
        }
        
        enet_peer_reset(peer);
        peer = nullptr;
    }
    
    connected.store(false);
}

void NetworkClient::Send(const std::string& data) {
    std::lock_guard<std::mutex> lock(sendMutex);
    sendQueue.push(data);
}

bool NetworkClient::HasMessages() const {
    return !messageQueue.empty();
}

NetworkMessage NetworkClient::PopMessage() {
    std::lock_guard<std::mutex> lock(messageMutex);
    if (messageQueue.empty()) {
        return {NetworkEvent::Error, ""};
    }
    
    NetworkMessage msg = messageQueue.front();
    messageQueue.pop();
    return msg;
}

void NetworkClient::Update() {}

void NetworkClient::NetworkThread() {
    ENetEvent event;
    
    while (running.load()) {
        {
            std::lock_guard<std::mutex> lock(sendMutex);
            while (!sendQueue.empty() && peer != nullptr) {
                const std::string& data = sendQueue.front();
                ENetPacket* packet = enet_packet_create(data.c_str(), data.length() + 1, ENET_PACKET_FLAG_RELIABLE);
                enet_peer_send(peer, 0, packet);
                sendQueue.pop();
            }
        }
        
        while (enet_host_service(client, &event, 10) > 0) {
            switch (event.type) {
                case ENET_EVENT_TYPE_CONNECT:
                    connected.store(true);
                    PushMessage(NetworkEvent::Connected, "Connected to server");
                    break;
                    
                case ENET_EVENT_TYPE_RECEIVE:
                    {
                        std::string data((char*)event.packet->data, event.packet->dataLength);
                        PushMessage(NetworkEvent::DataReceived, data);
                        enet_packet_destroy(event.packet);
                    }
                    break;
                    
                case ENET_EVENT_TYPE_DISCONNECT:
                    connected.store(false);
                    PushMessage(NetworkEvent::Disconnected, "Disconnected from server");
                    peer = nullptr;
                    break;
                    
                case ENET_EVENT_TYPE_NONE:
                    break;
            }
        }
    }
}

void NetworkClient::PushMessage(NetworkEvent event, const std::string& data) {
    std::lock_guard<std::mutex> lock(messageMutex);
    messageQueue.push({event, data});
}
