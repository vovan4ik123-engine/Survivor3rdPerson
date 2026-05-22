#include "BaseWeapon.h"

namespace Survivor3rdPerson
{
    BaseWeapon::BaseWeapon(const float reloadTime, WeaponType wType) : m_reloadTime(reloadTime), weaponType(wType)
    {

    }

    BaseWeapon::~BaseWeapon()
    {

    }

    void BaseWeapon::showDamage(const int damage, const glm::vec3& pos, const bool crit)
    {
        float numberHeight = std::max(2.5f, glm::distance(Beryll::Camera::getCameraPos(), pos) * 0.03f);
        if(crit)
            numberHeight *= 3.0f;
        Beryll::TextOnScene::addNumbersToShow(damage, numberHeight, 0.5f, pos + glm::vec3{0.0f, 10.0f, 0.0f},
                                              glm::vec3{Beryll::RandomGenerator::getFloat() * 10.0f - 5.0f,
                                                        Beryll::RandomGenerator::getFloat() * 3.0f + 3.0f,
                                                        Beryll::RandomGenerator::getFloat() * 10.0f - 5.0f},
                                              60.0f);
    }
}
