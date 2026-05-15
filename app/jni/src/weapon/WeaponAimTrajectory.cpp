#include "WeaponAimTrajectory.h"
#include "EnumsAndVariables.h"

namespace Survivor3rdPerson
{
    WeaponAimTrajectory::WeaponAimTrajectory()
    {
        m_trajectoryPoint = std::make_shared<Beryll::SimpleObject>("models3D/weapon/AimTrajectoryPoint.fbx", EnumsAndVars::SceneGR_NONE);
        m_trajectoryHitPoint = std::make_shared<Beryll::SimpleObject>("models3D/weapon/AimTrajectoryHitPoint.fbx", EnumsAndVars::SceneGR_NONE);

        m_shader = Beryll::Renderer::createShader("shaders/GLES/PlayerAirTrajectory.vert",
                                                  "shaders/GLES/PlayerAirTrajectory.frag");
        m_shader->bind();
        m_shader->activateDiffuseTextureMat1();
    }

    WeaponAimTrajectory::~WeaponAimTrajectory()
    {

    }

    void WeaponAimTrajectory::calculateAndDraw(const float bulletMass,
                                               const glm::vec3& bulletGravity,
                                               const glm::vec3& startPosition,
                                               const float throwAngleRadians,
                                               const glm::vec3& impulseVector, // glm::length(impulseVector) = throw power.
                                                     const glm::vec3& linearFactor,
                                               const glm::vec3& sunLightDir)
    {
        float inverseMass = 0.0f;
        if(bulletMass > 0.0f)
            inverseMass = 1.0f / bulletMass;

        glm::vec3 linearVelocity = impulseVector * linearFactor * inverseMass;
        float speed = glm::length(linearVelocity);

        glm::vec3 throwDirXZPlane{0.0f};
        glm::vec3 startPositionXZPlane = startPosition;
        startPositionXZPlane.y = 0.0f;
        glm::vec3 normalizedImpulseVector = glm::normalize(impulseVector);

        float XZDistance = 0.0f;
        const float startHeight = startPosition.y;
        float calculatedY = 0.0f;
        glm::vec3 previousPoint{0.0f};
        glm::vec3 currentPoint{0.0f};
        bool castRayBetweenPoints = false;

        // Calculate some points on fly trajectory.
        for(int i = 2; i <= 20; ++i)
        {
            throwDirXZPlane = normalizedImpulseVector * float(i * i) * 1.7f; // Point every float(i * i)... meters on trajectory !!! Not on ground !!!
            if(i == 3)
                throwDirXZPlane *= 1.05f;
            else if(i == 4)
                throwDirXZPlane *= 1.02f;

            throwDirXZPlane.y = 0.0f; // Do point projection on ground.
            XZDistance = glm::length(throwDirXZPlane);
            // https://www.omnicalculator.com/physics/trajectory-projectile-motion
            calculatedY = startHeight +
                          XZDistance * glm::tan(throwAngleRadians) + bulletGravity.y * glm::pow(XZDistance, 2) /
                                                                     (2 * glm::pow(speed, 2) * glm::pow(glm::cos(throwAngleRadians), 2));

            currentPoint = startPositionXZPlane + throwDirXZPlane;
            currentPoint.y = calculatedY;

            m_shader->bind();
            m_shader->set3Float("sunLightDir", sunLightDir);
            m_shader->set3Float("cameraPos", Beryll::Camera::getCameraPos());
            m_shader->set1Float("ambientLight", 0.8f);
            m_shader->set1Float("specularLightStrength", 2.0f);

            if(castRayBetweenPoints)
            {
                Beryll::RayClosestHit hit = Beryll::Physics::castRayClosestHit(previousPoint, currentPoint,
                                                                               EnumsAndVars::CollGr_WEAPON_BULLET,
                                                                               EnumsAndVars::CollGr_STATIC_ENV | EnumsAndVars::CollGr_ENEMY);

                if(hit)
                {
                    m_trajectoryHitPoint->setOrigin(hit.hitPoint);

                    float scaleFactor = 1.0f + glm::distance(startPosition, hit.hitPoint) / 350.0f; // Scale to + 100% size every 350m.
                    m_modelMatrix = m_trajectoryHitPoint->getModelMatrix() * glm::scale(glm::mat4{1.0f}, glm::vec3{scaleFactor});
                    m_shader->setMatrix4x4Float("modelMatrix", m_modelMatrix);
                    m_shader->setMatrix3x3Float("normalMatrix", glm::mat3(m_modelMatrix));
                    Beryll::Renderer::drawObject(m_trajectoryHitPoint, m_modelMatrix, m_shader);

                    return;
                }
            }

            // Draw.
            m_trajectoryPoint->setOrigin(currentPoint);

            m_modelMatrix = m_trajectoryPoint->getModelMatrix();
            m_shader->setMatrix4x4Float("modelMatrix", m_modelMatrix);
            m_shader->setMatrix3x3Float("normalMatrix", glm::mat3(m_modelMatrix));
            Beryll::Renderer::drawObject(m_trajectoryPoint, m_modelMatrix, m_shader);

            previousPoint = currentPoint;
            castRayBetweenPoints = true;
        }
    }
}
