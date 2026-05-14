#pragma once

#include "EngineHeaders.h"
#include "PlayStateGUILayer.h"
#include "player/Player.h"
#include "enemy/BaseEnemy.h"
#include "pathfinding/AStar.h"
#include "weapon/BallGun.h"

namespace Survivor3rdPerson
{
    class PlayStateSceneLayer : public Beryll::Layer
    {
    public:
        PlayStateSceneLayer(std::shared_ptr<PlayStateGUILayer> gui);
        ~PlayStateSceneLayer() override;

        void updateBeforePhysics() override;
        void updateAfterPhysics() override;
        void draw() override;

    private:
        void loadPlayerAndWeapon();
        void loadEnv();
        void loadEnemies();
        void loadShadersAndLight();
        void handleControls();
        void handleCamera();
        void checkMapBorders();
        void updatePathfindingAndSpawnEnemies();
        void spawnEnemies();
        void handleEnemiesAttacks();

        std::shared_ptr<PlayStateGUILayer> m_gui;

        std::shared_ptr<Player> m_player;
        std::shared_ptr<BaseWeapon> m_playersWeapon;
        std::vector<std::shared_ptr<BaseEnemy>> m_movableEnemiesToSort; // This array will sorted many times.
        std::vector<std::shared_ptr<BaseEnemy>> m_movableEnemiesOriginalOrder; // This must keep always same order as loaded.
        std::vector<std::shared_ptr<BaseEnemy>> m_movableEnemiesToSpawn;
        std::vector<std::shared_ptr<Beryll::SceneObject>> m_animatedOrDynamicObjects;
        std::vector<std::shared_ptr<Beryll::BaseSimpleObject>> m_staticEnv;
        std::vector<std::shared_ptr<Beryll::BaseSimpleObject>> m_objWithNormalMap;
        std::vector<std::shared_ptr<Beryll::BaseSimpleObject>> m_simpleObjForShadowMap;
        std::vector<std::shared_ptr<Beryll::BaseAnimatedObject>> m_animatedObjForShadowMap;

        // Shaders and light.
        std::shared_ptr<Beryll::Shader> m_simpleObjSunLightShadows;
        std::shared_ptr<Beryll::Shader> m_simpleObjSunLightShadowsNormals;
        std::shared_ptr<Beryll::Shader> m_animatedObjSunLight;
        std::unique_ptr<Beryll::ShadowMap> m_shadowMap;
        glm::mat4 m_sunLightVPMatrix{1.0f};
        // Assign in subclass constructor.
        glm::vec3 m_dirToSun{0.0f};
        glm::vec3 m_sunLightDir{0.0f};

        // Camera.
        float m_lastFingerMovePosX = 0.0f;
        float m_lastFingerMovePosY = 0.0f;
        float m_eyesLookAngleXZ = 0.0f; // Degrees.
        float m_eyesLookAngleY = -15.0f; // Degrees.
        glm::vec3 m_cameraOffset{0.0f};
        glm::vec3 m_cameraFront{0.0f};
        float m_cameraDistance = 60.0f;

        std::unique_ptr<Beryll::SkyBox> m_skyBox;

        // Map borders.
        float m_mapMinX = -800.0f;
        float m_mapMaxX = 800.0f;
        float m_mapMinZ = -800.0f;
        float m_mapMaxZ = 800.0f;

        // Pathfinding for enemies.
        std::shared_ptr<AStar> m_pathFinder; // Assign new object with map size in constructor of specific map.
        std::vector<glm::ivec2> m_pathAllowedPositionsXZ; // Points for enemy movements.
        glm::ivec2 m_playerClosestPathPoint{0};
        std::vector<glm::ivec2> m_pointsToSpawnEnemies;
        std::vector<glm::ivec2> m_pointsToSpawnEnemiesOnPlayerMoveDir;

        // Enemies waves.
        bool m_prepareWave1 = true;
        const float m_enemiesWave1Time = 2.0f; // Sec.
        bool m_prepareWave2 = true;
        const float m_enemiesWave2Time = 10.0f;
        bool m_prepareWave3 = true;
        const float m_enemiesWave3Time = 20.0f;
    };
}
