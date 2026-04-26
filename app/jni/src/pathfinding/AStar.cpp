#include "AStar.h"

namespace Survivor3rdPerson
{
    AStar::AStar()
    {

    }

    AStar::AStar(int mapLeftX, int mapRightX, int mapBottomY, int mapUpperY, int nodeDistance)
    {
        m_mapLeftX = mapLeftX;
        m_mapRightX = mapRightX;
        m_mapBottomY = mapBottomY;
        m_mapUpperY = mapUpperY;

        m_walls.reserve(1000);
        m_blockedPositions.reserve(1000);
        m_allNodesCreated.reserve(400);

        m_nDist = nodeDistance;

        m_directions = {{0, m_nDist}, {m_nDist, 0}, {0, -m_nDist}, {-m_nDist, 0},
                        {-m_nDist, -m_nDist}, {m_nDist, m_nDist}, {-m_nDist, m_nDist}, {m_nDist, -m_nDist}};
        m_directionsSize = m_directions.size();
    }

    AStar::~AStar()
    {

    }

    void AStar::addWallPosition(glm::ivec2 wall)
    {
        if(std::find(m_walls.begin(), m_walls.end(), wall) == m_walls.end())
        {
            m_walls.push_back(wall);
        }
    }

    void AStar::addBlockedPosition(glm::ivec2 pos)
    {
        if(std::find(m_blockedPositions.begin(), m_blockedPositions.end(), pos) == m_blockedPositions.end())
        {
            m_blockedPositions.push_back(pos);
        }
    }

    bool AStar::isCollisionWithWallOrBlocked(glm::ivec2 coords)
    {
        if(std::find(m_blockedPositions.begin(), m_blockedPositions.end(), coords) != m_blockedPositions.end() ||
           coords.x < m_mapLeftX || coords.x > m_mapRightX ||
           coords.y < m_mapBottomY || coords.y > m_mapUpperY ||
           std::find(m_walls.begin(), m_walls.end(), coords) != m_walls.end())
        {
            return true;
        }

        return false;
    }

    std::vector<glm::ivec2> AStar::findPath(const glm::ivec2 start, const glm::ivec2 end, const int pathNodeMaxCount)
    {
        if(isCollisionWithWallOrBlocked(start))
            return {start};

        m_allNodesCreated.emplace_back(start, nullptr);
        Node* current = nullptr;
        int openSetCount = 1;

        while(openSetCount > 0)
        {
            current = nullptr;
            for(Node& node : m_allNodesCreated)
            {
                if(current == nullptr && node.nodeInOpenSet)
                {
                    current = &node;
                }
                else if(node.nodeInOpenSet && node.F <= current->F)
                {
                    current = &node;
                }
            }

            if(current == nullptr || current->coordinates == end)
                break;

            current->nodeInOpenSet = false;
            --openSetCount;

            for(int i = 0; i < m_directionsSize; ++i)
            {
                glm::ivec2 newCoordinates(current->coordinates + m_directions[i]);

                if(isCollisionWithWallOrBlocked(newCoordinates))
                    continue;

                int totalCost = current->G + ((i < 4) ? m_costMovePerpendicular : m_costMoveDiagonal);

                Node* successor = findNode(m_allNodesCreated, newCoordinates);

                if(successor && !successor->nodeInOpenSet)
                {
                    continue;
                }
                else if(successor == nullptr)
                {
                    m_allNodesCreated.emplace_back(newCoordinates, current);
                    ++openSetCount;

                    successor = &m_allNodesCreated[m_allNodesCreated.size() - 1];
                    successor->G = totalCost;
                    successor->H = estimateRoad(successor->coordinates, end);
                    successor->F = successor->G + successor->H;
                    successor->nodesCountInChain = current->nodesCountInChain + 1;
                }
                else if(totalCost < successor->G)
                {
                    successor->parent = current;
                    successor->G = totalCost;
                    // successor->H should be same.
                    successor->F = successor->G + successor->H;
                    successor->nodesCountInChain = current->nodesCountInChain + 1;
                }

                if(pathNodeMaxCount != -1 && successor->nodesCountInChain >= pathNodeMaxCount)
                {
                    // Chain already long enough. Stop search.
                    current = successor;
                    openSetCount = 0;
                    break;
                }
            }
        }

        std::vector<glm::ivec2> path;
        while(current != nullptr)
        {
            path.push_back(current->coordinates);
            current = current->parent;
        }

        m_allNodesCreated.clear();

        if(path.empty())
            return {start};

        std::reverse(path.begin(), path.end());
        return path;
    }

    Node* AStar::findNode(std::vector<Node>& nodes, glm::ivec2 coords)
    {
        for(Node& node : nodes)
        {
            if(node.coordinates == coords)
                return &node;
        }

        return nullptr;
    }
}
