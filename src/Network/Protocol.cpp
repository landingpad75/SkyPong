#include <Network/Protocol.hpp>
#include <cstring>

namespace Protocol {
    std::string Serialize(MessageType type, const std::string& data) {
        std::string msg;
        msg.resize(3 + data.size());
        
        msg[0] = static_cast<uint8_t>(type);
        uint16_t len = static_cast<uint16_t>(data.size());
        msg[1] = (len >> 8) & 0xFF;
        msg[2] = len & 0xFF;
        
        if (!data.empty()) {
            std::memcpy(&msg[3], data.c_str(), data.size());
        }
        
        return msg;
    }
    
    bool Deserialize(const std::string& msg, MessageType& type, std::string& data) {
        if (msg.size() < 3) return false;
        
        type = static_cast<MessageType>(static_cast<uint8_t>(msg[0]));
        uint16_t len = (static_cast<uint8_t>(msg[1]) << 8) | static_cast<uint8_t>(msg[2]);
        
        if (msg.size() < 3 + len) return false;
        
        data = msg.substr(3, len);
        return true;
    }
}
