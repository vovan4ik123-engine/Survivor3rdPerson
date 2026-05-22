#include "Player.h"
#include "EnumsAndVariables.h"
#include "Sounds.h"

namespace Survivor3rdPerson
{
    Player::Player(const char* filePath,  // Common params.
                   bool wantCollisionCallBack, // Physics params.
                   Beryll::CollisionFlags collFlag,
                   Beryll::CollisionGroups collGroup,
                   Beryll::CollisionGroups collMask,
                   Beryll::SceneObjectGroups sceneGroup)
                   : Beryll::SimpleCollidingCharacter(filePath,
                                                      1.0f,
                                                      wantCollisionCallBack,
                                                      collFlag,
                                                      collGroup,
                                                      collMask,
                                                      sceneGroup)
    {
        m_maxHP = 100.0f;
        m_currentHP = m_maxHP;
        Beryll::SimpleCollidingCharacter::setGravity(glm::vec3{0.0f, -70.0f, 0.0f});
    }

    Player::~Player()
    {

    }

    void Player::update()
    {
        if(Beryll::Physics::getIsCollisionWithGroup(getID(), EnumsAndVars::CollGr_JUMPPAD))
        {
            if(getController().jump(glm::vec3(0.0f, 120.0f, 0.0f)))
                Sounds::playSoundEffect(SoundType::JUMPPAD);
        }
    }
}
