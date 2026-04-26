#include "BaseEnemy.h"

namespace Survivor3rdPerson
{
    int BaseEnemy::m_activeEnemiesCount = 0;

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
        m_obj->enableDraw();
        m_obj->enableUpdate();
        m_obj->enableCollisionMesh();

        m_currentHP = m_maxHP;

        ++BaseEnemy::m_activeEnemiesCount;
        m_isEnabled = true;
        unitState = UnitState::MOVE;
    }

    void BaseEnemy::disableEnemy()
    {
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

    void BaseEnemy::attack(const glm::vec3& playerOrigin)
    {
        //BR_INFO("%s", "BaseEnemy::attack()");
        m_obj->rotateToPoint(playerOrigin, true);
        m_obj->setCurrentAnimationByIndex(EnumsAndVars::AnimationIndexes::attack, true, true);
        m_lastAttackTime = EnumsAndVars::mapPlayTimeSec;
        unitState = UnitState::ATTACKING;

        Sounds::playSoundEffect(attackSound);
        Sounds::playSoundEffect(attackHitSound);
    }
}
