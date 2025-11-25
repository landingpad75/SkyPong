#pragma once

// C-style wrapper to avoid exposing Discord/Windows headers
// This allows raylib code to call Discord functions without header conflicts

#ifdef __cplusplus
extern "C" {
#endif

void Discord_Init();
void Discord_Update();
void Discord_Shutdown();
const char* Discord_GetPlayerName();

#ifdef __cplusplus
}
#endif
