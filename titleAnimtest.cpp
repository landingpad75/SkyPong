#include "raylib.h"

int main(void){
    const int screenWidth = 800;
    const int screenHeight = 600;
    InitWindow(screenWidth, screenHeight, "raylib animation example");

    
    SetTargetFPS(60); 

    while (!WindowShouldClose()) {
        
    }

    CloseWindow();

    return 0;
}
