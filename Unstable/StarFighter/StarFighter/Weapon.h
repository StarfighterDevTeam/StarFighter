#ifndef WEAPON_H_INCLUDED
#define WEAPON_H_INCLUDED

#include <SFML/Graphics.hpp>
#include "GameObject.h"
#include "Game.h"
#include "Ammo.h"

enum WeaponType
{
	Weapon_Katana,
	Weapon_Spear,
	Weapon_Shuriken,
};


class Weapon : public GameObject
{
public:
	Weapon(GameObject* owner, WeaponType type, sf::Color color = sf::Color::Yellow);
	~Weapon();
	
	void update(sf::Time deltaTime) override;
	void Draw(sf::RenderTexture& screen) override;
	void CollisionWithEnemy(GameObject* enemy) override;
	void CollisionWithWeapon(GameObject* enemy_weapon) override;
	void CollisionWithBullet(GameObject* enemy_bullet) override;

	void Extend(sf::Vector2f ratio);
	Ammo* Shoot();
	void Shoot(float angle_rad);
	void Shoot(GameObject* target);

	int m_dmg;
	sf::Vector2f m_range;
	float m_attack_duration;
	sf::Clock m_attack_clock;
	bool m_is_piercing;
	bool m_is_ranged;
	Ammo* m_bullet;
	float m_bullet_speed;
	bool m_bullet_is_following_target;
	bool m_bullet_is_unique;
	bool m_can_be_parried;
	vector<GameObject*> m_enemies_tagged;
	size_t GetFiredBulletsCount();

	WeaponType m_type;
	GameObject* m_owner;
};

#endif // WEAPON_H_INCLUDED