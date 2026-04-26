#pragma once

#include "EngineHeaders.h"

namespace Survivor3rdPerson
{
    class Player : public Beryll::SimpleCollidingCharacter
    {
    public:
        Player(const char* filePath,  // Common params.
               float collisionMassKg,    // Physics params.
               bool wantCollisionCallBack,
               Beryll::CollisionFlags collFlag,
               Beryll::CollisionGroups collGroup,
               Beryll::CollisionGroups collMask,
               Beryll::SceneObjectGroups sceneGroup,
               const float HP);
        ~Player() override;

        void update();
        float getCurrentHP() { return m_currentHP; }
        float getMaxHP() { return m_maxHP; }
        void resurrect() { m_currentHP = m_maxHP; }
        void takeDamage(const float damag) { m_currentHP -= damag; }

    private:
        // HP.
        float m_maxHP = 0.0f;
        float m_currentHP = 0.0f;
    };
}
