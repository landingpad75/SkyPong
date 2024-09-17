#include <settings.h>

// Define setTheme method
void Settings::setTheme(Theme theme) {
    gameTheme = theme;
    song = theme;
}

// Define setWindowState method
void Settings::setWindowState(GameScreen state) {
    windowState = state;
}

// Define setRPCLocation method
void Settings::setRPCLocation(RPCAt loc, matchMode mode) {
    RPC = loc;
    match = mode;
}
