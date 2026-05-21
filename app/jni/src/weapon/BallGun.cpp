#include "BallGun.h"

namespace Survivor3rdPerson
{
    BallGun::BallGun(const float reloadTime) : BaseWeapon(reloadTime, WeaponType::BALL_GUN)
    {
        m_bullets.reserve(15);

        for(int i = 0; i < m_bullets.capacity(); ++i)
        {
            auto obj = std::make_shared<Beryll::SimpleCollidingObject>("models3D/weapon/BallGunBullet.fbx",
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

    BallGun::~BallGun()
    {
        for(const auto& bullet : m_bullets)
        {
            bullet->disableUpdate();
            bullet->disableCollisionMesh();
            bullet->disableDraw();
        }
    }

    void BallGun::update(const glm::vec3& playerOrig, const glm::vec3& playerFaceDirXZ,
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
        m_shotAngleRadians = angleBetweenWorldUpAndCameraBack - glm::half_pi<float>() + 0.3f; // + 0...f direct trajectory more up.

        m_shotImpulseVector = playerFaceDirXZ;
        m_shotImpulseVector.y = glm::tan(m_shotAngleRadians);
        m_shotImpulseVector = glm::normalize(m_shotImpulseVector);
        m_shotImpulseVector *= m_bulletMass;
        m_shotImpulseVector *= 400.0f;

        m_shotStartPosition = playerOrig + playerFaceDirXZ * 4.0f;
        m_shotStartPosition.y += 4.0f;

        // Do damage.
        const int enemiesFirstID = enemies[0]->getObjID();
        const int enemiesLastID = enemies.back()->getObjID();
        for(const auto& bullet : m_bullets)
        {
            if(bullet->getIsEnabledUpdate())
            {
                int collisionID = Beryll::Physics::getAnyCollisionForID(bullet->getID());
                if(collisionID >= enemiesFirstID && collisionID <= enemiesLastID)
                {
                    int number = Beryll::RandomGenerator::getInt(1000) + 1;
                    float numberHeight = std::max(2.5f, glm::distance(Beryll::Camera::getCameraPos(), bullet->getOrigin()) * 0.03f);
                    if(Beryll::RandomGenerator::getFloat() < 0.1f)
                    {
                        number *= 10;
                        numberHeight *= 3.0f;
                    }

                    enemies[collisionID - enemiesFirstID]->takeDamage(number);
                    Beryll::TextOnScene::addNumbersToShow(number, numberHeight, 0.5f, bullet->getOrigin() + glm::vec3{0.0f, 10.0f, 0.0f},
                                                          glm::vec3{Beryll::RandomGenerator::getFloat() * 10.0f - 5.0f,
                                                                    Beryll::RandomGenerator::getFloat() * 3.0f + 3.0f,
                                                                    Beryll::RandomGenerator::getFloat() * 10.0f - 5.0f},
                                                          60.0f);
                }
            }
        }
    }

    // Draw method can change shader because it can draw aim trajectory and it has own shader.
    void BallGun::draw(const glm::mat4& sunLightVPMatrix, const glm::vec3& sunLightDir, const std::shared_ptr<Beryll::Shader>& shader)
    {
        glm::mat4 modelMatrix{1.0f};

        for(const auto& bullet : m_bullets)
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
                                         m_shotStartPosition,
                                         m_shotAngleRadians,
                                         m_shotImpulseVector,
                                         glm::vec3{1.0f},
                                         sunLightDir);
    }

    void BallGun::shoot()
    {
        if(m_lastShotTime + m_reloadTime < EnumsAndVars::mapPlayTimeSec)
        {
            if(!m_bullets[m_currentBulletIndex]->getIsEnabledUpdate())
            {
                m_bullets[m_currentBulletIndex]->enableDraw();
                m_bullets[m_currentBulletIndex]->enableUpdate();
                m_bullets[m_currentBulletIndex]->enableCollisionMesh();
            }
            m_bullets[m_currentBulletIndex]->setOrigin(m_shotStartPosition, true);
            m_bullets[m_currentBulletIndex]->applyCentralImpulse(m_shotImpulseVector);

            ++m_currentBulletIndex;
            if(m_currentBulletIndex >= m_bullets.size())
                m_currentBulletIndex = 0;

            m_lastShotTime = EnumsAndVars::mapPlayTimeSec;
        }
    }
}
