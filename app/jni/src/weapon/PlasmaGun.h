#pragma once

#include "BaseWeapon.h"
#include "WeaponAimTrajectory.h"

namespace Survivor3rdPerson
{
    class PlasmaGun : public BaseWeapon
    {
    public:
        PlasmaGun() = delete;
        PlasmaGun(const float reloadTime, const int maxEnemiesHit);
        ~PlasmaGun() override;

        void update(const glm::vec3& playerOrig, const glm::vec3& playerFaceDirXZ,
                    const std::vector<std::shared_ptr<BaseEnemy>>& enemies) override;
        // Draw method can change shader because it can draw aim trajectory and it has own shader.
        void draw(const glm::mat4& sunLightVPMatrix, const glm::vec3& sunLightDir, const std::shared_ptr<Beryll::Shader>& shader) override;
        void shoot() override;

    protected:

    private:
        struct Plasma
        {
            bool active = false;
            bool lookForFirstEnemy = false;
            std::shared_ptr<Beryll::SimpleObject> obj;
            glm::vec3 moveDir{0.0f};
            std::vector<std::shared_ptr<BaseEnemy>> enemiesToHit{20, nullptr};
            int currentEnemyIndex = 0;
            int enemiesHitCount = 0;

            void disable()
            {
                active = false;
                obj->setOrigin(glm::vec3{0.0f, 999.0f, 0.0f});
                obj->disableUpdate();
                obj->disableDraw();
            }

            void activate(const glm::vec3& orig, const glm::vec3& dir)
            {
                active = true;
                lookForFirstEnemy = true;
                obj->setOrigin(orig);
                obj->enableUpdate();
                obj->enableDraw();
                moveDir = glm::normalize(dir);
                obj->addToRotation(glm::rotation(obj->getFaceDirXYZ(), moveDir));
                enemiesToHit.clear();
                currentEnemyIndex = 0;
                enemiesHitCount = 0;
            }
        };
        std::vector<Plasma> m_plasmas;
        int m_currentPlasmaIndex = 0;
        int m_maxEnemiesHit = 10;
        const float m_plasmaSpeed = 200.0f;
        glm::vec3 m_shotStartPosition{0.0f};
        glm::vec3 m_shotImpulseVector{0.0f};
        float m_shotAngleRadians = 0.0f;
        WeaponAimTrajectory m_aimTrajectory;
        std::vector<std::shared_ptr<BaseEnemy>> m_allEnemies;
    };
}
