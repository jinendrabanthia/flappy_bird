cmake_minimum_required(VERSION 3.10)
project(FlappyBird)

set(CMAKE_CXX_STANDARD 17)

find_package(SFML COMPONENTS Graphics Window System Audio REQUIRED)

add_executable(FlappyBird
    main.cpp
    Game.cpp
    GameState.cpp
    Pipe.cpp
    Bird.cpp
    AssetManager.cpp
)

target_link_libraries(FlappyBird SFML::Graphics SFML::Window SFML::System SFML::Audio)
