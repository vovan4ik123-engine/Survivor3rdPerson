#pragma once

namespace EnumsAndVars
{
    struct FontsPath
    {
        static const inline std::string roboto = "fonts/roboto.ttf";
        static const inline std::string cousine = "fonts/cousine.ttf";
    };

    struct AnimationIndexes
    {
        static constexpr inline int run = 0;    // 1_Run    - names in blender.
        static constexpr inline int stand = 1;  // 2_Stand
        static constexpr inline int attack = 2; // 3_Attack
        static constexpr inline int die1 = 3;   // 4_Die1
        static constexpr inline int die2 = 4;   // 5_Die2
        static constexpr inline int die3 = 5;   // 6_Die3
    };

    struct SettingsMenu
    {
        // Stored in DB.

        // Not stored in DB.
        static inline float cameraHorizontalSpeed = 432.0f;
        static inline float cameraVerticalSpeed = 155.7f;
        static inline float cameraSpeedThresholdToAccelerate = 50.0f;
    };

    // Game specific aliases for physics collision groups.
    constexpr inline Beryll::CollisionGroups CollGr_PLAYER = Beryll::CollisionGroups::GROUP_1;
    constexpr inline Beryll::CollisionGroups CollGr_STATIC_ENV = Beryll::CollisionGroups::GROUP_2;
    constexpr inline Beryll::CollisionGroups CollGr_JUMPPAD = Beryll::CollisionGroups::GROUP_3;
    constexpr inline Beryll::CollisionGroups CollGr_ENEMY = Beryll::CollisionGroups::GROUP_4;
    constexpr inline Beryll::CollisionGroups CollGr_WEAPON_BULLET = Beryll::CollisionGroups::GROUP_5;
    constexpr inline Beryll::CollisionGroups CollGr_RAY_FOR_ENV = Beryll::CollisionGroups::GROUP_6;

    // Game specific aliases for objects on scene.
    constexpr inline Beryll::SceneObjectGroups SceneGR_NONE = Beryll::SceneObjectGroups::NONE;
    constexpr inline Beryll::SceneObjectGroups SceneGR_PLAYER = Beryll::SceneObjectGroups::GROUP_1;
    constexpr inline Beryll::SceneObjectGroups SceneGR_ENEMY = Beryll::SceneObjectGroups::GROUP_2;

    // Enemies.
    constexpr inline int enemiesMaxPathfindingInOneFrame = 15;
    constexpr inline float enemiesMinDistanceSquaredToSpawn = 150.0f * 150.0f; // To use glm::distance2 and avoid sqrt().
    constexpr inline float enemiesMaxDistanceSquaredToSpawn = 500.0f * 500.0f;

    constexpr inline float enemiesDistanceSquaredRespawnAfter = 520.0f * 520.0f; // Respawn enemy closer if distance to player > ....
    constexpr inline int enemiesPathFinderStep = 10;
    inline int enemiesCurrentPathfindingIndex = 0;

    // Play.
    inline float mapPlayTimeSec = 0.0f;

    inline void reset()
    {
        // Player.

        // Enemies.
        enemiesCurrentPathfindingIndex = 0;

        // Play.
        mapPlayTimeSec = 0.0f;
    }
}
