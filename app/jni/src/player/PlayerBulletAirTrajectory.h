#pragma once

#include "EngineHeaders.h"

namespace Survivor3rdPerson
{
    class PlayerBulletAirTrajectory
    {
    public:
        PlayerBulletAirTrajectory();
        ~PlayerBulletAirTrajectory();

        glm::mat4 getModelMatrix() { return m_trajectoryPoint->getModelMatrix(); }

        void calculateAndDraw(const float bulletMass,
                              const glm::vec3& bulletGravity,
                              const glm::vec3& startPosition,
                              const float throwAngleRadians,
                              const glm::vec3& impulseVector,
                              const glm::vec3& linearFactor,
                              const glm::vec3& sunLightDir);

    private:
        // One m_trajectoryPoint should be used for all m_calculatedPositions
        // Set new origin for next position and draw it again.
        std::shared_ptr<Beryll::SimpleObject> m_trajectoryPoint;
        std::shared_ptr<Beryll::SimpleObject> m_trajectoryHitPoint;

        glm::mat4 m_modelMatrix{1.0f};
        std::shared_ptr<Beryll::Shader> m_shader;
    };
}
