#ifndef WEAPON_H_INCLUDED
#define WEAPON_H_INCLUDED

#include <SFML/Graphics.hpp>
#include "GameObject.h"
#include "Game.h"

enum WeaponTypes
{
	Weapon_Katana,
};


class Weapon : public GameObject
{
public:
	Weapon(WeaponTypes type);
	void InitWeapon(sf::Vector2f size, sf::Color color);
	void update(sf::Time deltaTime) override;
	void Draw(sf::RenderTexture& screen) override;
	void CollisionWithEnemy(GameObject* enemy) override;

	void Extend(sf::Vector2f ratio);

	int m_dmg;
	sf::Vector2f m_melee_range;
	float m_melee_duration;

	vector<GameObject*> m_enemies_tagged;

	WeaponTypes m_type;
};

#endif // WEAPON_H_INCLUDED