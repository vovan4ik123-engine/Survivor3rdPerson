#include "Sword.h"

namespace Survivor3rdPerson
{
    Sword::Sword(float reloadTime, float distance, float angleRad) : BaseWeapon(reloadTime, WeaponType::SWORD)
    {
        m_attackDistance = distance;
        m_attackAngleRad = angleRad;
    }

    Sword::~Sword()
    {

    }

    void Sword::update(const glm::vec3& playerOrig, const glm::vec3& playerFaceDirXZ,
                const std::vector<std::shared_ptr<BaseEnemy>>& enemies)
    {
        // Do damage.
        if(m_applyDamage)
        {
            for(const auto& enemy : enemies)
            {
                glm::vec3 enemyDir = enemy->getObj()->getOrigin() - playerOrig;
                enemyDir.y = 0.0f;
                if(glm::distance(playerOrig, enemy->getObj()->getOrigin()) < m_attackDistance && glm::length(enemyDir) > 0.0f &&
                   BeryllUtils::Common::getAngleInRadians(playerFaceDirXZ, glm::normalize(enemyDir)) < m_attackAngleRad)
                {
                    enemy->takeDamage(1.0f);

                    // Damage on screen.
                    int number = Beryll::RandomGenerator::getInt(1000) + 1;
                    float numberHeight = std::max(2.5f, glm::distance(Beryll::Camera::getCameraPos(), enemy->getObj()->getOrigin()) * 0.03f);
                    if(Beryll::RandomGenerator::getFloat() < 0.1f)
                    {
                        number *= 10;
                        numberHeight *= 3.0f;
                    }
                    Beryll::TextOnScene::addNumbersToShow(number, numberHeight, 0.5f, enemy->getObj()->getOrigin() + glm::vec3{0.0f, 10.0f, 0.0f},
                                                          glm::vec3{Beryll::RandomGenerator::getFloat() * 10.0f - 5.0f,
                                                                    Beryll::RandomGenerator::getFloat() * 3.0f + 3.0f,
                                                                    Beryll::RandomGenerator::getFloat() * 10.0f - 5.0f},
                                                          60.0f);
                }
            }
        }

        m_applyDamage = false;
    }

    void Sword::draw(const glm::mat4& sunLightVPMatrix, const glm::vec3& sunLightDir, const std::shared_ptr<Beryll::Shader>& shader)
    {
        // Oo.
    }

    void Sword::shoot()
    {
        if(m_lastShotTime + m_reloadTime < EnumsAndVars::mapPlayTimeSec)
        {
            m_applyDamage = true;
            m_lastShotTime = EnumsAndVars::mapPlayTimeSec;
        }
    }
}
