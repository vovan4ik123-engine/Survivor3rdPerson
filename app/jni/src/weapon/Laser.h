#pragma once

#include "BaseWeapon.h"
#include "WeaponAimTrajectory.h"

namespace Survivor3rdPerson
{
    class Laser : public BaseWeapon
    {
    public:
        Laser() = delete;
        Laser(const float reloadTime);
        ~Laser() override;

        void update(const glm::vec3& playerOrig, const glm::vec3& playerFaceDirXZ,
                    const std::vector<std::shared_ptr<BaseEnemy>>& enemies) override;
        // Draw method can change shader because it can draw aim trajectory and it has own shader.
        void draw(const glm::mat4& sunLightVPMatrix, const glm::vec3& sunLightDir, const std::shared_ptr<Beryll::Shader>& shader) override;
        void shoot() override;

    protected:

    private:
        glm::vec3 m_shotStartPosition{0.0f};
        glm::vec3 m_shotImpulseVector{0.0f};
        float m_shotAngleRadians = 0.0f;
        float m_shotDistance = 300.0f;
        std::vector<int> m_hittedEnemiesIDs;
        WeaponAimTrajectory m_aimTrajectory;
    };
}
