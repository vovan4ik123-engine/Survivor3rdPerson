#pragma once

#include "EngineHeaders.h"
#include "EnumsAndVariables.h"

namespace Survivor3rdPerson
{
    class PlayerBullet
    {
    public:
        PlayerBullet() = delete;
        PlayerBullet(const char* filePath,  // Common params.
                     float collisionMassKg,    // Physics params.
                     bool wantCollisionCallBack,
                     Beryll::CollisionFlags collFlag,
                     Beryll::CollisionGroups collGroup,
                     Beryll::CollisionGroups collMask,
                     Beryll::SceneObjectGroups sceneGroup);
        ~PlayerBullet();

        std::shared_ptr<Beryll::SimpleCollidingObject> getObj() { return m_obj; }
        int getObjID() { return m_objID; }

        void enableBullet();
        void disableBullet();
        bool getIsEnabled() { return m_isEnabled; }
        void shoot(const glm::vec3& position, const glm::vec3& impulse);

    private:
        std::shared_ptr<Beryll::SimpleCollidingObject> m_obj;
        int m_objID = 0;
        bool m_isEnabled = false;
    };
}
