#pragma once

#include "EngineHeaders.h"
#include "EnumsAndVariables.h"
#include "enemy/BaseEnemy.h"

namespace Survivor3rdPerson
{
    enum class WeaponType
    {
        NONE, BALL_GUN, SHOT_GUN, LASER_GUN, SWORD
    };

    class BaseWeapon
    {
    public:
        BaseWeapon() = delete;
        BaseWeapon(float reloadTime, WeaponType wType);
        virtual ~BaseWeapon();

        virtual void update(const glm::vec3& playerOrig, const glm::vec3& playerFaceDirXZ,
                            const std::vector<std::shared_ptr<BaseEnemy>>& enemies) = 0;
        // Draw method can change shader because it can draw bullets trajectory and it has own shader.
        virtual void draw(const glm::mat4& sunLightVPMatrix, const glm::vec3& sunLightDir, const std::shared_ptr<Beryll::Shader>& shader) = 0;
        virtual void shoot() = 0;
        const WeaponType weaponType = WeaponType::NONE;

    protected:
        float m_lastShotTime = -99999.0f;
        const float m_reloadTime = 1.0f; // Multiply by m_reloadTimeFactor.
        float m_reloadTimeFactor = 1.0f; // Used to increase or decrease m_reloadTime (by improvement or debuff).

    };
}
