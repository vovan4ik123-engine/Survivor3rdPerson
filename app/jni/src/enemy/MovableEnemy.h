#pragma once

#include "BaseEnemy.h"
#include "pathfinding/AStar.h"

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
                     float HP,
                     std::shared_ptr<AStar> pathFinder);
        ~MovableEnemy() override;

        void update(const glm::vec3& playerOrigin) override;
        void findPath(glm::ivec2 destinationPoint);
        float pathUpdateTime = -99999.0f; // Track path update time for specific enemy.

    protected:

    private:
        void move() override;

        // Pathfinding.
        std::shared_ptr<AStar> m_pathFinder;
        std::vector<glm::ivec2> m_pathArray; // On XZ plane. INTEGER values.
        int m_pathArrayIndexToMove = 0;
        glm::ivec2 m_currentPointToMove2DIntegers{std::numeric_limits<int>::min()};
        glm::vec3 m_currentPointToMove3DFloats{0.0f};
        void removePointToMoveFromBlocked();

    };
}
