#pragma once

#include "BaseWeapon.h"

namespace Survivor3rdPerson
{
    class Sword : public BaseWeapon
    {
    public:
        Sword() = delete;
        Sword(float reloadTime, float distance, float angleRad);
        ~Sword() override;
        void update(const glm::vec3& playerOrig, const glm::vec3& playerFaceDirXZ,
                    const std::vector<std::shared_ptr<BaseEnemy>>& enemies) override;
        // Draw method can change shader because it can draw bullets trajectory and it has own shader.
        void draw(const glm::mat4& sunLightVPMatrix, const glm::vec3& sunLightDir, const std::shared_ptr<Beryll::Shader>& shader) override;
        void shoot() override;

    protected:

    private:
        float m_attackDistance = 10.0f;
        float m_attackAngleRad = glm::radians(20.0f);
        bool m_applyDamage = false;
    };
}
