#include <pong.h>
#include <socket.h>

#define midwidth screen_width / 2
#define midheight screen_height / 2
#define GLSL_VERSION 330
#define SUPPORT_FILEFORMAT_SVG

using namespace std;

int main(void) {
    init();
    WebSocketClient client;
    client.connect("ws://localhost:6969");
    // segfault here
    std::cout << "WebSocket initialized" << std::endl;
    Image icon = LoadImage("./exit.png"); 
    std::string title = "SkyPong";
    StartWindow(icon, title);
    std::cout << "Started window" << std::endl;
    Settings settings;
    

    LoadingScreenAnim loading;
    loading.screenHeight = screen_height;
    loading.screenWidth = screen_width;
    TitleScreen titleScreen;
    titleScreen.anim.screenHeight = screen_height;
    titleScreen.anim.screenWidth = screen_width;
    update("In loading screen", "Getting Ready..");
    bool initted = true;

    while (!WindowShouldClose() && initted){
        client.time += 10;
        std::cout << client.time << std::endl;
        if(client.time > 10000){
            client.time = 0;
            client.send_message("players");
        }
        switch(settings.windowState){
            case LOADING: {
                int done = loading.UpdateAnimationData();
                if(done){
                    settings.setWindowState(TITLE);
                }
            } break;
            case TITLE: {
                if(!titleScreen.anim.done)
                    titleScreen.anim.done = titleScreen.anim.UpdateAnimationData();
                else {
                    titleScreen.Update();
                    update_match("dfhqfrdghq", "drnjsqrtj", "matching");
                }

            } break;
            default: break;
        }

        BeginDrawing();
        switch(settings.windowState){
            case LOADING: {
                loading.DrawAnimation();
            } break;
            case TITLE:{
                if(!titleScreen.anim.done)
                    titleScreen.anim.DrawAnimation();
                else {
                    titleScreen.Draw();
                    DrawText(client.online ? "Online" : "Offline", 100, 100, 40, BLACK);
                    DrawText(std::to_string(client.players).c_str(), 100, 125, 40, BLACK);
                }
            } break;
            default: break;
        }

        DrawFPS(20, 20);
        EndDrawing();
    }

    end();
    CloseWindow();
    return 0;
}
