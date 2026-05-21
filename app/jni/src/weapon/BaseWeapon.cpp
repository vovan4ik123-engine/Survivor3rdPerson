#include "BaseWeapon.h"

namespace Survivor3rdPerson
{
    BaseWeapon::BaseWeapon(const float reloadTime, WeaponType wType) : m_reloadTime(reloadTime), weaponType(wType)
    {

    }

    BaseWeapon::~BaseWeapon()
    {

    }
}
