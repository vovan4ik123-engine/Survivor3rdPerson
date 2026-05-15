#include "BaseWeapon.h"

namespace Survivor3rdPerson
{
    BaseWeapon::BaseWeapon(float reloadTime, WeaponType wType) : m_reloadTime(reloadTime), weaponType(wType)
    {

    }

    BaseWeapon::~BaseWeapon()
    {

    }
}
