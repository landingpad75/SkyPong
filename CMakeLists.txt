cmake_minimum_required(VERSION 3.30)

project(SkyPong)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Windows-specific defines to prevent header conflicts
if(WIN32)
    add_compile_definitions(WIN32_LEAN_AND_MEAN NOMINMAX)
endif()

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
    outside/enet/include
    outside/discord/source
    outside/raylib/include
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

find_library(DISCORD_GAME_SDK discord_game_sdk PATHS ${CMAKE_SOURCE_DIR}/lib ${CMAKE_SOURCE_DIR}/outside/discord/source)

if(MSVC)
    message("WidnwoS")
else()
    if(DISCORD_GAME_SDK)
        target_link_libraries(SkyPong ${DISCORD_GAME_SDK})
    endif()
endif()

target_include_directories(SkyPong PRIVATE
    include
    outside/enet/include
    outside/discord/source
    outside/raylib/include
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
    outside/enet/include
    outside/discord/source
    outside/raylib/include
)

target_link_libraries(SkyPongServer PRIVATE
    SPEngine
    enet
)