#pragma once

#include "CppHeaders.h"
#include "EngineHeaders.h"

namespace Survivor3rdPerson
{
    // https://web.archive.org/web/20171022224528/http://www.policyalmanac.org:80/games/aStarTutorial.htm
    // https://github.com/daancode/a-star/tree/master
    // https://en.wikipedia.org/wiki/A*_search_algorithm
    struct Node
    {
        Node(glm::ivec2 coords, Node* par) : coordinates(coords), parent(par) {}

        int G = 0; // Score from start to this node.
        int H = 0; // Estimated score from this node to end.
        int F = 0; // G + H.

        bool nodeInOpenSet = true;
        int nodesCountInChain = 1;

        glm::ivec2 coordinates{0, 0};
        Node* parent = nullptr;
    };

    class AStar final
    {
    public:
        AStar();
        AStar(int mapLeftX, int mapRightX, int mapBottomY, int mapUpperY, int nodeDistance);
        ~AStar();

        void addWallPosition(glm::ivec2 wall);
        void addBlockedPosition(glm::ivec2 pos);
        void clearBlockedPositions() { m_blockedPositions.clear(); }
        std::vector<glm::ivec2> findPath(const glm::ivec2 start, const glm::ivec2 end, const int pathNodeMaxCount = -1); // -1 = return all nodes from start to end.

    private:
        static constexpr int m_costMovePerpendicular = 10;
        static constexpr int m_costMoveDiagonal = 14;

        int m_nDist = 0; // Distance between current node and left/right/bottom/up neighbor node.
        std::vector<glm::ivec2> m_directions;
        int m_directionsSize = 0;

        int m_mapLeftX = 0;
        int m_mapRightX = 0;
        int m_mapBottomY = 0;
        int m_mapUpperY = 0;

        std::vector<glm::ivec2> m_walls;
        std::vector<glm::ivec2> m_blockedPositions;
        std::vector<Node> m_allNodesCreated; // Create objects on stack.

        int estimateRoad(glm::ivec2 start, glm::ivec2 end)
        {
            // Manhattan method to estimate road cost from start to end.
            return (std::abs(start.x - end.x) + std::abs(start.y - end.y)) * m_costMovePerpendicular;
        }

        Node* findNode(std::vector<Node>& nodes, glm::ivec2 coords);
        bool isCollisionWithWallOrBlocked(glm::ivec2 coords);
    };
}
