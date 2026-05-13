#pragma once

#include "EngineHeaders.h"
#include "EnumsAndVariables.h"
#include "Sounds.h"

namespace Survivor3rdPerson
{
    enum class UnitState
    {
        MOVE, IN_ATTACK_RADIUS, STAND_AIMING, ATTACKING, CAN_ATTACK, DYING
    };

    enum class UnitType
    {
        NONE,
        ENEMY_1,
        ENEMY_2
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
        
        virtual void update(const glm::vec3& playerOrigin) = 0; // Can be different for different subclasses.

        std::shared_ptr<Beryll::AnimatedCollidingCharacter> getObj() { return m_obj; }
        int getObjID() { return m_objID; }

        void enableEnemy();
        void disableEnemy();
        bool getIsEnabled() { return m_isEnabled; }
        static int getActiveCount() { return BaseEnemy::m_activeEnemiesCount; }
        bool getIsTimeToAttack() { return (m_lastAttackTime + timeBetweenAttacks) < EnumsAndVars::mapPlayTimeSec; }
        bool getIsDelayBeforeFirstAttack() { return (m_prepareToFirstAttackStartTime + timeBetweenAttacks) > EnumsAndVars::mapPlayTimeSec; }
        void takeDamage(const float damag) { m_currentHP -= damag; }
        void spawn(glm::ivec2 spawnPoint2D);
        void attack(const glm::vec3& playerOrigin);

        static float lastSpawnOrRespawnTime; // Track spawn time in sec for all enemies.
        static float spawnOrRespawnDelay; // Time in sec.
        UnitState unitState = UnitState::MOVE;
        UnitType unitType = UnitType::NONE;
        SoundType attackSound = SoundType::NONE;
        SoundType dieSound = SoundType::NONE;
        float spawnTime = -99999.0f; // Track spawn time for specific enemy.

        bool isCanBeSpawned = false;

        float damage = 0.0f;
        float attackDistance = 10.0f;
        float damageRadius = 0.0f; // Use if unit AttackType::RANGE_DAMAGE_RADIUS.
        float timeBetweenAttacks = 0.0f; // Sec.

        int experienceWhenDie = 0;

    protected:
        std::shared_ptr<Beryll::AnimatedCollidingCharacter> m_obj;
        int m_objID = 0;

        static int m_activeEnemiesCount;
        bool m_isEnabled = true;

        // Attack data.
        float m_lastAttackTime = -9999.0f; // Sec.
        float m_prepareToFirstAttackStartTime = -9999.0f;
        bool m_prepareToFirstAttack = true; // When was outside attack radius and enter inside attack radius.

        // HP.
        float m_maxHP = 0.0f;
        float m_currentHP = 0.0f;
    };
}
