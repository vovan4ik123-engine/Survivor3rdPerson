#include "MovableEnemy.h"

namespace Survivor3rdPerson
{
    MovableEnemy::MovableEnemy(const char* filePath,
                               float collisionMassKg,
                               bool wantCollisionCallBack,
                               Beryll::CollisionFlags collFlag,
                               Beryll::CollisionGroups collGroup,
                               Beryll::CollisionGroups collMask,
                               Beryll::SceneObjectGroups sceneGroup,
                               float HP,
                               std::shared_ptr<AStar> pathFinder)
                                 : BaseEnemy(filePath,
                                             collisionMassKg,
                                             wantCollisionCallBack,
                                             collFlag,
                                             collGroup,
                                             collMask,
                                             sceneGroup,
                                             HP)
    {
        unitState = UnitState::MOVE;
        m_pathFinder = std::move(pathFinder);
    }

    MovableEnemy::~MovableEnemy()
    {

    }

    void MovableEnemy::update(const glm::vec3& playerOrigin)
    {
        if(unitState == UnitState::DYING)
        {
            if(m_obj->getIsOneTimeAnimationFinished())
                disableEnemy();

            return;
        }
        else if(m_currentHP <= 0.0f)
        {
            m_obj->setCurrentAnimationByIndex(3 + Beryll::RandomGenerator::getInt(2), true, false, false);
            unitState = UnitState::DYING;
            m_obj->disableCollisionMesh();
            removePointToMoveFromBlocked();
            return;
        }

        if(unitState == UnitState::ATTACKING)
        {
            //BR_INFO("%s", "MovableEnemy is attacking");
            if(m_obj->getIsOneTimeAnimationFinished())
            {
                m_obj->setCurrentAnimationByIndex(EnumsAndVars::AnimationIndexes::stand, false, false);
                unitState = UnitState::STAND_AIMING;
            }
        }
        else if(getIsDelayBeforeFirstAttack())
        {
            //BR_INFO("%s", "MovableEnemy DelayBeforeFirstAttack");
            unitState = UnitState::STAND_AIMING;
            m_obj->setCurrentAnimationByIndex(EnumsAndVars::AnimationIndexes::stand, false, false);
            m_obj->rotateToPoint(playerOrigin, true);
        }
        else if(glm::distance(m_obj->getOrigin(), playerOrigin) > attackDistance)
        {
            //BR_INFO("%s", "MovableEnemy move because distance");
            move();
        }
        else
        {
            //BR_INFO("%s", "MovableEnemy IN_ATTACK_RADIUS");
            unitState = UnitState::IN_ATTACK_RADIUS;

            if(getIsTimeToAttack())
            {
                //BR_INFO("%s", "MovableEnemy if(getIsTimeToAttack())");
                // Check if enemy see player.
                Beryll::RayClosestHit rayEnv = Beryll::Physics::castRayClosestHit(m_obj->getOrigin(),
                                                                                  playerOrigin,
                                                                                  EnumsAndVars::CollGr_RAY_FOR_ENV,
                                                                                  EnumsAndVars::CollGr_STATIC_ENV);

                if(rayEnv)
                {
                    //BR_INFO("%s", "MovableEnemy move because dont see");
                    move();
                }
                else
                {
                    if(m_prepareToFirstAttack)
                    {
                        //BR_INFO("%s", "MovableEnemy prepareToFirstAttack");
                        m_prepareToFirstAttack = false;
                        m_prepareToFirstAttackStartTime = EnumsAndVars::mapPlayTimeSec;
                    }
                    else
                    {
                        //BR_INFO("%s", "MovableEnemy CAN_ATTACK");
                        unitState = UnitState::CAN_ATTACK;
                    }
                }
            }
        }
    }

    void MovableEnemy::move()
    {
        unitState = UnitState::MOVE;

        m_prepareToFirstAttack = true;

        m_obj->setCurrentAnimationByIndex(EnumsAndVars::AnimationIndexes::run, false, false, true);

        m_obj->getController().moveToPosition(m_currentPointToMove3DFloats, true, true);

        if(m_obj->getController().getIsMoving()) // Already moving.
            return;

        // Not moving. Current point to move reached. Delete it from path finder.
        removePointToMoveFromBlocked();

        if(m_pathArrayIndexToMove + 1 >= m_pathArray.size()) // No more points to move.
            return;

        // Find new point to move.
        ++m_pathArrayIndexToMove;
        m_currentPointToMove2DIntegers = m_pathArray[m_pathArrayIndexToMove];
        m_pathFinder->addBlockedPosition(m_currentPointToMove2DIntegers);

        float currentY = m_currentPointToMove3DFloats.y;
        m_currentPointToMove3DFloats = glm::vec3(m_currentPointToMove2DIntegers.x,
                                                 m_obj->getFromOriginToBottom(),
                                                 m_currentPointToMove2DIntegers.y);

        glm::vec3 rayFrom = m_currentPointToMove3DFloats;
        rayFrom.y += 200.0f;
        glm::vec3 rayTo = m_currentPointToMove3DFloats;
        rayTo.y -= 200.0f;
        Beryll::RayClosestHit rayHit = Beryll::Physics::castRayClosestHit(rayFrom,
                                                                          rayTo,
                                                                          EnumsAndVars::CollGr_RAY_FOR_ENV,
                                                                          EnumsAndVars::CollGr_STATIC_ENV);

        if(rayHit)
            m_currentPointToMove3DFloats.y = rayHit.hitPoint.y + m_obj->getFromOriginToBottom();
    }

    void MovableEnemy::findPath(glm::ivec2 destinationPoint)
    {
        // Remove old point to move.
        removePointToMoveFromBlocked();

        // Closest XZ point to enemy on grid (regardless of player). Sometimes can be behind enemy. It just any closest point.
        glm::ivec2 closestXZ{std::roundf(m_obj->getOrigin().x / EnumsAndVars::pathFinderStep) * EnumsAndVars::pathFinderStep,
                             std::roundf(m_obj->getOrigin().z / EnumsAndVars::pathFinderStep) * EnumsAndVars::pathFinderStep};

        m_pathArray = m_pathFinder->findPath(closestXZ, destinationPoint, 6);
        pathUpdateTime = EnumsAndVars::mapPlayTimeSec;

        if(m_pathArray.empty())
        {
            BR_ASSERT(false, "%s", "m_pathArray.empty()");
        }

        if(m_pathArray.size() > 1)
            m_pathArray.erase(m_pathArray.begin()); // Because sometimes closestXZ is behind enemy and enemy rotates and start move back.

        // Assign new.
        m_pathArrayIndexToMove = 0;
        m_currentPointToMove2DIntegers = m_pathArray[0];
        m_pathFinder->addBlockedPosition(m_currentPointToMove2DIntegers);
        m_currentPointToMove3DFloats = glm::vec3(m_currentPointToMove2DIntegers.x, 0.0f, m_currentPointToMove2DIntegers.y);

        glm::vec3 rayFrom = m_currentPointToMove3DFloats;
        rayFrom.y = 400.0f;
        glm::vec3 rayTo = m_currentPointToMove3DFloats;
        rayTo.y = -400.0f;
        Beryll::RayClosestHit rayHit = Beryll::Physics::castRayClosestHit(rayFrom, rayTo,
                                                                          EnumsAndVars::CollGr_RAY_FOR_ENV,
                                                                          EnumsAndVars::CollGr_STATIC_ENV);
        if(rayHit)
            m_currentPointToMove3DFloats.y = rayHit.hitPoint.y + m_obj->getFromOriginToBottom();
        else
            m_currentPointToMove3DFloats.y = m_obj->getFromOriginToBottom();
    }

    void MovableEnemy::removePointToMoveFromBlocked()
    {
        if(m_currentPointToMove2DIntegers.x != std::numeric_limits<int>::min())
        {
            m_pathFinder->removeBlockedPosition(m_currentPointToMove2DIntegers);
            m_currentPointToMove2DIntegers = glm::ivec2(std::numeric_limits<int>::min());
        }
    }
}
