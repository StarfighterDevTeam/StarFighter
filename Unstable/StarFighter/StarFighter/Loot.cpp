#include "Loot.h"

extern Game* CurrentGame;

using namespace sf;

#define LOOT_BONUS_MELEE_RANGE			100.f

Loot::Loot(LootType type, sf::Vector2f position)
{
	m_type = type;
	
	switch (type)
	{
		case Loot_BonusMeleeRange:
		{
			Init(position, sf::Vector2f(0, 0), "2D/qi.png", sf::Vector2f(20, 24), 1, 1);
			m_range_bonus = sf::Vector2f(LOOT_BONUS_MELEE_RANGE, 0);
			break;
		}
		case Loot_WeaponSpear:
		{
			Init(position, sf::Vector2f(0, 0), "2D/spear.png", sf::Vector2f(100, 60), 1, 1);
			m_weapon_type = Weapon_Spear;
			break;
		}
		case Loot_WeaponShuriken:
		{
			Init(position, sf::Vector2f(0, 0), "2D/shuriken.png", sf::Vector2f(100, 100), 1, 1);
			m_weapon_type = Weapon_Shuriken;
			break;
		}
	}
}

void Loot::update(sf::Time deltaTime)
{
	AnimatedSprite::update(deltaTime);
}