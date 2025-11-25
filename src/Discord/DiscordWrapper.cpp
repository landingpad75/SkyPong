#include <Discord/DiscordManager.hpp>

// This file exists solely to isolate Discord/Windows headers from raylib
// It provides C-style functions that can be called without including Discord headers

extern "C" {
    void Discord_Init() {
        DiscordManager::Instance().Init();
    }
    
    void Discord_Update() {
        DiscordManager::Instance().Update();
    }
    
    void Discord_Shutdown() {
        DiscordManager::Instance().Shutdown();
    }
    
    const char* Discord_GetPlayerName() {
        static std::string name;
        name = DiscordManager::Instance().GetPlayerName();
        return name.c_str();
    }
}
