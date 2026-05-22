#include "Laser.h"

namespace Survivor3rdPerson
{
    Laser::Laser(const float reloadTime) : BaseWeapon(reloadTime, WeaponType::LASER_GUN)
    {
        m_hittedEnemiesIDs.reserve(100);
    }

    Laser::~Laser()
    {

    }

    void Laser::update(const glm::vec3& playerOrig, const glm::vec3& playerFaceDirXZ,
                       const std::vector<std::shared_ptr<BaseEnemy>>& enemies)
    {
        float angleBetweenWorldUpAndCameraBack = BeryllUtils::Common::getAngleInRadians(BeryllConstants::worldUp, Beryll::Camera::getCameraBackDirectionXYZ());
        m_shotAngleRadians = angleBetweenWorldUpAndCameraBack - glm::half_pi<float>() + 0.3f; // + 0...f direct trajectory more up.

        m_shotImpulseVector = playerFaceDirXZ;
        m_shotImpulseVector.y = glm::tan(m_shotAngleRadians);
        m_shotImpulseVector = glm::normalize(m_shotImpulseVector);

        m_shotStartPosition = playerOrig + playerFaceDirXZ * 4.0f;
        m_shotStartPosition.y += 4.0f;

        // Do damage.
        const int enemiesFirstID = enemies[0]->getObjID();
        const int enemiesLastID = enemies.back()->getObjID();
        for(const int enemyID : m_hittedEnemiesIDs)
        {
            if(enemyID >= enemiesFirstID && enemyID <= enemiesLastID)
            {
                int damage = Beryll::RandomGenerator::getInt(1000) + 1;
                bool critical = false;
                if(Beryll::RandomGenerator::getFloat() < 0.1f)
                {
                    damage *= 10;
                    critical = true;
                }

                enemies[enemyID - enemiesFirstID]->takeDamage(damage);
                showDamage(damage, enemies[enemyID - enemiesFirstID]->getObj()->getOrigin(), critical);
            }
        }

        m_hittedEnemiesIDs.clear();
    }

    void Laser::draw(const glm::mat4& sunLightVPMatrix, const glm::vec3& sunLightDir, const std::shared_ptr<Beryll::Shader>& shader)
    {
        // Laser drawn by particle system.

        m_aimTrajectory.calculateAndDraw(1.0f,
                                         0.0f,
                                         m_shotStartPosition,
                                         m_shotAngleRadians,
                                         m_shotImpulseVector,
                                         glm::vec3{1.0f},
                                         sunLightDir);
    }

    void Laser::shoot()
    {
        if(m_lastShotTime + m_reloadTime < EnumsAndVars::mapPlayTimeSec)
        {
            Beryll::RayAllHits allHits = Beryll::Physics::castRayAllHits(m_shotStartPosition, m_shotStartPosition + (m_shotImpulseVector * m_shotDistance),
                                                                         EnumsAndVars::CollGr_WEAPON_BULLET,
                                                                         EnumsAndVars::CollGr_ENEMY);
            if(allHits)
                m_hittedEnemiesIDs = std::move(allHits.hittedObjectsID);

            for(float dist = 0.0f; dist < m_shotDistance; dist += 4.0f)
            {
                const glm::vec3 pos = m_shotStartPosition + (m_shotImpulseVector * dist);
                Beryll::ParticleSystem::EmitCubesFromCenter(1, 0.4f, 0.9f, 0.9f, glm::vec4{1.0f}, glm::vec4{1.0f, 1.0f, 1.0f, 0.0f}, pos, glm::vec3{0.0f}, 0.0f);
            }

            m_lastShotTime = EnumsAndVars::mapPlayTimeSec;
        }
    }
}
