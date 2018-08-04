#include "Enemy.h"

extern Game* CurrentGame;

using namespace sf;

Enemy::Enemy(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber, int animationNumber) : GameObject(position, speed, textureName, size, origin, frameNumber, animationNumber)
{
	m_hp_max = 2;
	m_hp = m_hp_max;
	m_dmg = 1;
	m_melee_cooldown = 2.f;

	m_is_attacking = false;
	m_melee_weapon = new Weapon(this, Weapon_Katana, sf::Color::Red);
	(*CurrentGame).addToScene(m_melee_weapon, EnemyMeleeWeaponLayer, EnemyMeleeWeapon);

	float angle = RandomizeFloatBetweenValues(sf::Vector2f(0, 360));
	//SetSpeedVectorFromAbsoluteSpeedAndAngle(200, angle * M_PI / 180);
}

Enemy::~Enemy()
{
	GameObject* playerShip = (GameObject*)(*CurrentGame).m_playerShip;
	if (playerShip->GetDashEnemy() == this)
	{
		playerShip->SetDashEnemy(NULL);
	}

	if (m_melee_weapon)
	{
		m_melee_weapon->m_GarbageMe = true;
		m_melee_weapon->m_visible = false;
	}
}

void Enemy::update(sf::Time deltaTime)
{
	//bounce on screen borders
	bool bounced = BounceOnBorders((*CurrentGame).m_map_size);

	//Melee
	bool starting_melee_attacking = false;
	if (m_is_attacking)//reset
	{
		if (m_melee_weapon && m_melee_weapon->m_melee_clock.getElapsedTime().asSeconds() > m_melee_weapon->m_melee_duration)
		{
			m_is_attacking = false;
			m_melee_weapon->m_visible = false;
			m_melee_weapon->Extend(sf::Vector2f(0.f, 1.f));
			m_melee_weapon->m_enemies_tagged.clear();
			m_melee_weapon->setColor(m_melee_weapon->m_color);

			m_melee_cooldown_clock.restart();
		}
	}

	if (m_melee_cooldown_clock.getElapsedTime().asSeconds() > m_melee_cooldown)//condition for attack
	{
		if (m_melee_weapon && !m_is_attacking)
		{
			m_is_attacking = true;
			starting_melee_attacking = true;
			m_melee_weapon->m_visible = true;
			m_melee_weapon->m_melee_clock.restart();
		}
	}

	if (m_is_attacking)//update
	{
		float ratio = m_melee_weapon->m_melee_clock.getElapsedTime().asSeconds() / m_melee_weapon->m_melee_duration;
		if (ratio > 1.0f)
		{
			ratio = 1.0f;
		}
		m_melee_weapon->Extend(sf::Vector2f(ratio, 1.f));
	}

	UpdateWeaponPosition(m_melee_weapon);

	GameObject::update(deltaTime);
}

bool Enemy::DealDamage(int dmg)
{
	m_hp -= dmg;

	if (m_hp <= 0)
	{
		Death();
		return true;
	}

	return false;
}

void Enemy::Death()
{
	m_GarbageMe = true;
	m_visible = false;
}

int Enemy::GetRating()
{
	int rating = 0;

	rating += m_hp_max;
	rating += m_dmg;

	return rating;
}