#include "BaseEnemy.h"

namespace Survivor3rdPerson
{
    int BaseEnemy::m_activeEnemiesCount = 0;
    float BaseEnemy::lastSpawnOrRespawnTime = -99999.0f;
    float BaseEnemy::spawnOrRespawnDelay = 1.0f;

    BaseEnemy::BaseEnemy(const char* filePath,
                         float collisionMassKg,
                         bool wantCollisionCallBack,
                         Beryll::CollisionFlags collFlag,
                         Beryll::CollisionGroups collGroup,
                         Beryll::CollisionGroups collMask,
                         Beryll::SceneObjectGroups sceneGroup,
                         const float HP)
    {
        m_obj = std::make_shared<Beryll::AnimatedCollidingCharacter>(filePath,
                                                                     collisionMassKg,
                                                                     wantCollisionCallBack,
                                                                     collFlag,
                                                                     collGroup,
                                                                     collMask,
                                                                     sceneGroup);

        m_objID = m_obj->getID();
        m_maxHP = HP;
        m_currentHP = HP;

        disableEnemy();
    }

    BaseEnemy::~BaseEnemy()
    {
        //BR_INFO("%s", "BaseEnemy::~BaseEnemy()");
        disableEnemy();
    }

    void BaseEnemy::enableEnemy()
    {
        if(m_isEnabled)
            return;

        m_obj->enableDraw();
        m_obj->enableUpdate();
        m_obj->enableCollisionMesh();

        m_currentHP = m_maxHP;

        ++BaseEnemy::m_activeEnemiesCount;
        m_isEnabled = true;
        unitState = UnitState::MOVE;
        m_prepareToFirstAttack = true;
        m_obj->setCurrentAnimationByIndex(EnumsAndVars::AnimationIndexes::run, false, true, true);
    }

    void BaseEnemy::disableEnemy()
    {
        if(!m_isEnabled)
            return;

        m_obj->disableDraw();
        m_obj->disableUpdate();
        m_obj->disableCollisionMesh();

        if(BaseEnemy::m_activeEnemiesCount > 0)
            --BaseEnemy::m_activeEnemiesCount;

        m_isEnabled = false;
        m_lastAttackTime = -9999.0f;
        m_prepareToFirstAttackStartTime = -9999.0f;
        m_prepareToFirstAttack = true;
    }

    void BaseEnemy::spawn(glm::ivec2 spawnPoint2D)
    {
        if(!getIsEnabled())
            enableEnemy();

        glm::vec3 spawnPoint3D{spawnPoint2D.x, 0.0f, spawnPoint2D.y};
        glm::vec3 rayFrom{spawnPoint3D.x, 400.0f, spawnPoint3D.z};
        glm::vec3 rayTo{spawnPoint3D.x, -400.0f, spawnPoint3D.z};
        Beryll::RayClosestHit rayHit = Beryll::Physics::castRayClosestHit(rayFrom, rayTo,
                                                                          Beryll::CollisionGroups::RAY_FOR_ENVIRONMENT,
                                                                          Beryll::CollisionGroups::STATIC_ENVIRONMENT);
        if(rayHit)
            spawnPoint3D.y = rayHit.hitPoint.y + m_obj->getFromOriginToBottom();
        else
            spawnPoint3D.y = m_obj->getFromOriginToBottom();

        m_obj->setOrigin(spawnPoint3D);
        spawnTime = EnumsAndVars::mapPlayTimeSec;
    }

    void BaseEnemy::attack(const glm::vec3& playerOrigin)
    {
        //BR_INFO("%s", "BaseEnemy::attack()");
        m_obj->rotateToPoint(playerOrigin, true);
        m_obj->setCurrentAnimationByIndex(EnumsAndVars::AnimationIndexes::attack, true, true);
        m_lastAttackTime = EnumsAndVars::mapPlayTimeSec;
        unitState = UnitState::ATTACKING;
    }
}
