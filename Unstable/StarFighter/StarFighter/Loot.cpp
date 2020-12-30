#include "Loot.h"

extern Game* CurrentGame;

#define THURSTER_LOOT_PROBABILITY	10
#define ARMOR_LOOT_PROBABILITY		10
#define SHIELD_LOOT_PROBABILITY		10
#define AEROBRAKE_LOOT_PROBABILITY	10
#define WEAPON_LOOT_PROBABILITY		30
#define MODULE_LOOT_PROBABILITY		1

Loot::Loot (sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, std::string display_name) : GameObject(position, speed, textureName, size)
{
	m_collider_type = LootObject;
	m_layer = LootLayer;

	 m_visible = true;
	 m_isOnScene = true;
	 m_money = 0;
	 m_display_name = display_name;
}

void Loot::update(sf::Time deltaTime, float hyperspeedMultiplier)
{
	//dialog blocking the update?
	if ((*CurrentGame).m_waiting_for_dialog_validation)
	{
		AnimatedSprite::update(deltaTime);
		return;
	}

	static sf::Vector2f newposition, offset, newspeed;
	newspeed = m_speed;

	float l_hyperspeedMultiplier = hyperspeedMultiplier < 1 ? hyperspeedMultiplier : 1;

	//slowmotion
	newspeed.y += (l_hyperspeedMultiplier - 1) * (*CurrentGame).m_vspeed;

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
	
	return new_loot;
}