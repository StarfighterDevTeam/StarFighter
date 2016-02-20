#include "Loot.h"

extern Game* CurrentGame;

#define THURSTER_LOOT_PROBABILITY	10
#define ARMOR_LOOT_PROBABILITY		10
#define SHIELD_LOOT_PROBABILITY		10
#define AEROBRAKE_LOOT_PROBABILITY	10
#define WEAPON_LOOT_PROBABILITY		30
#define MODULE_LOOT_PROBABILITY		1

Loot::Loot (sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, std::string m_display_name) : GameObject(position, speed, textureName, size)
{
	 m_visible = true;
	 m_isOnScene = true;
	 m_collider_type = GameObjectType::LootObject;
	 m_money = 0;
	 m_display_name = m_display_name;
}

void Loot::update(sf::Time deltaTime, float hyperspeedMultiplier)
{
	static sf::Vector2f newposition, offset, newspeed;
	newspeed = m_speed;

	if (hyperspeedMultiplier > 1)
	{
		newspeed.x += GameObject::getSpeed_for_Scrolling((*CurrentGame).m_direction, (hyperspeedMultiplier - 1) * (*CurrentGame).m_vspeed).x;
		newspeed.y += GameObject::getSpeed_for_Scrolling((*CurrentGame).m_direction, (hyperspeedMultiplier - 1) * (*CurrentGame).m_vspeed).y;
	}
	else if (hyperspeedMultiplier < 1)
	{
		newspeed.x = m_speed.x * hyperspeedMultiplier;
		newspeed.y = m_speed.y * hyperspeedMultiplier;
	}

	setGhost(hyperspeedMultiplier > 1.0f);

	//Basic movement (initial vector)
	newposition.x = getPosition().x + (newspeed.x)*deltaTime.asSeconds();
	newposition.y = getPosition().y + (newspeed.y)*deltaTime.asSeconds();

	setPosition(newposition.x, newposition.y);

	AnimatedSprite::update(deltaTime);
}


Loot* Loot::Clone()
{
	Loot* new_loot = new Loot(this->getPosition(), this->m_speed, this->m_textureName, this->m_size, this->m_display_name);
	new_loot->m_money = this->m_money;
	new_loot->m_equipment_loot = this->m_equipment_loot;

	return new_loot;
}