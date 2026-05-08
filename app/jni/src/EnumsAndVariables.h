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

    // Player.
    constexpr inline float playerMass = 1.0f;
    constexpr inline float playerStartHP = 100.0f;
    constexpr inline glm::vec3 playerGravity{0.0f, -70.0f, 0.0f};

    // Player bullet.
    constexpr inline float bulletMass = 0.001f;
    constexpr inline glm::vec3 bulletGravity{0.0f, -10.0f, 0.0f};
    inline float shotTimeSec = -9999.0f;
    constexpr inline float shotDelaySec = 0.05f;

    // Enemies.
    constexpr inline int enemiesMaxPathfindingInOneFrame = 10;
    constexpr inline float enemiesMinDistanceToSpawn = 150.0f;
    constexpr inline float enemiesMaxDistanceToSpawn = 500.0f;
    constexpr inline float enemiesDistanceRespawnAfter = enemiesMaxDistanceToSpawn + 10.0f; // Respawn enemy closer if distance to player > ....
    inline int enemiesCurrentPathfindingIndex = 0;

    // Play.
    inline float mapPlayTimeSec = 0.0f;

    // Path finding.
    constexpr inline int pathFinderStep = 10;

    inline void reset()
    {
        // Player.

        // Player bullet.
        shotTimeSec = -9999.0;

        // Enemies.
        enemiesCurrentPathfindingIndex = 0;

        // Play.
        mapPlayTimeSec = 0.0f;
    }
}
