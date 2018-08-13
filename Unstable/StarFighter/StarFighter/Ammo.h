#ifndef AMMO_H_INCLUDED
#define AMMO_H_INCLUDED

#include "Globals.h"
#include "Game.h"
#include "GameObject.h"
#include "FX.h"

#define PARRY_BULLET_COOLDOWN			0.2f

class Ammo : public GameObject
{
public :
	Ammo(GameObject* owner, float speed, int dmg, bool is_piercing, bool can_be_parried, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber = 1, int animationNumber = 1);
	Ammo(GameObject* owner, sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber = 1, int animationNumber = 1);
	~Ammo();
	void update(sf::Time deltaTime) override;

	void CollisionWithEnemy(GameObject* enemy) override;

	GameObject* m_owner;
	GameObject* m_target;
	float m_ref_speed;
	int m_dmg;
	bool m_is_piercing;
	bool m_can_be_parried;
	vector<GameObject*> m_enemies_tagged;

	float m_parry_timer;
};

#endif // AMMO_H_INCLUDED
