#include "Sword.h"

namespace Survivor3rdPerson
{
    Sword::Sword(const float reloadTime, const float distance, const float angleRad) : BaseWeapon(reloadTime, WeaponType::SWORD)
    {
        m_attackDistance = distance;
        m_attackDistanceSquared = distance * distance;
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
                if(glm::distance2(playerOrig, enemy->getObj()->getOrigin()) < m_attackDistanceSquared && glm::length(enemyDir) > 0.0f &&
                   BeryllUtils::Common::getAngleInRadians(playerFaceDirXZ, glm::normalize(enemyDir)) < m_attackAngleRad)
                {
                    int damage = Beryll::RandomGenerator::getInt(1000) + 1;
                    bool critical = false;
                    if(Beryll::RandomGenerator::getFloat() < 0.1f)
                    {
                        damage *= 10;
                        critical = true;
                    }

                    enemy->takeDamage(damage);
                    showDamage(damage, enemy->getObj()->getOrigin(), critical);
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
