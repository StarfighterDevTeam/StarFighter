#include "Loot.h"

#define THURSTER_LOOT_PROBABILITY	10
#define ARMOR_LOOT_PROBABILITY		10
#define SHIELD_LOOT_PROBABILITY		10
#define AEROBRAKE_LOOT_PROBABILITY	10
#define WEAPON_LOOT_PROBABILITY		30
#define MODULE_LOOT_PROBABILITY		1

Loot::Loot (sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size) : Independant(position, speed, textureName, size)
{
	 visible = true;
	 isOnScene = true;
	 hasEquipmentLoot = false;
	 collider_type = IndependantType::LootObject;
	 money = 0;
}

Loot* Loot::Clone()
{
	Loot* new_loot = new Loot(this->getPosition(), this->speed, this->textureName, this->m_size);
	new_loot->money = this->getMoney();
	new_loot->loot_equipment = this->loot_equipment;

	return new_loot;
}