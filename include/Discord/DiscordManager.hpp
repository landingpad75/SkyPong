#pragma once

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif

#include <discord.h>
#include <string>
#include <functional>
#include <memory>

enum class DiscordState {
    MainMenu,
    InLobby,
    InGame,
    Matchmaking
};

class DiscordManager {
public:
    static DiscordManager& Instance() {
        static DiscordManager instance;
        return instance;
    }
    
    void Init();
    void Shutdown();
    void Update();
    
    void SetState(DiscordState state, const std::string& details = "", const std::string& partyId = "");
    void SetJoinCallback(std::function<void(const std::string&)> callback);
    
    bool IsInitialized() const { return initialized; }
    std::string GetPlayerName() const { return playerName; }
    
private:
    DiscordManager() = default;
    ~DiscordManager() { Shutdown(); }
    
    DiscordManager(const DiscordManager&) = delete;
    DiscordManager& operator=(const DiscordManager&) = delete;
    
    bool initialized = false;
    DiscordState currentState = DiscordState::MainMenu;
    std::string currentPartyId;
    std::function<void(const std::string&)> joinCallback;
    
    discord::Core* core;
    std::string playerName;
    
    std::string GenerateUUID();
};
