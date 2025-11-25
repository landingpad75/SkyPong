#pragma once

// WARNING: This header includes Windows headers via Discord SDK
// DO NOT include this in any file that also includes raylib.h
// Use Discord/DiscordWrapper.h instead for raylib code

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#undef DrawText
#undef LoadImage
#undef CloseWindow
#undef ShowCursor
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
