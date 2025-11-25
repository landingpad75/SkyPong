#include <Core/Application.hpp>
#include <Core/Scope.hpp>
#include <Scenes/MainMenu.hpp>
#include <Scenes/ChoiceMenu.hpp>
#include <Scenes/OnlineLobby.hpp>
#include <Scenes/PongGame.hpp>
#include <Scenes/OnlineGame.hpp>
#include <Game/GameState.hpp>
#include <Network/NetworkClient.hpp>
#include <Discord/DiscordWrapper.h>

Application app;
NetworkClient* g_networkClient = nullptr;
bool g_isHost = false;

int main(){
    Discord_Init();
    app
      .setFullscreen(false)
      .setResizable(true)
      .setName("SkyPong")
      .setDims({1280, 720})
      .Init();

    app.mgr.AddScene(Scope(ChoiceMenu), "choice_menu");
    app.mgr.AddScene(Scope(OnlineLobby), "vs_online");
    app.mgr.AddScene(ScopeArgs(PongGame, GameMode::VsAI), "vs_ai");
    app.mgr.AddScene(ScopeArgs(PongGame, GameMode::VsFriend), "vs_friend");
    app.mgr.AddScene(Scope(OnlineGame), "online_game");
    app.mgr.AddTopScene(Scope(MainMenu), "main_menu");
    
    while (app.running()) {
        Discord_Update();
        app.mgr.Update();

        BeginDrawing();
            app.mgr.Draw();
        EndDrawing();
    }
    
    Discord_Shutdown();
    
    if (g_networkClient) {
        g_networkClient->Shutdown();
        delete g_networkClient;
        g_networkClient = nullptr;
    }

    app.Deinit();
}