#include "ShotGun.h"

namespace Survivor3rdPerson
{
    ShotGun::ShotGun(float reloadTime, int bulletsPerShot) : BaseWeapon(reloadTime, WeaponType::SHOT_GUN), m_bulletsPerShot(bulletsPerShot)
    {
        m_bullets.reserve(60);

        for(int i = 0; i < m_bullets.capacity(); ++i)
        {
            auto obj = std::make_shared<Beryll::SimpleCollidingObject>("models3D/weapon/ShotGunBullet.fbx",
                                                                       m_bulletMass,
                                                                       true,
                                                                       Beryll::CollisionFlags::DYNAMIC,
                                                                       EnumsAndVars::CollGr_WEAPON_BULLET,
                                                                       EnumsAndVars::CollGr_STATIC_ENV | EnumsAndVars::CollGr_ENEMY,
                                                                       EnumsAndVars::SceneGR_NONE);

            obj->disableUpdate();
            obj->disableCollisionMesh();
            obj->disableDraw();

            m_bullets.push_back(obj);
        }
    }

    ShotGun::~ShotGun()
    {
        for(const auto& bullet : m_bullets)
        {
            bullet->disableUpdate();
            bullet->disableCollisionMesh();
            bullet->disableDraw();
        }
    }

    void ShotGun::update(const glm::vec3& playerOrig, const glm::vec3& playerFaceDirXZ,
                         const std::vector<std::shared_ptr<BaseEnemy>>& enemies)
    {
        for(const auto& bullet : m_bullets)
        {
            if(bullet->getIsEnabledUpdate())
            {
                bullet->updateAfterPhysics();

                if(glm::length(bullet->getOrigin()) > 2500.0f) // Distance from origin (0.0.0).
                {
                    bullet->disableUpdate();
                    bullet->disableCollisionMesh();
                    bullet->disableDraw();
                }
            }
        }

        float angleBetweenWorldUpAndCameraBack = BeryllUtils::Common::getAngleInRadians(BeryllConstants::worldUp, Beryll::Camera::getCameraBackDirectionXYZ());
        m_bulletAngleRadians = angleBetweenWorldUpAndCameraBack - glm::half_pi<float>() + 0.3f; // + 0...f direct trajectory more up.

        m_bulletImpulseVector = playerFaceDirXZ;
        m_bulletImpulseVector.y = glm::tan(m_bulletAngleRadians);
        m_bulletImpulseVector = glm::normalize(m_bulletImpulseVector);
        m_bulletImpulseVector *= m_bulletMass;
        m_bulletImpulseVector *= 400.0f;

        m_bulletStartPosition = playerOrig + playerFaceDirXZ * 4.0f;
        m_bulletStartPosition.y += 4.0f;

        // Do damage.
        const int enemiesFirstID = enemies[0]->getObjID();
        const int enemiesLastID = enemies.back()->getObjID();
        for(auto& bullet : m_bullets)
        {
            if(bullet->getIsEnabledUpdate())
            {
                int collisionID = Beryll::Physics::getAnyCollisionForID(bullet->getID());
                if(collisionID >= enemiesFirstID && collisionID <= enemiesLastID)
                {
                    enemies[collisionID - enemiesFirstID]->takeDamage(1.0f);

                    // Damage on screen.
                    int number = Beryll::RandomGenerator::getInt(1000) + 1;
                    float numberHeight = std::max(2.5f, glm::distance(Beryll::Camera::getCameraPos(), bullet->getOrigin()) * 0.03f);
                    if(Beryll::RandomGenerator::getFloat() < 0.1f)
                    {
                        number *= 10;
                        numberHeight *= 3.0f;
                    }
                    Beryll::TextOnScene::addNumbersToShow(number, numberHeight, 0.5f, bullet->getOrigin() + glm::vec3{0.0f, 10.0f, 0.0f},
                                                          glm::vec3{Beryll::RandomGenerator::getFloat() * 10.0f - 5.0f,
                                                                    Beryll::RandomGenerator::getFloat() * 3.0f + 3.0f,
                                                                    Beryll::RandomGenerator::getFloat() * 10.0f - 5.0f},
                                                          60.0f);
                }
            }
        }
    }

    // Draw method can change shader because it can draw bullets trajectory and it has own shader.
    void ShotGun::draw(const glm::mat4& sunLightVPMatrix, const glm::vec3& sunLightDir, const std::shared_ptr<Beryll::Shader>& shader)
    {
        glm::mat4 modelMatrix{1.0f};

        for(auto& bullet : m_bullets)
        {
            if(bullet->getIsEnabledDraw())
            {
                modelMatrix = bullet->getModelMatrix();
                shader->setMatrix4x4Float("MVPLightMatrix", sunLightVPMatrix * modelMatrix);
                shader->setMatrix4x4Float("modelMatrix", modelMatrix);
                shader->setMatrix3x3Float("normalMatrix", glm::mat3(modelMatrix));
                Beryll::Renderer::drawObject(bullet, modelMatrix, shader);
            }
        }

        m_aimTrajectory.calculateAndDraw(m_bulletMass,
                                         glm::vec3{0.0f, -10.0f, 0.0f},
                                         m_bulletStartPosition,
                                         m_bulletAngleRadians,
                                         m_bulletImpulseVector,
                                         glm::vec3{1.0f},
                                         sunLightDir);
    }

    void ShotGun::shoot()
    {
        if(m_lastShotTime + m_reloadTime < EnumsAndVars::mapPlayTimeSec)
        {
            for(int i = 0; i < m_bulletsPerShot; ++i)
            {
                if(!m_bullets[m_currentBulletIndex]->getIsEnabledUpdate())
                {
                    m_bullets[m_currentBulletIndex]->enableDraw();
                    m_bullets[m_currentBulletIndex]->enableUpdate();
                    m_bullets[m_currentBulletIndex]->enableCollisionMesh();
                }

                // Rotate vec around axis by angle (in radians). Left - right.
                const float angleLeftRight = Beryll::RandomGenerator::getFloat() * 0.4f;
                glm::vec3 rotateForBullet = glm::rotate(m_bulletImpulseVector, angleLeftRight, Beryll::Camera::getCameraUp());
                rotateForBullet = glm::rotate(rotateForBullet, 0.2f, -Beryll::Camera::getCameraUp());
                // Up - down.
                const float angleUpDown = Beryll::RandomGenerator::getFloat() * 0.1f;
                rotateForBullet = glm::rotate(rotateForBullet, angleUpDown, Beryll::Camera::getCameraRightXYZ());
                rotateForBullet = glm::rotate(rotateForBullet, 0.05f, -Beryll::Camera::getCameraRightXYZ());

                m_bullets[m_currentBulletIndex]->setOrigin(m_bulletStartPosition, true);
                m_bullets[m_currentBulletIndex]->applyCentralImpulse(rotateForBullet);

                ++m_currentBulletIndex;
                if(m_currentBulletIndex >= m_bullets.size())
                    m_currentBulletIndex = 0;
            }

            m_lastShotTime = EnumsAndVars::mapPlayTimeSec;
        }
    }
}
