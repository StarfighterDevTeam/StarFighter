#include "Loot.h"

extern Game* CurrentGame;

using namespace sf;

#define LOOT_BONUS_MELEE_RANGE				100.f
#define LOOT_FLOTATION_FREQUENCY			0.5f // in seconds
#define LOOT_FLOTATION_AMPLITUDE			8.f

#define LOOT_ATTRACTION_ACCELERATION		2000.f
#define LOOT_ATTRACTION_RADIUS				50.f

#define LOOT_EXPIRATION_FEEDBACK_DURATION		4.f

Loot::Loot(LootType type, sf::Vector2f position)
{
	m_type = type;
	m_flotation_timer = 0.f;
	m_attraction_radius = 0.f;
	m_attraction_target = NULL;
	m_expiration_timer = 0.f;
	m_expiration_duration = 10.f;
	setColor(sf::Color(255, 255, 255, 255), true);
	
	switch (type)
	{
		case Loot_BonusMeleeRange:
		{
			Init(position, sf::Vector2f(0, 0), "2D/qi.png", sf::Vector2f(20, 24), 1, 1);
			m_range_bonus = sf::Vector2f(LOOT_BONUS_MELEE_RANGE, 0);
			setColor(sf::Color(255, 0, 0, 255), true);
			break;
		}
		case Loot_WeaponKatana:
		{
			Init(position, sf::Vector2f(0, 0), "2D/katana.png", sf::Vector2f(80, 80), 1, 1);
			m_weapon_type = Weapon_Katana;
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
		case Loot_DashAmmo:
		{
			Init(position, sf::Vector2f(0, 0), "2D/qi.png", sf::Vector2f(20, 24), 1, 1);
			m_dash_ammo = 1;
			setColor(sf::Color(0, 255, 0, 255), true);
			m_attraction_radius = LOOT_ATTRACTION_RADIUS;
			break;
		}
	}
}

void Loot::update(sf::Time deltaTime)
{
	//update timers
	m_flotation_timer += deltaTime.asSeconds();
	m_expiration_timer += deltaTime.asSeconds();

	//reset color
	setColor(m_color);

	//attraction
	if (m_attraction_radius > 0 && m_attraction_target == NULL)
	{
		GameObject* attraction_target = (*CurrentGame).GetClosestObjectTyped(getPosition(), PlayerShip);
		if (attraction_target)
		{
			float range = m_attraction_radius + attraction_target->m_diag + m_diag;

			if (GetDistanceSquaredBetweenPositions(getPosition(), attraction_target->getPosition()) < range* range)
			{
				m_attraction_target = attraction_target;
				SetConstantSpeedToDestination(m_attraction_target->getPosition(), 1.f);
			}
		}
	}

	//oscillation
	if (m_attraction_target == NULL)
	{
		float flotation_offset = LOOT_FLOTATION_AMPLITUDE * sin(m_flotation_timer * M_PI / LOOT_FLOTATION_FREQUENCY);
		m_speed.y = flotation_offset;
	}
	else//attraction
	{
		AddValueToVector(&m_speed, LOOT_ATTRACTION_ACCELERATION * deltaTime.asSeconds());
		SetConstantSpeedToDestination(m_attraction_target->getPosition(), GetAbsoluteSpeed());
		//m_speed.x += ;
		//m_speed.y += LOOT_ATTACTION_ACCELERATION * deltaTime.asSeconds();
		//printf("m_speed.x : %f, m_speed.y: %f\n", m_speed.x, m_speed.y);
		
	}

	//expiration
	if (m_expiration_timer >= m_expiration_duration && m_expiration_duration >= 0 && m_attraction_target == NULL)
	{
		m_GarbageMe = true;
		m_visible = false;
	}

	if (m_expiration_timer >= m_expiration_duration - LOOT_EXPIRATION_FEEDBACK_DURATION)
		setColor(sf::Color(m_color.r, m_color.g, m_color.b, 255 * cos(m_expiration_timer * 10)), false);
	
	GameObject::update(deltaTime);
}