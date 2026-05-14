#pragma once

#include "EngineHeaders.h"
#include "EnumsAndVariables.h"
#include "enemy/BaseEnemy.h"

namespace Survivor3rdPerson
{
    class BaseWeapon
    {
    public:
        BaseWeapon() = delete;
        BaseWeapon(float shotDelay);
        virtual ~BaseWeapon();

        virtual void update(const glm::vec3& playerOrig, const glm::vec3& playerFaceDirXZ,
                            const std::vector<std::shared_ptr<BaseEnemy>>& enemies) = 0;
        // Draw method can change shader because it can draw bullets trajectory and it has own shader.
        virtual void draw(const glm::mat4& sunLightVPMatrix, const glm::vec3& sunLightDir, const std::shared_ptr<Beryll::Shader>& shader) = 0;
        virtual void shoot() = 0;

    protected:
        float m_lastShotTime = -99999.0f;
        const float m_shotDelay = 1.0f; // Multiply by m_shotDelayFactor.
        float m_shotDelayFactor = 1.0f; // Used to increase or decrease m_shotDelay (by improvement or debuff).

    };
}
