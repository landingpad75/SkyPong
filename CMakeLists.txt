cmake_minimum_required(VERSION 3.40)

project(SkyPong)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(outside/raylib)
add_subdirectory(outside/discord)
add_subdirectory(outside/enet)

add_library(SPEngine STATIC
    src/Core/Application.cpp
    src/Core/SceneManager.cpp
    src/Network/NetworkClient.cpp
    src/Network/Protocol.cpp
    src/Discord/DiscordManager.cpp
)

target_include_directories(SPEngine PRIVATE
    include
)

target_link_libraries(SPEngine PRIVATE raylib enet discord)

add_executable(SkyPong
    src/Scenes/ChoiceMenu.cpp
    src/Scenes/MainMenu.cpp
    src/Scenes/OnlineLobby.cpp
    src/Scenes/PongGame.cpp
    src/Scenes/OnlineGame.cpp
    src/main.cpp
)

target_include_directories(SkyPong PRIVATE
    include
)

target_link_libraries(SkyPong PRIVATE
    SPEngine
    raylib
    enet
    discord
)

add_executable(SkyPongServer
    src/Server/Server.cpp
    src/Server/main.cpp
)

target_include_directories(SkyPongServer PRIVATE
    include
)

target_link_libraries(SkyPongServer PRIVATE
    SPEngine
    enet
)