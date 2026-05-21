#include "Bazooka.h"

namespace Survivor3rdPerson
{
    Bazooka::Bazooka(const float reloadTime, const float damageRadius) : BaseWeapon(reloadTime, WeaponType::BAZOOKA)
    {
        m_damageRadius = damageRadius;
        m_damageRadiusSquared = damageRadius * damageRadius;

        m_rockets.reserve(10);

        for(int i = 0; i < m_rockets.capacity(); ++i)
        {
            auto obj = std::make_shared<Beryll::SimpleCollidingObject>("models3D/weapon/BazookaRocket.fbx",
                                                                       m_rocketMass,
                                                                       true,
                                                                       Beryll::CollisionFlags::DYNAMIC,
                                                                       EnumsAndVars::CollGr_WEAPON_BULLET,
                                                                       EnumsAndVars::CollGr_STATIC_ENV | EnumsAndVars::CollGr_ENEMY,
                                                                       EnumsAndVars::SceneGR_NONE);

            obj->disableUpdate();
            obj->disableCollisionMesh();
            obj->disableDraw();

            m_rockets.push_back(obj);
        }
    }

    Bazooka::~Bazooka()
    {
        for(const auto& rocket : m_rockets)
        {
            rocket->disableUpdate();
            rocket->disableCollisionMesh();
            rocket->disableDraw();
        }
    }

    void Bazooka::update(const glm::vec3& playerOrig, const glm::vec3& playerFaceDirXZ,
                         const std::vector<std::shared_ptr<BaseEnemy>>& enemies)
    {
        for(const auto& rocket : m_rockets)
        {
            if(rocket->getIsEnabledUpdate())
            {
                rocket->updateAfterPhysics();

                if(glm::length(rocket->getOrigin()) > 2500.0f) // Distance from origin (0.0.0).
                {
                    rocket->disableUpdate();
                    rocket->disableCollisionMesh();
                    rocket->disableDraw();
                }
            }
        }

        float angleBetweenWorldUpAndCameraBack = BeryllUtils::Common::getAngleInRadians(BeryllConstants::worldUp, Beryll::Camera::getCameraBackDirectionXYZ());
        m_shotAngleRadians = angleBetweenWorldUpAndCameraBack - glm::half_pi<float>() + 0.3f; // + 0...f direct trajectory more up.

        m_shotImpulseVector = playerFaceDirXZ;
        m_shotImpulseVector.y = glm::tan(m_shotAngleRadians);
        m_shotImpulseVector = glm::normalize(m_shotImpulseVector);
        m_shotImpulseVector *= m_rocketMass;
        m_shotImpulseVector *= 400.0f;

        m_shotStartPosition = playerOrig + playerFaceDirXZ * 4.0f;
        m_shotStartPosition.y += 4.0f;

        // Do damage.
        for(const auto& rocket : m_rockets)
        {
            if(rocket->getIsEnabledUpdate())
            {
                int collisionID = Beryll::Physics::getAnyCollisionForID(rocket->getID());
                if(collisionID > 0)
                {
                    Beryll::ParticleSystem::EmitCubesFromCenter(6, 0.4f, m_damageRadius * 0.12f, m_damageRadius * 0.25f,
                                                                glm::vec4{0.957f, 0.957f, 0.082f, 1.0f}, glm::vec4{0.867f, 0.164f, 0.07f, 0.2f},
                                                                rocket->getOrigin(),
                                                                glm::vec3{Beryll::RandomGenerator::getFloat() * 8.0f - 4.0f,
                                                                          Beryll::RandomGenerator::getFloat() * 8.0f - 4.0f,
                                                                          Beryll::RandomGenerator::getFloat() * 8.0f - 4.0f},
                                                                20.0f);

                    rocket->disableUpdate();
                    rocket->disableCollisionMesh();
                    rocket->disableDraw();

                    for(const auto& enemy : enemies)
                    {
                        if(enemy->getIsEnabled() && enemy->unitState != EnemyState::DYING &&
                           glm::distance2(enemy->getObj()->getOrigin(), rocket->getOrigin()) < m_damageRadiusSquared)
                        {
                            // Rocket damage all enemies in radius but do damage only to enemies in front of player.
                            glm::vec3 enemyDirXZ = enemy->getObj()->getOrigin() - playerOrig;
                            enemyDirXZ.y = 0.0f;
                            if(BeryllUtils::Common::getAngleInRadians(playerFaceDirXZ, glm::normalize(enemyDirXZ)) < 1.4f)
                            {
                                int number = Beryll::RandomGenerator::getInt(1000) + 1;
                                float numberHeight = std::max(2.5f, glm::distance(Beryll::Camera::getCameraPos(), enemy->getObj()->getOrigin()) * 0.03f);
                                if(Beryll::RandomGenerator::getFloat() < 0.1f)
                                {
                                    number *= 10;
                                    numberHeight *= 3.0f;
                                }

                                enemy->takeDamage(number);
                                Beryll::TextOnScene::addNumbersToShow(number, numberHeight, 0.5f, enemy->getObj()->getOrigin() + glm::vec3{0.0f, 10.0f, 0.0f},
                                                                      glm::vec3{Beryll::RandomGenerator::getFloat() * 10.0f - 5.0f,
                                                                                Beryll::RandomGenerator::getFloat() * 3.0f + 3.0f,
                                                                                Beryll::RandomGenerator::getFloat() * 10.0f - 5.0f},
                                                                      60.0f);
                            }
                        }
                    }
                }
            }
        }
    }

    // Draw method can change shader because it can draw aim trajectory and it has own shader.
    void Bazooka::draw(const glm::mat4& sunLightVPMatrix, const glm::vec3& sunLightDir, const std::shared_ptr<Beryll::Shader>& shader)
    {
        glm::mat4 modelMatrix{1.0f};

        for(const auto& rocket : m_rockets)
        {
            if(rocket->getIsEnabledDraw())
            {
                modelMatrix = rocket->getModelMatrix();
                shader->setMatrix4x4Float("MVPLightMatrix", sunLightVPMatrix * modelMatrix);
                shader->setMatrix4x4Float("modelMatrix", modelMatrix);
                shader->setMatrix3x3Float("normalMatrix", glm::mat3(modelMatrix));
                Beryll::Renderer::drawObject(rocket, modelMatrix, shader);
            }
        }

        m_aimTrajectory.calculateAndDraw(m_rocketMass,
                                         glm::vec3{0.0f, -10.0f, 0.0f},
                                         m_shotStartPosition,
                                         m_shotAngleRadians,
                                         m_shotImpulseVector,
                                         glm::vec3{1.0f},
                                         sunLightDir);
    }

    void Bazooka::shoot()
    {
        if(m_lastShotTime + m_reloadTime < EnumsAndVars::mapPlayTimeSec)
        {
            if(!m_rockets[m_currentRocketIndex]->getIsEnabledUpdate())
            {
                m_rockets[m_currentRocketIndex]->enableDraw();
                m_rockets[m_currentRocketIndex]->enableUpdate();
                m_rockets[m_currentRocketIndex]->enableCollisionMesh();
            }

            //const glm::vec3 shotDirNorm = glm::normalize(m_shotImpulseVector);
            //const glm::vec3 rightVec = glm::normalize(glm::cross(shotDirNorm, BeryllConstants::worldUp));
            //const glm::vec3 upVec = glm::normalize(glm::cross(rightVec, shotDirNorm));
            m_rockets[m_currentRocketIndex]->addToRotation(glm::rotation(m_rockets[m_currentRocketIndex]->getFaceDirXYZ(), glm::normalize(m_shotImpulseVector)));

            m_rockets[m_currentRocketIndex]->setOrigin(m_shotStartPosition, true);
            m_rockets[m_currentRocketIndex]->applyCentralImpulse(m_shotImpulseVector);

            ++m_currentRocketIndex;
            if(m_currentRocketIndex >= m_rockets.size())
                m_currentRocketIndex = 0;

            m_lastShotTime = EnumsAndVars::mapPlayTimeSec;
        }
    }
}
