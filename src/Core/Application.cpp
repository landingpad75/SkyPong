#include <Core/Application.hpp>
unsigned int flags = 0;


Application& Application::Init(){
    if(resizable){
        SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    }
    
    InitWindow(defaultDimensions.x, defaultDimensions.y, appName.c_str());
    InitAudioDevice();
    SetExitKey(KEY_NULL);
    SetTargetFPS(GetMonitorRefreshRate(GetCurrentMonitor()));
    
    return *this;
}

bool Application::running() { return !WindowShouldClose(); }

Application& Application::Deinit(){
    CloseAudioDevice();
    CloseWindow();

    return *this;
}