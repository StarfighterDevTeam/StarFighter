#include "Wing.h"

extern Game* CurrentGame;

using namespace sf;

Weapon::Weapon(L16Entity* owner, WeaponType weapon_type, BallisticType ballistic_type, sf::Vector2f polar_offset, float heading_offset)
{
	m_owner = owner;
	m_weapon_type = weapon_type;
	m_ballistic_type = ballistic_type;
	m_polar_offset = polar_offset;
	m_heading_offset = heading_offset;
	m_rate_of_fire_timer = 0;

	m_locked_target = NULL;
	m_locking_target_clock = 0;
	m_locking_angle_coverage = 60;

	m_lock_rectangle.setOutlineColor(sf::Color::Green);
	m_lock_rectangle.setOutlineThickness(-2);
	m_lock_rectangle.setFillColor(sf::Color(0, 0, 0, 0));

	switch (weapon_type)
	{
		case Weapon_AAM:
		{
			m_rate_of_fire = 1;
			m_range = 600;
			m_collision_domain = Circle_L16Ballistic_Air;
			break;
		}
	}
}

Weapon::~Weapon()
{

}

void Weapon::Fire()
{
	Ballistic* shot = new Ballistic(m_ballistic_type, m_position, m_owner->m_alliance, m_heading, m_range, m_collision_domain, m_locked_target);
	(*CurrentGame).m_L16_entities.push_back(shot);
	(*CurrentGame).AddCircleObject(shot);

	m_rate_of_fire_timer = m_rate_of_fire;
}

void Weapon::update(sf::Time deltaTime)
{
	//position
	m_position.x = m_owner->getPosition().x + cos(m_polar_offset.x) * m_polar_offset.y;
	m_position.y = m_owner->getPosition().y - sin(m_polar_offset.x) * m_polar_offset.y;

	//heading
	m_heading = m_owner->m_radar_heading + m_heading_offset;

	//rate of fire
	if (m_rate_of_fire_timer > 0)
	{
		m_rate_of_fire_timer -= deltaTime.asSeconds();
	}
	else
	{
		m_rate_of_fire_timer = 0;
	}

	//locking target
	if (m_locked_target == NULL)
	{
		AllianceType alliance = m_owner->m_alliance == PlayerAlliance ? EnemyAlliance : PlayerAlliance;

		L16Entity* locked_target = (*CurrentGame).GetClosestL16Target(m_position, m_heading, m_collision_domain, alliance, m_range, m_locking_angle_coverage / 2);

		if (locked_target != NULL)
		{
			m_locking_target_clock += deltaTime.asSeconds();
			if (m_locking_target_clock >= 1)
			{
				//lock target
				m_locked_target = locked_target;

				m_lock_rectangle.setSize(sf::Vector2f(m_locked_target->getRadius() * 2, m_locked_target->getRadius() * 2));
				m_lock_rectangle.setOrigin(sf::Vector2f(m_locked_target->getRadius(), m_locked_target->getRadius()));
				m_lock_rectangle.setPosition(m_locked_target->getPosition());
			}
		}
		else
		{
			m_locking_target_clock = 0;
		}
	}
	else
	{
		m_lock_rectangle.setPosition(m_locked_target->getPosition());

		if (CanStayLocked(m_locked_target) == false)
		{
			m_locked_target = NULL;
			m_locking_target_clock = 0;
		}
	}
}

bool Weapon::IsReadyToFire()
{
	bool rate_of_fire = m_rate_of_fire_timer <= 0;

	return rate_of_fire;
}

bool Weapon::CanStayLocked(L16Entity* entity)
{
	float angle_delta = GetAngleDegToTargetPosition(m_position, m_heading, entity->getPosition());
	if (abs(angle_delta) > m_locking_angle_coverage)
	{
		return false;
	}

	float dist_squared = GetDistanceSquaredBetweenPositions(m_position, entity->getPosition());
	if (dist_squared > m_range * m_range)
	{
		return false;
	}

	return true;
}