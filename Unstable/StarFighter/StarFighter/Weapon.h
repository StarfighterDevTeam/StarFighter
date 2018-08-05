#ifndef WEAPON_H_INCLUDED
#define WEAPON_H_INCLUDED

#include <SFML/Graphics.hpp>
#include "GameObject.h"
#include "Game.h"

enum WeaponTypes
{
	Weapon_Katana,
	Weapon_Spear,
	Weapon_Shuriken,
};


class Weapon : public GameObject
{
public:
	Weapon(GameObject* owner, WeaponTypes type, sf::Color color = sf::Color::Yellow);
	void InitWeapon(sf::Vector2f size, sf::Color color);
	void update(sf::Time deltaTime) override;
	void Draw(sf::RenderTexture& screen) override;
	void CollisionWithEnemy(GameObject* enemy) override;
	void CollisionBetweenWeapons(GameObject* enemy_weapon) override;

	void Extend(sf::Vector2f ratio);

	int m_dmg;
	sf::Vector2f m_melee_range;
	float m_melee_duration;
	sf::Clock m_melee_clock;
	bool m_piercing;
	bool m_ranged;

	vector<GameObject*> m_enemies_tagged;

	WeaponTypes m_type;
	GameObject* m_owner;
};

#endif // WEAPON_H_INCLUDED