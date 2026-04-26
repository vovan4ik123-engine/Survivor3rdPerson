#include "Player.h"
#include "EnumsAndVariables.h"
#include "Sounds.h"

namespace Survivor3rdPerson
{
    Player::Player(const char* filePath,  // Common params.
                   float collisionMassKg,    // Physics params.
                   bool wantCollisionCallBack,
                   Beryll::CollisionFlags collFlag,
                   Beryll::CollisionGroups collGroup,
                   Beryll::CollisionGroups collMask,
                   Beryll::SceneObjectGroups sceneGroup,
                   const float HP)
                   : Beryll::SimpleCollidingCharacter(filePath,
                                                      collisionMassKg,
                                                      wantCollisionCallBack,
                                                      collFlag,
                                                      collGroup,
                                                      collMask,
                                                      sceneGroup)
    {
        m_maxHP = HP;
        m_currentHP = HP;
    }

    Player::~Player()
    {

    }

    void Player::update()
    {
        if(Beryll::Physics::getIsCollisionWithGroup(getID(), Beryll::CollisionGroups::JUMPPAD))
        {
            if(getController().jump(glm::vec3(0.0f, 120.0f, 0.0f)))
                Sounds::playSoundEffect(SoundType::JUMPPAD);
        }
    }
}
