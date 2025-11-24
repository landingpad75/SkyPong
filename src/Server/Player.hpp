#pragma once

#include <string>
#include <enet/enet.h>

struct Player {
    ENetPeer* peer;
    std::string name;
    std::string currentRoom;
};