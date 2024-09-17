#include <pong.h>
#define midwidth screen_width / 2
#define midheight screen_height / 2
#define GLSL_VERSION            330

using namespace std;


int main(void) {
    Image icon = LoadImage("./resources/icons/icon.png");
    char* title = "SkyPong";
    StartWindow(icon, title);
    Settings settings;
    LoadingScreenAnim loading;
    loading.screenHeight = screen_height;
    loading.screenWidth = screen_width;
    TitleScreen titleScreen;
    titleScreen.anim.screenHeight = screen_height;
    titleScreen.anim.screenWidth = screen_width;

    while (!WindowShouldClose()){
        switch(settings.windowState){
            case LOADING: {
                int done = loading.UpdateAnimationData();
                if(done){
                    settings.setWindowState(TITLE);
                    SetTargetFPS(60);    
                }
            } break;
            case TITLE: {
                if(!titleScreen.anim.done)
                    titleScreen.anim.done = titleScreen.anim.UpdateAnimationData();
                
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
                else 
                    titleScreen.Draw();
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

/*
    char* Themetext;
        

    BeginDrawing();
        switch(windowState){
            case LOADING: {
                ClearBackground(BLACK);
                DrawTexture(logo, 0, 0, BLACK);
            } break;
            case TITLE: {

        player.automatic = true;
        player2.automatic = true;
        int idk = 0;
        LoadGame(ball, gameTheme, player, player2, idk, idk);
        DrawRectangleRec(settingsButton, isPressed.settings ? btclr2 : btclr1);
        DrawRectangleRec(playButton, isPressed.play ? btclr2 : btclr1);
        DrawRectangleRec(editorButton, isPressed.editor ? btclr2 : btclr1);
        DrawText("Settings", settingsButton.x + buttonSize / 2 - MeasureText("Settings", 20) / 2, settingsButton.y + buttonSize / 2 - 10, 20, WHITE);
        DrawText("Play", playButton.x + buttonSize / 2 - MeasureText("Play", 20) / 2, playButton.y + buttonSize / 2 - 10, 20, WHITE);
        DrawText("Quit", editorButton.x + buttonSize / 2 - MeasureText("Quit", 20) / 2, editorButton.y + buttonSize / 2 - 10, 20, WHITE);   
            } break;
            case GAME: {
        GameModeChooser(gameTheme);
            } break;
            case GAMEVSCPU: {
            player2.automatic = true;
            Vector2 score = LoadGame(ball, gameTheme, player, player2, playerScore, cpuScore);
            playerScore = score.x;
            cpuScore = score.y;
            } break;
            case PAUSE: {
        DrawTable(gameTheme);
        switch(pausedFrom){
            case GAMEVSCPU: {
                player2.Draw();
            } break;
            case GAMEVSPLAYER: {
                player2.Draw();
            } break;
			default:
				break;
        }
        ball.Draw();
        player.Draw();
        DrawText("Paused", screen_width / 2 - MeasureText("Paused", 50) / 2, 20, 50, BLACK);
         DrawRectangleRec(settingsButton, isPressed.settings ? btclr2 : btclr1);
         DrawRectangleRec(playButton, isPressed.play ? btclr2 : btclr1);
         DrawRectangleRec(editorButton, isPressed.editor ? btclr2 : btclr1);

        DrawText("Settings", settingsButton.x + buttonSize / 2 - MeasureText("Settings", 20) / 2, settingsButton.y + buttonSize / 2 - 10, 20, WHITE);
        DrawText("Resume", playButton.x + buttonSize / 2 - MeasureText("Resume", 20) / 2, playButton.y + buttonSize / 2 - 10, 20, WHITE);
        DrawText("Menu", editorButton.x + buttonSize / 2 - MeasureText("Menu", 20) / 2, editorButton.y + buttonSize / 2 - 10, 20, WHITE);
            } break;
            case GAMEVSPLAYER: {
		        player2.automatic = false;
                LoadGame(ball, gameTheme, player, player2, playerScore, cpuScore);
            } break;
            case TIMED: {
                LoadTimedScreen(ball, player2, player, gameTheme, playerScore, cpuScore);
            } break;
            case SETTINGS:{
                toggleSize = 57;
                int safe = voice * 100;
            DrawTable(gameTheme);
            DrawText("Settings", screen_width / 2 - MeasureText("Settings", 30) / 2, 20, 30, BLACK);
            DrawText(TextFormat("%i", safe), 84, 77, 51, WHITE);
            DrawText("Volume", 53, 18, 32, WHITE);
            DrawText("Theme", 52, 189, 37, WHITE);
            DrawText(">", 160, 65, 77, WHITE);
            DrawText("<", 27, 65, 77, WHITE);
            DrawText(Themetext, 41, 257, 41, WHITE);

            } break;
            default: break;
        }
        
        EndDrawing();
 */