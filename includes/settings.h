#pragma once

#include <raylib.h>
#include <common.h>

class Settings {
public:
    Theme gameTheme = NORMAL;
	Theme song = NORMAL;

	GameScreen pausedFrom = GameScreen::TITLE;
	GameScreen windowState = LOADING;
	RPCAt RPC = NO;
	matchMode match = NONE;

    // Setter for the theme
    void setTheme(Theme theme);

    // Setter for the window state
    void setWindowState(GameScreen state);

    // Setter for RPC location and match mode
    void setRPCLocation(RPCAt loc, matchMode mode = NONE);
};