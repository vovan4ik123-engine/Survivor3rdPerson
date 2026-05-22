#include "PlasmaGun.h"

namespace Survivor3rdPerson
{
    PlasmaGun::PlasmaGun(const float reloadTime, const int maxEnemiesHit) : BaseWeapon(reloadTime, WeaponType::PLASMA_GUN)
    {
        m_maxEnemiesHit = maxEnemiesHit;
        if(m_maxEnemiesHit > 20)
            m_maxEnemiesHit = 20;

        m_plasmas.reserve(30);

        for(int i = 0; i < m_plasmas.capacity(); ++i)
        {
            m_plasmas.emplace_back(Plasma{});
            m_plasmas.back().obj = std::make_shared<Beryll::SimpleObject>("models3D/weapon/Plasma.fbx", EnumsAndVars::SceneGR_NONE);
            m_plasmas.back().disable();
        }
    }

    PlasmaGun::~PlasmaGun()
    {
        for(Plasma& plasma : m_plasmas)
        {
            plasma.disable();
        }
    }

    void PlasmaGun::update(const glm::vec3& playerOrig, const glm::vec3& playerFaceDirXZ,
                           const std::vector<std::shared_ptr<BaseEnemy>>& enemies)
    {
        if(m_allEnemies.empty())
            m_allEnemies = enemies;

        for(Plasma& plasma : m_plasmas)
        {
            if(plasma.active && glm::length(plasma.obj->getOrigin()) > 5000.0f)
                plasma.disable();
        }

        float angleBetweenWorldUpAndCameraBack = BeryllUtils::Common::getAngleInRadians(BeryllConstants::worldUp, Beryll::Camera::getCameraBackDirectionXYZ());
        m_shotAngleRadians = angleBetweenWorldUpAndCameraBack - glm::half_pi<float>() + 0.3f; // + 0...f direct trajectory more up.

        m_shotImpulseVector = playerFaceDirXZ;
        m_shotImpulseVector.y = glm::tan(m_shotAngleRadians);
        m_shotImpulseVector = glm::normalize(m_shotImpulseVector);

        m_shotStartPosition = playerOrig + playerFaceDirXZ * 4.0f;
        m_shotStartPosition.y += 4.0f;

        bool plasmaNeedMove = true;
        for(Plasma& plasma : m_plasmas)
        {
            if(!plasma.active)
                continue;

            plasmaNeedMove = true;
            glm::vec3 moveVec = plasma.moveDir * m_plasmaSpeed * Beryll::TimeStep::getTimeStepSec(); // Consider speed and time.

            if(plasma.lookForFirstEnemy)
            {
                plasma.enemiesToHit.clear();
                plasma.currentEnemyIndex = 0;

                std::sort(m_allEnemies.begin(), m_allEnemies.end(), [&](const std::shared_ptr<BaseEnemy>& e1, const std::shared_ptr<BaseEnemy>& e2)
                {
                    return (glm::distance2(plasma.obj->getOrigin(), e1->getObj()->getOrigin()) < glm::distance2(plasma.obj->getOrigin(), e2->getObj()->getOrigin()));
                });

                // Collect more enemies than m_maxEnemiesHit because some can die before plasma reach them.
                for(int i = 0; i < m_allEnemies.size() && plasma.enemiesToHit.size() < m_maxEnemiesHit * 2; ++i)
                {
                    const glm::vec3 enemyDir = m_allEnemies[i]->getObj()->getOrigin() - playerOrig;
                    if(m_allEnemies[i]->getIsEnabled() && m_allEnemies[i]->unitState != EnemyState::DYING &&
                       BeryllUtils::Common::getAngleInRadians(playerFaceDirXZ, glm::normalize(enemyDir)) < 0.9f)
                    {
                        if(plasma.lookForFirstEnemy && glm::distance2(plasma.obj->getOrigin(), m_allEnemies[i]->getObj()->getOrigin()) < 30.0f * 30.0f)
                            plasma.lookForFirstEnemy = false; // First enemy found.

                        if(!plasma.lookForFirstEnemy) // Collecting enemies to hit.
                            plasma.enemiesToHit.push_back(m_allEnemies[i]);
                    }
                }
            }
            else
            {
                const std::shared_ptr<BaseEnemy> enemyToMove = plasma.enemiesToHit[plasma.currentEnemyIndex];
                const glm::vec3 needMove = enemyToMove->getObj()->getOrigin() - plasma.obj->getOrigin();
                plasma.moveDir = glm::normalize(needMove);
                moveVec = plasma.moveDir * m_plasmaSpeed * Beryll::TimeStep::getTimeStepSec(); // Consider speed and time.
                if(glm::any(glm::isnan(needMove)) || glm::length2(needMove) < 0.01f || glm::length2(moveVec) > glm::length2(needMove))
                {
                    plasma.obj->setOrigin(enemyToMove->getObj()->getOrigin());
                    plasmaNeedMove = false; // Already moved to enemy origin.
                    ++plasma.enemiesHitCount;
                    // Do damage to this enemy.
                    int damage = Beryll::RandomGenerator::getInt(1000) + 1;
                    bool critical = false;
                    if(Beryll::RandomGenerator::getFloat() < 0.1f)
                    {
                        damage *= 10;
                        critical = true;
                    }

                    enemyToMove->takeDamage(damage);
                    showDamage(damage, enemyToMove->getObj()->getOrigin(), critical);
                    // And go to next enemy.
                    ++plasma.currentEnemyIndex;
                    for(; plasma.currentEnemyIndex < plasma.enemiesToHit.size(); ++plasma.currentEnemyIndex)
                    {
                        if(plasma.enemiesToHit[plasma.currentEnemyIndex]->getIsEnabled() &&
                           plasma.enemiesToHit[plasma.currentEnemyIndex]->unitState != EnemyState::DYING)
                            break;
                    }

                    if(plasma.currentEnemyIndex >= plasma.enemiesToHit.size() || plasma.enemiesHitCount >= m_maxEnemiesHit)
                        plasma.disable();
                }
            }

            if(plasma.active && plasmaNeedMove)
            {
                plasma.obj->addToRotation(glm::rotation(plasma.obj->getFaceDirXYZ(), plasma.moveDir));
                const glm::vec3 newOrig = plasma.obj->getOrigin() + moveVec;
                plasma.obj->setOrigin(newOrig);
            }
        }
    }

    // Draw method can change shader because it can draw aim trajectory and it has own shader.
    void PlasmaGun::draw(const glm::mat4& sunLightVPMatrix, const glm::vec3& sunLightDir, const std::shared_ptr<Beryll::Shader>& shader)
    {
        glm::mat4 modelMatrix{1.0f};
        shader->set1Float("ambientLight", 0.8f);

        for(Plasma& plasma : m_plasmas)
        {
            if(plasma.obj->getIsEnabledDraw())
            {
                modelMatrix = plasma.obj->getModelMatrix();
                shader->setMatrix4x4Float("MVPLightMatrix", sunLightVPMatrix * modelMatrix);
                shader->setMatrix4x4Float("modelMatrix", modelMatrix);
                shader->setMatrix3x3Float("normalMatrix", glm::mat3(modelMatrix));
                Beryll::Renderer::drawObject(plasma.obj, modelMatrix, shader);
            }
        }

        m_aimTrajectory.calculateAndDraw(1.0f,
                                         0.0f,
                                         m_shotStartPosition,
                                         m_shotAngleRadians,
                                         m_shotImpulseVector,
                                         glm::vec3{1.0f},
                                         sunLightDir);
    }

    void PlasmaGun::shoot()
    {
        if(m_lastShotTime + m_reloadTime < EnumsAndVars::mapPlayTimeSec)
        {
            m_plasmas[m_currentPlasmaIndex].activate(m_shotStartPosition, m_shotImpulseVector);

            ++m_currentPlasmaIndex;
            if(m_currentPlasmaIndex >= m_plasmas.size())
                m_currentPlasmaIndex = 0;

            m_lastShotTime = EnumsAndVars::mapPlayTimeSec;
        }
    }
}
