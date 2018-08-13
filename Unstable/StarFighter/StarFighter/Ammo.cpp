#include "Ammo.h"

extern Game* CurrentGame;

using namespace sf;

Ammo::Ammo(GameObject* owner, float speed, int dmg, bool is_piercing, bool can_be_parried, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber, int animationNumber) : GameObject(sf::Vector2f(0, 0), sf::Vector2f(0, 0), textureName, size, origin, frameNumber, animationNumber)
{
	m_ref_speed = speed;
	m_target = NULL;
	m_owner = owner;
	m_visible = false;
	m_dmg = dmg;
	m_is_piercing = is_piercing;
	m_can_be_parried = can_be_parried;
	m_parry_timer = PARRY_BULLET_COOLDOWN;
}

Ammo::Ammo(GameObject* owner, sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber, int animationNumber) : GameObject(position, speed, textureName, size, origin, frameNumber, animationNumber)
{
	m_target = NULL;
	m_owner = owner;
	m_visible = false;
}

Ammo::~Ammo()
{
	printf("");
}

void Ammo::update(sf::Time deltaTime)
{
	//update timers
	m_parry_timer += deltaTime.asSeconds();

	if (m_target)
	{
		SetSpeedForConstantSpeedToDestination(m_target->getPosition(), m_ref_speed);
	}

	UpdateRotation();
	rotate(M_PI);

	GameObject::update(deltaTime);
}

void Ammo::CollisionWithEnemy(GameObject* enemy)
{
	if (m_is_piercing == false)// && m_enemies_tagged.size() > 0)//piercing is the ability to hit several targets with one stroke
	{
		enemy->DealDamage(m_dmg);
		m_GarbageMe = true;
		m_visible = false;
	}
	else
	{
		std::vector<GameObject*>::iterator it = find(m_enemies_tagged.begin(), m_enemies_tagged.end(), enemy);
		if (it == m_enemies_tagged.end())
		{
			m_enemies_tagged.push_back(enemy);

			enemy->DealDamage(m_dmg);
		}
	}
}

void Ammo::CollisionWithBullet(GameObject* enemy_bullet)
{
	Ammo* bullet = (Ammo*)enemy_bullet;

	if (m_can_be_parried)
	{
		m_GarbageMe = true;
		m_visible = false;
	}

	if (bullet->m_can_be_parried)
	{
		bullet->m_GarbageMe = true;
		bullet->m_visible = false;
	}
}