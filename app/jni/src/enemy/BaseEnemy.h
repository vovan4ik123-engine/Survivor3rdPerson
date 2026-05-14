#pragma once

#include "EngineHeaders.h"
#include "EnumsAndVariables.h"
#include "Sounds.h"

namespace Survivor3rdPerson
{
    enum class EnemyType
    {
        NONE,
        ENEMY_1,
        ENEMY_2
    };

    enum class EnemyState
    {
        MOVE, IN_ATTACK_RADIUS, STAND_AIMING, ATTACKING, CAN_ATTACK, DYING
    };

    class BaseEnemy
    {
    public:
        BaseEnemy() = delete;
        BaseEnemy(const char* filePath,  // Common params.
                  float collisionMassKg,    // Physics params.
                  bool wantCollisionCallBack,
                  Beryll::CollisionFlags collFlag,
                  Beryll::CollisionGroups collGroup,
                  Beryll::CollisionGroups collMask,
                  Beryll::SceneObjectGroups sceneGroup,
                  const float HP);
        virtual ~BaseEnemy();
        
        virtual void update(const glm::vec3& playerOrigin) = 0;
        virtual void findPath(glm::ivec2 destinationPoint) = 0;
        float pathUpdateTime = -99999.0f; // Track path update time for specific enemy.

        std::shared_ptr<Beryll::AnimatedCollidingCharacter> getObj() { return m_obj; }
        const int getObjID() const { return m_objID; }

        void enableEnemy();
        void disableEnemy();
        const bool getIsEnabled() const { return m_isEnabled; }
        static int getActiveCount() { return BaseEnemy::m_activeEnemiesCount; }
        bool getIsTimeToAttack() { return (m_lastAttackTime + timeBetweenAttacks) < EnumsAndVars::mapPlayTimeSec; }
        bool getIsDelayBeforeFirstAttack() { return (m_prepareToFirstAttackStartTime + timeBetweenAttacks) > EnumsAndVars::mapPlayTimeSec; }
        void takeDamage(const float damag) { m_currentHP -= damag; }
        void spawn(glm::ivec2 spawnPoint2D);
        void attack(const glm::vec3& playerOrigin);

        EnemyState unitState = EnemyState::MOVE;
        EnemyType unitType = EnemyType::NONE;

        // Sounds.
        SoundType attackSound = SoundType::NONE;
        SoundType dieSound = SoundType::NONE;

        // Spawn.
        bool isCanBeSpawned = false;
        float spawnTime = -99999.0f; // Track spawn time for specific enemy.
        static float lastSpawnOrRespawnTime; // Track spawn time in sec for all enemies.
        static float spawnOrRespawnDelay; // Time in sec.

        // Attack.
        float damage = 0.0f;
        float attackDistance = 20.0f;
        float timeBetweenAttacks = 1.0f; // Sec.

    protected:
        std::shared_ptr<Beryll::AnimatedCollidingCharacter> m_obj;
        int m_objID = 0;

        static int m_activeEnemiesCount;
        bool m_isEnabled = true;

        // Attack.
        float m_lastAttackTime = -99999.0f; // Sec.
        float m_prepareToFirstAttackStartTime = -99999.0f;
        bool m_prepareToFirstAttack = true; // When was outside attack radius and enter inside attack radius.

        // HP.
        float m_maxHP = 0.0f;
        float m_currentHP = 0.0f;
    };
}
