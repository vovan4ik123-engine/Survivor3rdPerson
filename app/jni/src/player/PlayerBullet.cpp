#include "PlayerBullet.h"

namespace Survivor3rdPerson
{
    PlayerBullet::PlayerBullet(const char* filePath,
                               float collisionMassKg,
                               bool wantCollisionCallBack,
                               Beryll::CollisionFlags collFlag,
                               Beryll::CollisionGroups collGroup,
                               Beryll::CollisionGroups collMask,
                               Beryll::SceneObjectGroups sceneGroup)
    {
        m_obj = std::make_shared<Beryll::SimpleCollidingObject>(filePath,
                                                                collisionMassKg,
                                                                wantCollisionCallBack,
                                                                collFlag,
                                                                collGroup,
                                                                collMask,
                                                                sceneGroup);

        m_objID = m_obj->getID();

        disableBullet();
    }

    PlayerBullet::~PlayerBullet()
    {
        disableBullet();
    }

    void PlayerBullet::enableBullet()
    {
        m_obj->enableDraw();
        m_obj->enableUpdate();
        m_obj->enableCollisionMesh();

        m_isEnabled = true;
    }

    void PlayerBullet::disableBullet()
    {
        m_obj->disableDraw();
        m_obj->disableUpdate();
        m_obj->disableCollisionMesh();

        m_isEnabled = false;
    }

    void PlayerBullet::shoot(const glm::vec3& position, const glm::vec3& impulse)
    {
        enableBullet();

        m_obj->setOrigin(position, true);
        m_obj->applyCentralImpulse(impulse);
    }
}
