#include "Grenade.h"

namespace Survivor3rdPerson
{
    Grenade::Grenade(const float reloadTime, const float damageRadius) : BaseWeapon(reloadTime, WeaponType::GRENADE_GUN)
    {
        m_damageRadius = damageRadius;
        m_damageRadiusSquared = damageRadius * damageRadius;

        m_grenades.reserve(15);

        for(int i = 0; i < m_grenades.capacity(); ++i)
        {
            auto obj = std::make_shared<Beryll::SimpleCollidingObject>("models3D/weapon/Grenade.fbx",
                                                                       m_grenadeMass,
                                                                       true,
                                                                       Beryll::CollisionFlags::DYNAMIC,
                                                                       EnumsAndVars::CollGr_WEAPON_BULLET,
                                                                       EnumsAndVars::CollGr_STATIC_ENV | EnumsAndVars::CollGr_ENEMY,
                                                                       EnumsAndVars::SceneGR_NONE);

            obj->disableUpdate();
            obj->disableCollisionMesh();
            obj->disableDraw();

            m_grenades.push_back(obj);
        }
    }

    Grenade::~Grenade()
    {
        for(const auto& grenade : m_grenades)
        {
            grenade->disableUpdate();
            grenade->disableCollisionMesh();
            grenade->disableDraw();
        }
    }

    void Grenade::update(const glm::vec3& playerOrig, const glm::vec3& playerFaceDirXZ,
                         const std::vector<std::shared_ptr<BaseEnemy>>& enemies)
    {
        for(const auto& grenade : m_grenades)
        {
            if(grenade->getIsEnabledUpdate())
            {
                grenade->updateAfterPhysics();

                if(glm::length(grenade->getOrigin()) > 2500.0f) // Distance from origin (0.0.0).
                {
                    grenade->disableUpdate();
                    grenade->disableCollisionMesh();
                    grenade->disableDraw();
                }
            }
        }

        float angleBetweenWorldUpAndCameraBack = BeryllUtils::Common::getAngleInRadians(BeryllConstants::worldUp, Beryll::Camera::getCameraBackDirectionXYZ());
        m_shotAngleRadians = angleBetweenWorldUpAndCameraBack - glm::half_pi<float>() + 1.1f; // + 0...f direct trajectory more up.

        m_shotImpulseVector = playerFaceDirXZ;
        m_shotImpulseVector.y = glm::tan(m_shotAngleRadians);
        m_shotImpulseVector = glm::normalize(m_shotImpulseVector);
        m_shotImpulseVector *= m_grenadeMass;
        m_shotImpulseVector *= 140.0f;

        m_shotStartPosition = playerOrig + playerFaceDirXZ * 4.0f;
        m_shotStartPosition.y += 4.0f;

        // Do damage.
        for(const auto& grenade : m_grenades)
        {
            if(grenade->getIsEnabledUpdate())
            {
                int collisionID = Beryll::Physics::getAnyCollisionForID(grenade->getID());
                if(collisionID > 0)
                {
                    Beryll::ParticleSystem::EmitCubesFromCenter(6, 0.4f, m_damageRadius * 0.12f, m_damageRadius * 0.25f,
                                                                glm::vec4{0.957f, 0.957f, 0.082f, 1.0f}, glm::vec4{0.867f, 0.164f, 0.07f, 0.2f},
                                                                grenade->getOrigin(),
                                                                glm::vec3{Beryll::RandomGenerator::getFloat() * 8.0f - 4.0f,
                                                                          Beryll::RandomGenerator::getFloat() * 8.0f - 4.0f,
                                                                          Beryll::RandomGenerator::getFloat() * 8.0f - 4.0f},
                                                                20.0f);

                    grenade->disableUpdate();
                    grenade->disableCollisionMesh();
                    grenade->disableDraw();

                    for(const auto& enemy : enemies)
                    {
                        if(enemy->getIsEnabled() && enemy->unitState != EnemyState::DYING &&
                           glm::distance2(enemy->getObj()->getOrigin(), grenade->getOrigin()) < m_damageRadiusSquared)
                        {
                            // Grenade damage all enemies in radius but do damage only to enemies in front of player.
                            glm::vec3 enemyDirXZ = enemy->getObj()->getOrigin() - playerOrig;
                            enemyDirXZ.y = 0.0f;
                            if(BeryllUtils::Common::getAngleInRadians(playerFaceDirXZ, glm::normalize(enemyDirXZ)) < 2.0f)
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
    void Grenade::draw(const glm::mat4& sunLightVPMatrix, const glm::vec3& sunLightDir, const std::shared_ptr<Beryll::Shader>& shader)
    {
        glm::mat4 modelMatrix{1.0f};

        for(const auto& grenade : m_grenades)
        {
            if(grenade->getIsEnabledDraw())
            {
                modelMatrix = grenade->getModelMatrix();
                shader->setMatrix4x4Float("MVPLightMatrix", sunLightVPMatrix * modelMatrix);
                shader->setMatrix4x4Float("modelMatrix", modelMatrix);
                shader->setMatrix3x3Float("normalMatrix", glm::mat3(modelMatrix));
                Beryll::Renderer::drawObject(grenade, modelMatrix, shader);
            }
        }

        m_aimTrajectory.calculateAndDraw(m_grenadeMass,
                                         m_grenadeGravity,
                                         m_shotStartPosition,
                                         m_shotAngleRadians,
                                         m_shotImpulseVector,
                                         glm::vec3{1.0f},
                                         sunLightDir);
    }

    void Grenade::shoot()
    {
        if(m_lastShotTime + m_reloadTime < EnumsAndVars::mapPlayTimeSec)
        {
            if(!m_grenades[m_currentGrenadeIndex]->getIsEnabledUpdate())
            {
                m_grenades[m_currentGrenadeIndex]->enableDraw();
                m_grenades[m_currentGrenadeIndex]->enableUpdate();
                m_grenades[m_currentGrenadeIndex]->enableCollisionMesh();
            }
            m_grenades[m_currentGrenadeIndex]->setGravity(m_grenadeGravity);
            m_grenades[m_currentGrenadeIndex]->setOrigin(m_shotStartPosition, true);
            m_grenades[m_currentGrenadeIndex]->applyCentralImpulse(m_shotImpulseVector);

            ++m_currentGrenadeIndex;
            if(m_currentGrenadeIndex >= m_grenades.size())
                m_currentGrenadeIndex = 0;

            m_lastShotTime = EnumsAndVars::mapPlayTimeSec;
        }
    }
}
