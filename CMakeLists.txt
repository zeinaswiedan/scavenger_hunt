cmake_minimum_required(VERSION 3.16)
project(ScavengerHunt VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

add_executable(ScavengerHunt
    main.cpp
    LoginWindow.cpp
    LoginWindow.h
    MainWindow.cpp
    MainWindow.h
    Player.cpp
    Player.h
    Hint.cpp
    Hint.h
    Game.cpp
    Game.h
    GameWidget.cpp
    GameWidget.h
    GameStateManager.cpp
    GameStateManager.h
    Shark.cpp
    Shark.h
    GameObject.h
    LevelConfig.h
    LevelData.h
)

target_link_libraries(ScavengerHunt PRIVATE Qt6::Widgets)
