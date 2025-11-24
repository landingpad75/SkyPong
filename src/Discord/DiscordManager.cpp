#include <Discord/DiscordManager.hpp>
#include <iostream>
#include <ctime>
#include <random>
#include <sstream>
#include <iomanip>

std::string DiscordManager::GenerateUUID() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 15);
    std::uniform_int_distribution<> dis2(8, 11);
    
    std::stringstream ss;
    ss << std::hex;
    
    for (int i = 0; i < 8; i++) {
        ss << dis(gen);
    }
    
    std::string uuid = ss.str();
    if (uuid.length() > 32) {
        uuid = uuid.substr(0, 32);
    }
    
    return "Player-" + uuid;
}

void DiscordManager::Init() {
    if (initialized) return;
    
    auto result = discord::Core::Create(1119974286392840313, DiscordCreateFlags_Default, &core);
    if (result != discord::Result::Ok || !core) {
        std::cerr << "Failed to create Discord Core! (err " << static_cast<int>(result) << ")" << std::endl;
        return;
    }
    
    core->SetLogHook(discord::LogLevel::Warn, [](discord::LogLevel level, const char* message) {
        std::cerr << "Discord Log(" << static_cast<uint32_t>(level) << "): " << message << std::endl;
    });
    
    core->ActivityManager().OnActivityJoin.Connect([this](const char* secret) {
        std::cout << "Discord: Join request with secret: " << secret << std::endl;
        if (joinCallback) {
            joinCallback(std::string(secret));
        }
    });
    
    core->ActivityManager().OnActivityJoinRequest.Connect([this](discord::User const& user) {
        std::cout << "Discord: Join request from " << user.GetUsername() << std::endl;
        core->ActivityManager().SendRequestReply(user.GetId(), discord::ActivityJoinRequestReply::Yes, 
            [](discord::Result result) {
                std::cout << "Sent join reply: " << static_cast<int>(result) << std::endl;
            });
    });
    
    core->ActivityManager().RegisterCommand("skypong");
    
    core->UserManager().OnCurrentUserUpdate.Connect([this]() {
        discord::User currentUser;
        auto result = core->UserManager().GetCurrentUser(&currentUser);
        if (result == discord::Result::Ok) {
            playerName = currentUser.GetUsername();
            std::cout << "Discord user: " << playerName << std::endl;
        } else {
            playerName = GenerateUUID();
            std::cout << "Failed to get Discord user, using generated name: " << playerName << std::endl;
        }
    });
    
    discord::User currentUser;
    auto userResult = core->UserManager().GetCurrentUser(&currentUser);
    if (userResult == discord::Result::Ok) {
        playerName = currentUser.GetUsername();
        std::cout << "Discord user: " << playerName << std::endl;
    } else {
        playerName = GenerateUUID();
        std::cout << "Discord not ready, using generated name: " << playerName << std::endl;
    }
    
    initialized = true;
    std::cout << "Discord Rich Presence initialized" << std::endl;
    
    SetState(DiscordState::MainMenu);
}

void DiscordManager::Shutdown() {
    if (!initialized) return;
    
    delete core;
    initialized = false;
    
    std::cout << "Discord Rich Presence shutdown" << std::endl;
}

void DiscordManager::Update() {
    if (!initialized || !core) return;
    
    core->RunCallbacks();
}

void DiscordManager::SetState(DiscordState state, const std::string& details, const std::string& partyId) {
    if (!initialized || !core) return;
    
    currentState = state;
    currentPartyId = partyId;
    
    discord::Activity activity{};
    
    static int64_t startTime = std::time(nullptr);
    activity.SetType(discord::ActivityType::Playing);
    activity.GetTimestamps().SetStart(startTime);
    activity.GetAssets().SetLargeImage("pong");
    activity.GetAssets().SetLargeText("SkyPong");
    
    switch (state) {
        case DiscordState::MainMenu:
            activity.SetState("In Main Menu");
            activity.SetDetails("Browsing menus");
            break;
            
        case DiscordState::InLobby:
            activity.SetState("In Lobby");
            activity.SetDetails(details.empty() ? "Looking for a match" : details.c_str());
            
            if (!partyId.empty()) {
                activity.GetSecrets().SetJoin(partyId.c_str());
                activity.GetParty().SetId(partyId.c_str());
                activity.GetParty().GetSize().SetCurrentSize(1);
                activity.GetParty().GetSize().SetMaxSize(2);
                activity.GetParty().SetPrivacy(discord::ActivityPartyPrivacy::Public);
            }
            break;
            
        case DiscordState::Matchmaking:
            activity.SetState("Matchmaking");
            activity.SetDetails("Waiting for opponent");
            
            if (!partyId.empty()) {
                activity.GetSecrets().SetJoin(partyId.c_str());
                activity.GetParty().SetId(partyId.c_str());
                activity.GetParty().GetSize().SetCurrentSize(1);
                activity.GetParty().GetSize().SetMaxSize(2);
                activity.GetParty().SetPrivacy(discord::ActivityPartyPrivacy::Public);
            }
            break;
            
        case DiscordState::InGame:
            activity.SetState("In Game");
            activity.SetDetails(details.empty() ? "Playing Pong" : details.c_str());
            
            if (!partyId.empty()) {
                activity.GetParty().SetId(partyId.c_str());
                activity.GetParty().GetSize().SetCurrentSize(2);
                activity.GetParty().GetSize().SetMaxSize(2);
            }
            break;
    }
    
    core->ActivityManager().UpdateActivity(activity, [](discord::Result result) {
        if (result != discord::Result::Ok) {
            std::cerr << "Failed to update activity: " << static_cast<int>(result) << std::endl;
        }
    });
}

void DiscordManager::SetJoinCallback(std::function<void(const std::string&)> callback) {
    joinCallback = callback;
}
