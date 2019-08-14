#include "Wing.h"

extern Game* CurrentGame;

using namespace sf;

Weapon::Weapon(L16Entity* owner, WeaponType type, BallisticType ballistic_type, sf::Vector2f polar_offset, float heading_offset)
{
	m_owner = owner;
	m_type = type;
	m_ballistic_type = ballistic_type;
	m_polar_offset = polar_offset;
	m_heading_offset = heading_offset;
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
}
