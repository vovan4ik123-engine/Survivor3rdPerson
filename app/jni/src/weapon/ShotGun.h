#pragma once

#include "BaseWeapon.h"
#include "WeaponAimTrajectory.h"

namespace Survivor3rdPerson
{
    class ShotGun : public BaseWeapon
    {
    public:
        ShotGun() = delete;
        ShotGun(float reloadTime, int bulletsPerShot);
        ~ShotGun() override;

        void update(const glm::vec3& playerOrig, const glm::vec3& playerFaceDirXZ,
                    const std::vector<std::shared_ptr<BaseEnemy>>& enemies) override;
        // Draw method can change shader because it can draw bullets trajectory and it has own shader.
        void draw(const glm::mat4& sunLightVPMatrix, const glm::vec3& sunLightDir, const std::shared_ptr<Beryll::Shader>& shader) override;
        void shoot() override;

    protected:

    private:
        std::vector<std::shared_ptr<Beryll::SimpleCollidingObject>> m_bullets;
        int m_bulletsPerShot = 10;
        int m_currentBulletIndex = 0;
        const float m_bulletMass = 0.1f;
        glm::vec3 m_bulletStartPosition{0.0f};
        glm::vec3 m_bulletImpulseVector{0.0f};
        float m_bulletAngleRadians = 0.0f;
        WeaponAimTrajectory m_aimTrajectory;
    };
}
