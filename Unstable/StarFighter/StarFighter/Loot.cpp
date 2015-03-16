#include "Loot.h"

#define THURSTER_LOOT_PROBABILITY	10
#define ARMOR_LOOT_PROBABILITY		10
#define SHIELD_LOOT_PROBABILITY		10
#define AEROBRAKE_LOOT_PROBABILITY	10
#define WEAPON_LOOT_PROBABILITY		30
#define MODULE_LOOT_PROBABILITY		1

Loot::Loot (sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, std::string m_display_name) : Independant(position, speed, textureName, size)
{
	 this->visible = true;
	 this->isOnScene = true;
	 this->hasEquipmentLoot = false;
	 this->collider_type = IndependantType::LootObject;
	 this->money = 0;
	 this->display_name = m_display_name;
}

Loot* Loot::Clone()
{
	Loot* new_loot = new Loot(this->getPosition(), this->speed, this->textureName, this->m_size, this->display_name);
	new_loot->money = this->getMoney();
	new_loot->equipment_loot = this->equipment_loot;

	return new_loot;
}