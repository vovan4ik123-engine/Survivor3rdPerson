#pragma once

#include "EngineHeaders.h"

namespace Survivor3rdPerson
{
    class Player : public Beryll::SimpleCollidingCharacter
    {
    public:
        Player(const char* filePath,  // Common params.
               bool wantCollisionCallBack,
               Beryll::CollisionFlags collFlag,
               Beryll::CollisionGroups collGroup,
               Beryll::CollisionGroups collMask,
               Beryll::SceneObjectGroups sceneGroup);
        ~Player() override;

        void update();
        float getCurrentHP() { return m_currentHP; }
        float getMaxHP() { return m_maxHP; }
        void resurrect() { m_currentHP = m_maxHP; }
        void takeDamage(const float damage) { m_currentHP -= damage; }

    private:
        float m_maxHP = 0.0f;
        float m_currentHP = 0.0f;
    };
}
