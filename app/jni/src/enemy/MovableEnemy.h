#pragma once

#include "BaseEnemy.h"

namespace Survivor3rdPerson
{
    class MovableEnemy : public BaseEnemy
    {
    public:
        MovableEnemy() = delete;
        MovableEnemy(const char* filePath,  // Common params.
                      float collisionMassKg,    // Physics params.
                      bool wantCollisionCallBack,
                     Beryll::CollisionFlags collFlag,
                     Beryll::CollisionGroups collGroup,
                     Beryll::CollisionGroups collMask,
                     Beryll::SceneObjectGroups sceneGroup,
                     float HP);
        ~MovableEnemy() override;

        void update(const glm::vec3& playerOrigin) override;
        void setPathArray(std::vector<glm::ivec2> pathArray, const int indexToMove) override;

    protected:
        void move() override;
        float m_timeDie = 0.0f;
    };
}
