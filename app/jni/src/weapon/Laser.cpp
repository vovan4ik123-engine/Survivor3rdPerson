#include "Laser.h"

namespace Survivor3rdPerson
{
    Laser::Laser(float reloadTime) : BaseWeapon(reloadTime, WeaponType::LASER_GUN)
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
        m_laserAngleRadians = angleBetweenWorldUpAndCameraBack - glm::half_pi<float>() + 0.3f; // + 0...f direct trajectory more up.

        m_laserDirection = playerFaceDirXZ;
        m_laserDirection.y = glm::tan(m_laserAngleRadians);
        m_laserDirection = glm::normalize(m_laserDirection);

        m_laserStartPosition = playerOrig + playerFaceDirXZ * 4.0f;
        m_laserStartPosition.y += 4.0f;

        // Do damage.
        const int enemiesFirstID = enemies[0]->getObjID();
        const int enemiesLastID = enemies.back()->getObjID();
        for(const int enemyID : m_hittedEnemiesIDs)
        {
            if(enemyID >= enemiesFirstID && enemyID <= enemiesLastID)
            {
                enemies[enemyID - enemiesFirstID]->takeDamage(1.0f);

                // Damage on screen.
                int number = Beryll::RandomGenerator::getInt(1000) + 1;
                float numberHeight = std::max(2.5f, glm::distance(Beryll::Camera::getCameraPos(), enemies[enemyID - enemiesFirstID]->getObj()->getOrigin()) * 0.03f);
                if(Beryll::RandomGenerator::getFloat() < 0.1f)
                {
                    number *= 10;
                    numberHeight *= 3.0f;
                }
                Beryll::TextOnScene::addNumbersToShow(number, numberHeight, 0.5f, enemies[enemyID - enemiesFirstID]->getObj()->getOrigin() + glm::vec3{0.0f, 10.0f, 0.0f},
                                                      glm::vec3{Beryll::RandomGenerator::getFloat() * 10.0f - 5.0f,
                                                                Beryll::RandomGenerator::getFloat() * 3.0f + 3.0f,
                                                                Beryll::RandomGenerator::getFloat() * 10.0f - 5.0f},
                                                      60.0f);
            }
        }

        m_hittedEnemiesIDs.clear();
    }

    void Laser::draw(const glm::mat4& sunLightVPMatrix, const glm::vec3& sunLightDir, const std::shared_ptr<Beryll::Shader>& shader)
    {
        // Laser drawn by particle system.

        m_aimTrajectory.calculateAndDraw(1.0f,
                                         glm::vec3{0.0f, 0.0f, 0.0f},
                                         m_laserStartPosition,
                                         m_laserAngleRadians,
                                         m_laserDirection,
                                         glm::vec3{1.0f},
                                         sunLightDir);
    }

    void Laser::shoot()
    {
        if(m_lastShotTime + m_reloadTime < EnumsAndVars::mapPlayTimeSec)
        {
            Beryll::RayAllHits allHits = Beryll::Physics::castRayAllHits(m_laserStartPosition, m_laserStartPosition + (m_laserDirection * m_laserDistance),
                                                                         EnumsAndVars::CollGr_WEAPON_BULLET,
                                                                         EnumsAndVars::CollGr_ENEMY);
            if(allHits)
                m_hittedEnemiesIDs = std::move(allHits.hittedObjectsID);

            for(float dist = 0.0f; dist < m_laserDistance; dist += 4.0f)
            {
                const glm::vec3 pos = m_laserStartPosition + (m_laserDirection * dist);
                Beryll::ParticleSystem::EmitCubesFromCenter(1, 0.4f, 0.9f, 0.9f, glm::vec4{1.0f}, glm::vec4{1.0f, 1.0f, 1.0f, 0.0f}, pos, glm::vec3{0.0f}, 0.0f);
            }

            m_lastShotTime = EnumsAndVars::mapPlayTimeSec;
        }
    }
}
