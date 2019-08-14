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

	switch (weapon_type)
	{
		case Weapon_AAM:
		{
			m_rate_of_fire = 1;
			break;
		}
	}
}

Weapon::~Weapon()
{

}


void Weapon::Fire()
{
	sf::Vector2f weapon_pos;
	weapon_pos.x = m_owner->getPosition().x + cos(m_polar_offset.x) * m_polar_offset.y;
	weapon_pos.y = m_owner->getPosition().y - sin(m_polar_offset.x) * m_polar_offset.y;

	Ballistic* shot = new Ballistic(m_ballistic_type, weapon_pos, m_owner->m_alliance, m_owner->m_radar_heading + m_heading_offset);
	(*CurrentGame).m_ballistics.push_back(shot);
	(*CurrentGame).AddCircleObject(shot);

	m_rate_of_fire_timer = m_rate_of_fire;
}

void Weapon::update(sf::Time deltaTime)
{
	if (m_rate_of_fire_timer > 0)
	{
		m_rate_of_fire_timer -= deltaTime.asSeconds();
	}
	else
	{
		m_rate_of_fire_timer = 0;
	}
}

bool Weapon::IsReadyToFire()
{
	bool rate_of_fire = m_rate_of_fire_timer <= 0;

	return rate_of_fire;
}