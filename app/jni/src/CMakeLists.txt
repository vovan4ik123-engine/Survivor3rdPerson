cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_compile_options(-fexceptions -frtti)

# for info/warning/assert messages
    add_definitions(-DBR_DEBUG)

add_library(main SHARED main.cpp
                        GameStateHelper.cpp
                        Sounds.cpp
                        # Game states.
                        playState/PlayStateGUILayer.cpp
                        playState/PlayStateSceneLayer.cpp
                        # Player.
                        player/Player.cpp
                        # Enemies.
                        enemy/BaseEnemy.cpp
                        enemy/MovableEnemy.cpp
                        pathfinding/AStar.cpp
                        # Weapons.
                        weapon/WeaponAimTrajectory.cpp
                        weapon/BaseWeapon.cpp
                        weapon/BallGun.cpp
                        weapon/ShotGun.cpp
                        weapon/Laser.cpp
            )

target_include_directories(main PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

#all other desired libs should be included in Beryll-static lib
target_link_libraries(main Beryll-static GLESv3)
